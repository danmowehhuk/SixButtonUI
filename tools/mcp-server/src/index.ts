#!/usr/bin/env node
// MCP server entry point. Owns one persistent serial connection to a
// SixButtonUI device running with SIXBUTTONUI_ENABLE_MCP, and exposes
// three tools: press a button, read the last-known UI state, and check
// connection status.
//
// Config is via environment variables (see README.md):
//   SIXBUTTON_SERIAL_PATH  - e.g. /dev/cu.usbserial-XXXX (required)
//   SIXBUTTON_BAUD_RATE    - defaults to 115200
//   SIXBUTTON_DEBUG_LOG    - defaults to ./sixbutton-mcp-debug.log

import { McpServer } from "@modelcontextprotocol/sdk/server/mcp.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import { z } from "zod";
import { SixButtonConnection } from "./serial.js";

const SERIAL_PATH = process.env.SIXBUTTON_SERIAL_PATH;
const BAUD_RATE = Number(process.env.SIXBUTTON_BAUD_RATE ?? "115200");
const DEBUG_LOG_PATH = process.env.SIXBUTTON_DEBUG_LOG ?? "./sixbutton-mcp-debug.log";

if (!SERIAL_PATH) {
  console.error(
    "SIXBUTTON_SERIAL_PATH is not set. Point it at the device's serial port (e.g. /dev/cu.usbserial-XXXX) and restart.",
  );
  process.exit(1);
}

const BUTTONS = ["UP", "DN", "LF", "RT", "EN", "ME"] as const;
type Button = (typeof BUTTONS)[number];

/** Translates a button + long/short flag into the wire code (e.g. "UP-L"). */
function toWireCode(button: Button, long: boolean): string {
  return long ? `${button}-L` : button;
}

const connection = new SixButtonConnection(SERIAL_PATH, BAUD_RATE, DEBUG_LOG_PATH);

const server = new McpServer({
  name: "sixbutton-mcp-server",
  version: "0.1.0",
});

server.registerTool(
  "press",
  {
    title: "Press a SixButtonUI button",
    description:
      "Simulates a physical button press on the live SixButtonUI device (UP/DN/LF/RT/EN/ME), " +
      "optionally as a long press. The server sends the press followed immediately by the " +
      "matching release, matching how a real button press behaves. If no handler is registered " +
      "for that action on the current screen, nothing happens - this is not an error.",
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
      return {
        isError: true,
        content: [{ type: "text", text: err instanceof Error ? err.message : String(err) }],
      };
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
      "navigation/selection flags. This is the last state the device pushed, not a live poll - " +
      "call `press` first if you need to react to a fresh screen.",
    inputSchema: {},
    annotations: {
      readOnlyHint: true,
      destructiveHint: false,
      idempotentHint: true,
      openWorldHint: true,
    },
  },
  async () => {
    const { viewModel, receivedAt } = connection.getLastViewModel();
    if (!viewModel) {
      return {
        content: [
          {
            type: "text",
            text: "No ViewModel has been received yet. Press a button or wait for the device to render.",
          },
        ],
      };
    }
    return {
      content: [
        {
          type: "text",
          text: JSON.stringify({ receivedAt, ...viewModel }, null, 2),
        },
      ],
    };
  },
);

server.registerTool(
  "status",
  {
    title: "Check the serial connection status",
    description:
      "Reports whether the server currently holds a live serial connection to the device, " +
      "which port/baud rate it's using, the last error (if any), and when a ViewModel was " +
      "last received.",
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
  try {
    await connection.connect();
  } catch (err) {
    console.error(
      `Failed to open ${SERIAL_PATH}: ${err instanceof Error ? err.message : String(err)}`,
    );
    console.error("The server will still start; tool calls will report the connection error.");
  }

  const transport = new StdioServerTransport();
  await server.connect(transport);
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
