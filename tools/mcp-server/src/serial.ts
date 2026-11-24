// Owns the serial connection to the SixButtonUI-based device for one test
// session at a time: connect() opens the port, which resets the board, and
// waits for its first screen; disconnect() releases the port so a flasher,
// a serial monitor or another session can use it. Within a session it
// writes "6BUI: <CODE>\n" press commands and routes each incoming line to
// the cached ViewModel (a "6BUI->'...'" line), the cached display text for
// that ViewModel (a "6BUI=>'...'" line), or a debug log file (everything
// else the device happens to print on the same wire).

import { SerialPort } from "serialport";
import { ReadlineParser } from "@serialport/parser-readline";
import { appendFileSync } from "node:fs";
import { calloutPath, chooseBoard, type BoardPin, type PortInfo } from "./boardFinder.js";
import { findPortHolder } from "./portHolder.js";
import { parseDisplayLine } from "./display.js";
import { parseViewModelLine, type ViewModel } from "./viewmodel.js";

export interface ConnectionOptions {
  baudRate: number;
  debugLogPath: string;
  /** Picks a board when several are plugged in. */
  pin: BoardPin;
  /** How long connect waits for the board's first screen after its reset. */
  firstViewModelTimeoutMs: number;
  listPorts?: () => Promise<PortInfo[]>;
  findHolder?: (path: string) => Promise<string | null>;
}

export interface ConnectionStatus {
  sessionOpen: boolean;
  path: string | null;
  baudRate: number;
  lastError: string | null;
  lastViewModelAt: string | null;
}

export interface Session {
  path: string;
  viewModel: ViewModel;
}

export class SixButtonConnection {
  private port: SerialPort | null = null;
  private path: string | null = null;
  private closing = false;
  private lastViewModel: ViewModel | null = null;
  private lastViewModelAt: string | null = null;
  // The rows the render function drew for lastViewModel, or null until
  // they arrive (never, for an app whose render function doesn't send them).
  private lastDisplay: string[] | null = null;
  private lastError: string | null = null;
  private wakeTimer: NodeJS.Timeout | null = null;
  private viewModelWaiter: ((vm: ViewModel) => void) | null = null;
  private readonly listPorts: () => Promise<PortInfo[]>;
  private readonly findHolder: (path: string) => Promise<string | null>;

  constructor(private readonly options: ConnectionOptions) {
    this.listPorts = options.listPorts ?? (() => SerialPort.list());
    this.findHolder = options.findHolder ?? findPortHolder;
  }

  /**
   * Starts a session on port, or on the board chooseBoard() finds. Fails
   * at once if a session is open or another process holds the port.
   */
  async connect(port?: string): Promise<Session> {
    if (this.port) {
      throw new Error(`A test session is already open on ${this.path}. Call disconnect first.`);
    }
    const paths = port
      ? [calloutPath(port)]
      : chooseBoard(await this.listPorts(), this.options.pin);
    this.lastViewModel = null;
    this.lastViewModelAt = null;
    this.lastDisplay = null;

    const failures: string[] = [];
    for (const path of paths) {
      const holder = await this.findHolder(path);
      if (holder) {
        throw new Error(
          `${path} is in use by ${holder}. Wait for it to finish, or close it, then connect again.`,
        );
      }
      try {
        await this.open(path);
      } catch (err) {
        failures.push(`${path}: ${message(err)}`);
        continue;
      }
      const viewModel = await this.nextViewModel(this.options.firstViewModelTimeoutMs);
      if (viewModel) {
        this.lastError = null;
        return { path, viewModel };
      }
      await this.close();
      failures.push(
        `${path}: no SixButtonUI screen within ${this.options.firstViewModelTimeoutMs} ms`,
      );
    }
    this.lastError = `Couldn't start a session. ${failures.join("; ")}.`;
    throw new Error(
      `${this.lastError} Is the firmware built with SIXBUTTONUI_ENABLE_MCP, at ` +
        `${this.options.baudRate} baud?`,
    );
  }

  /** Ends the session. Returns false if none was open. */
  async disconnect(): Promise<boolean> {
    if (!this.port) return false;
    await this.close();
    return true;
  }

  /** Writes "6BUI: <CODE>\n". Rejects outside a session. */
  async writeCode(code: string): Promise<void> {
    const port = this.port;
    if (!port) {
      const reason = this.lastError ? ` Last error: ${this.lastError}` : "";
      throw new Error(`No test session is open. Call connect first.${reason}`);
    }
    await new Promise<void>((resolve, reject) => {
      port.write(`6BUI: ${code}\n`, (err) => {
        if (err) reject(err);
        else resolve();
      });
    });
  }

  getLastViewModel(): {
    viewModel: ViewModel | null;
    display: string[] | null;
    receivedAt: string | null;
  } {
    return {
      viewModel: this.lastViewModel,
      display: this.lastDisplay,
      receivedAt: this.lastViewModelAt,
    };
  }

  getStatus(): ConnectionStatus {
    return {
      sessionOpen: this.port !== null,
      path: this.path,
      baudRate: this.options.baudRate,
      lastError: this.lastError,
      lastViewModelAt: this.lastViewModelAt,
    };
  }

  /** Opens path with a flock, so other flock users (e.g. a second server) can't. */
  private open(path: string): Promise<void> {
    return new Promise((resolve, reject) => {
      const port = new SerialPort({ path, baudRate: this.options.baudRate, lock: true }, (err) => {
        if (err) {
          reject(err);
          return;
        }
        this.port = port;
        this.path = path;
        // On macOS (serialport 12 and 13), incoming serial data doesn't
        // wake an otherwise idle event loop - it sits unread until some
        // other event (e.g. the next MCP request) runs the loop. A
        // periodic no-op timer keeps the loop turning so lines are read
        // as they arrive. unref() so it never keeps the process alive.
        this.wakeTimer = setInterval(() => {}, 10);
        this.wakeTimer.unref();
        resolve();
      });

      port.on("error", (err) => {
        this.lastError = err.message;
      });

      port.on("close", () => {
        if (!this.closing) {
          this.lastError = `The session on ${this.path} ended: the port closed (unplugged or reset?).`;
        }
        this.ended(port);
      });

      const lines = port.pipe(new ReadlineParser({ delimiter: "\n" }));
      lines.on("data", (rawLine: string) => this.handleLine(rawLine));
    });
  }

  private close(): Promise<void> {
    const port = this.port;
    if (!port) return Promise.resolve();
    this.closing = true;
    return new Promise((resolve) => {
      port.close(() => {
        this.ended(port);
        this.closing = false;
        resolve();
      });
    });
  }

  /** Forgets port once it's closed, however that happened. */
  private ended(port: SerialPort): void {
    if (this.port !== port) return;
    this.port = null;
    this.path = null;
    if (this.wakeTimer) clearInterval(this.wakeTimer);
    this.wakeTimer = null;
  }

  /** The next ViewModel the device sends, or null after timeoutMs. */
  private nextViewModel(timeoutMs: number): Promise<ViewModel | null> {
    return new Promise((resolve) => {
      const timer = setTimeout(() => {
        this.viewModelWaiter = null;
        resolve(null);
      }, timeoutMs);
      this.viewModelWaiter = (vm) => {
        clearTimeout(timer);
        this.viewModelWaiter = null;
        resolve(vm);
      };
    });
  }

  private handleLine(rawLine: string): void {
    const line = rawLine.replace(/\r$/, "");
    const vm = parseViewModelLine(line);
    if (vm) {
      this.lastViewModel = vm;
      this.lastViewModelAt = new Date().toISOString();
      this.lastDisplay = null;
      this.viewModelWaiter?.(vm);
      return;
    }
    const display = parseDisplayLine(line);
    if (display) {
      this.lastDisplay = display;
      return;
    }
    if (line.trim().length === 0) return;
    appendFileSync(this.options.debugLogPath, `[${new Date().toISOString()}] ${line}\n`);
  }
}

function message(err: unknown): string {
  return err instanceof Error ? err.message : String(err);
}
