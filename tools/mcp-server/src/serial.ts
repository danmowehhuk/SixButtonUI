// Owns the serial connection to the SixButtonUI-based device: writes
// "6BUI: <CODE>\n" press commands, reads incoming lines, and routes
// each line to either the cached ViewModel (if it's a "6BUI->'...'"
// line) or a debug log file (everything else the device happens to
// print on the same wire).
//
// Opens the port once at startup and holds it for the server's whole
// lifetime, deliberately - reopening a serial port resets the board
// (DTR auto-reset), so a tool that reopened the port per call would
// interrupt whatever the device was doing every single time.

import { SerialPort } from "serialport";
import { ReadlineParser } from "@serialport/parser-readline";
import { appendFileSync } from "node:fs";
import { parseViewModelLine, type ViewModel } from "./viewmodel.js";

export interface ConnectionStatus {
  connected: boolean;
  path: string;
  baudRate: number;
  lastError: string | null;
  lastViewModelAt: string | null;
}

export class SixButtonConnection {
  private port: SerialPort | null = null;
  private lastViewModel: ViewModel | null = null;
  private lastViewModelAt: string | null = null;
  private lastError: string | null = null;
  private connected = false;

  constructor(
    private readonly path: string,
    private readonly baudRate: number,
    private readonly debugLogPath: string,
  ) {}

  /** Opens the port once. Resolves once the port is open; does not
   *  wait for the device's own boot-time render. */
  connect(): Promise<void> {
    return new Promise((resolve, reject) => {
      const port = new SerialPort({ path: this.path, baudRate: this.baudRate }, (err) => {
        if (err) {
          this.lastError = err.message;
          this.connected = false;
          reject(err);
          return;
        }
        this.connected = true;
        this.lastError = null;
        resolve();
      });

      port.on("error", (err) => {
        this.lastError = err.message;
        this.connected = false;
      });

      port.on("close", () => {
        this.connected = false;
      });

      const lines = port.pipe(new ReadlineParser({ delimiter: "\n" }));
      lines.on("data", (rawLine: string) => this.handleLine(rawLine));

      this.port = port;
    });
  }

  private handleLine(rawLine: string): void {
    const line = rawLine.replace(/\r$/, "");
    const vm = parseViewModelLine(line);
    if (vm) {
      this.lastViewModel = vm;
      this.lastViewModelAt = new Date().toISOString();
      return;
    }
    if (line.trim().length === 0) return;
    appendFileSync(this.debugLogPath, `[${new Date().toISOString()}] ${line}\n`);
  }

  /** Writes "6BUI: <CODE>\n". Rejects if the port isn't currently open. */
  async writeCode(code: string): Promise<void> {
    if (!this.port || !this.connected) {
      throw new Error(
        `Not connected to ${this.path} - the device may be unplugged, or the server needs restarting. Last error: ${this.lastError ?? "none recorded"}`,
      );
    }
    await new Promise<void>((resolve, reject) => {
      this.port!.write(`6BUI: ${code}\n`, (err) => {
        if (err) reject(err);
        else resolve();
      });
    });
  }

  getLastViewModel(): { viewModel: ViewModel | null; receivedAt: string | null } {
    return { viewModel: this.lastViewModel, receivedAt: this.lastViewModelAt };
  }

  getStatus(): ConnectionStatus {
    return {
      connected: this.connected,
      path: this.path,
      baudRate: this.baudRate,
      lastError: this.lastError,
      lastViewModelAt: this.lastViewModelAt,
    };
  }
}
