# sixbutton-mcp-server

Local stdio MCP server that lets Claude drive a live SixButtonUI-based
device over a serial/UART bridge, using the `SIXBUTTONUI_ENABLE_MCP` wire
protocol built into SixButtonUI. Works with any SixButtonUI app built
with that flag - nothing here is specific to one particular sketch.

It holds one persistent serial connection for its whole lifetime -
reopening the port resets the board (DTR auto-reset), so this server
opens it once at startup and never reopens it per call.

Non-protocol lines the device happens to print on the same wire (debug
output, etc.) are appended to a local log file rather than surfaced as
a tool, so they stay `tail -f`-able without polluting tool responses.

## Prerequisites

- The device's firmware must be built with `SIXBUTTONUI_ENABLE_MCP`
  defined (see SixButtonUI's README).
- Node.js 18+.

## Setup

```sh
cd tools/mcp-server
npm install
npm run build
```

## Configuration

Set via environment variables:

| Variable | Required | Default | Meaning |
|---|---|---|---|
| `SIXBUTTON_SERIAL_PATH` | yes | - | Serial device path, e.g. `/dev/cu.usbserial-XXXX` |
| `SIXBUTTON_BAUD_RATE` | no | `115200` | Must match the firmware's `Serial.begin(...)` rate |
| `SIXBUTTON_DEBUG_LOG` | no | `./sixbutton-mcp-debug.log` | Where non-protocol serial lines are appended |

## Registering with Claude Code

```sh
claude mcp add sixbutton -- \
  env SIXBUTTON_SERIAL_PATH=/dev/cu.usbserial-XXXX \
  node /absolute/path/to/tools/mcp-server/dist/index.js
```

Adjust the serial path and add `SIXBUTTON_BAUD_RATE`/`SIXBUTTON_DEBUG_LOG`
`env` entries if you need non-default values.

## Tools

- **`press(button, long?)`** - `button` is one of `UP`/`DN`/`LF`/`RT`/`EN`/`ME`;
  `long` (default `false`) sends the long-press variant. Sends a press
  immediately followed by a release, matching a real physical press. A
  no-op if the current screen has no handler for that action.
- **`read_view_model()`** - returns the most recently received UI state
  (screen type, title, instructions, footer, cursor, nav/selection
  flags). This is the last state the device pushed, not a live poll.
- **`status()`** - connection health: whether the port is open, path,
  baud rate, last error, and when a ViewModel was last received.

## If the connection drops

This server does not auto-reconnect - a dropped serial connection (cable
unplugged, device reset) is reported via `status()` and surfaced as an
error from `press`/`read_view_model`, but the server won't retry the
port on its own. Restart the MCP connection (or the whole server) once
the device is back.
