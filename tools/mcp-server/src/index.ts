#!/usr/bin/env node
// MCP server entry point. Drives a SixButtonUI device running with
// SIXBUTTONUI_ENABLE_MCP in test sessions: connect starts one (opening the
// port and resetting the board), press and read_view_model work within it,
// and disconnect releases the port. status reports the session.
//
// Config is via environment variables (see README.md), all optional:
//   SIXBUTTON_SERIAL_NUMBER - picks a board when several are plugged in
//   SIXBUTTON_SERIAL_PATH   - or pins a device path
//   SIXBUTTON_BAUD_RATE     - defaults to 9600
//   SIXBUTTON_CONNECT_TIMEOUT_MS - wait for the first screen, default 5000
//   SIXBUTTON_DEBUG_LOG     - defaults to ./sixbutton-mcp-debug.log

import { McpServer } from "@modelcontextprotocol/sdk/server/mcp.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import { z } from "zod";
import { SixButtonConnection } from "./serial.js";

const BAUD_RATE = Number(process.env.SIXBUTTON_BAUD_RATE ?? "9600");
const CONNECT_TIMEOUT_MS = Number(process.env.SIXBUTTON_CONNECT_TIMEOUT_MS ?? "5000");
const DEBUG_LOG_PATH = process.env.SIXBUTTON_DEBUG_LOG ?? "./sixbutton-mcp-debug.log";

const BUTTONS = ["UP", "DN", "LF", "RT", "EN", "ME"] as const;
type Button = (typeof BUTTONS)[number];

/** Translates a button + long/short flag into the wire code (e.g. "UP-L"). */
function toWireCode(button: Button, long: boolean): string {
  return long ? `${button}-L` : button;
}

const connection = new SixButtonConnection({
  baudRate: BAUD_RATE,
  debugLogPath: DEBUG_LOG_PATH,
  pin: {
    path: process.env.SIXBUTTON_SERIAL_PATH || undefined,
    serialNumber: process.env.SIXBUTTON_SERIAL_NUMBER || undefined,
  },
  firstViewModelTimeoutMs: CONNECT_TIMEOUT_MS,
});

function errorResult(err: unknown) {
  return {
    isError: true,
    content: [{ type: "text" as const, text: err instanceof Error ? err.message : String(err) }],
  };
}

const server = new McpServer({
  name: "sixbutton-mcp-server",
  version: "0.1.0",
});

server.registerTool(
  "connect",
  {
    title: "Start a test session on the SixButtonUI device",
    description:
      "Starts a test session: finds the board (the one USB serial port plugged in, unless " +
      "`port` or the server's configuration picks one), opens its serial port, which resets " +
      "the board, and waits for its first screen, which it returns. Fails at once if a session " +
      "is already open or another process holds the port, naming that process. Call " +
      "`disconnect` when the session is done so builds and other sessions can use the board.",
    inputSchema: {
      port: z
        .string()
        .optional()
        .describe("Device path to use, e.g. /dev/cu.usbserial-110, when several boards are plugged in."),
    },
    annotations: {
      readOnlyHint: false,
      destructiveHint: false,
      idempotentHint: false,
      openWorldHint: true,
    },
  },
  async ({ port }) => {
    try {
      const session = await connection.connect(port);
      return {
        content: [
          {
            type: "text",
            text: JSON.stringify({ connected: session.path, ...session.viewModel }, null, 2),
          },
        ],
      };
    } catch (err) {
      return errorResult(err);
    }
  },
);

server.registerTool(
  "disconnect",
  {
    title: "End the test session",
    description:
      "Ends the test session and releases the serial port, so a build can flash the board or " +
      "another session can connect. Call it when a session is done, including after a failure.",
    inputSchema: {},
    annotations: {
      readOnlyHint: false,
      destructiveHint: false,
      idempotentHint: true,
      openWorldHint: true,
    },
  },
  async () => {
    const ended = await connection.disconnect();
    return {
      content: [
        { type: "text", text: ended ? "Session ended; the port is free." : "No session was open." },
      ],
    };
  },
);

server.registerTool(
  "press",
  {
    title: "Press a SixButtonUI button",
    description:
      "Simulates a physical button press on the live SixButtonUI device (UP/DN/LF/RT/EN/ME), " +
      "optionally as a long press. The server sends the press followed immediately by the " +
      "matching release, matching how a real button press behaves. If no handler is registered " +
      "for that action on the current screen, nothing happens - this is not an error. " +
      "Needs a session; see `connect`.",
    inputSchema: {
      button: z.enum(BUTTONS).describe("Which button to press: UP, DN, LF, RT, EN, or ME."),
      long: z
        .boolean()
        .optional()
        .describe("True for a long press. Defaults to false (a regular press)."),
    },
    annotations: {
      readOnlyHint: false,
      destructiveHint: false,
      idempotentHint: false,
      openWorldHint: true,
    },
  },
  async ({ button, long }) => {
    const code = toWireCode(button, long ?? false);
    try {
      await connection.writeCode(code);
    } catch (err) {
      return errorResult(err);
    }
    return {
      content: [{ type: "text", text: `Sent ${code}.` }],
    };
  },
);

server.registerTool(
  "read_view_model",
  {
    title: "Read the current SixButtonUI screen state",
    description:
      "Returns the most recently received UI state (ViewModel) reported by the device: the " +
      "current screen's type, title, instructions, footer, cursor position, and available " +
      "navigation/selection flags. `display` holds the rows of text the app's render function " +
      "drew for that state (cut to fit, with any marks or arrows it adds; a character outside " +
      "printable ASCII shows as \\xHH, its code); it's null if the app doesn't send them or " +
      "drew nothing, or they haven't arrived yet. " +
      "This is the last state the device pushed in this session, not a live poll - call " +
      "`press` first if you need to react to a fresh screen.",
    inputSchema: {},
    annotations: {
      readOnlyHint: true,
      destructiveHint: false,
      idempotentHint: true,
      openWorldHint: true,
    },
  },
  async () => {
    const { viewModel, display, receivedAt } = connection.getLastViewModel();
    if (!viewModel) {
      return {
        content: [
          {
            type: "text",
            text: connection.getStatus().sessionOpen
              ? "No ViewModel has been received yet. Press a button or wait for the device to render."
              : "No test session is open. Call connect first.",
          },
        ],
      };
    }
    return {
      content: [
        {
          type: "text",
          text: JSON.stringify({ receivedAt, ...viewModel, display }, null, 2),
        },
      ],
    };
  },
);

server.registerTool(
  "status",
  {
    title: "Check the test session",
    description:
      "Reports whether a test session is open, which port and baud rate it uses, the last " +
      "error (if any), and when a ViewModel was last received.",
    inputSchema: {},
    annotations: {
      readOnlyHint: true,
      destructiveHint: false,
      idempotentHint: true,
      openWorldHint: true,
    },
  },
  async () => {
    return {
      content: [{ type: "text", text: JSON.stringify(connection.getStatus(), null, 2) }],
    };
  },
);

async function main() {
  const transport = new StdioServerTransport();
  await server.connect(transport);
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
