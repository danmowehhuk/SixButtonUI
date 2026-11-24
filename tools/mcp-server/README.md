# sixbutton-mcp-server

Local stdio MCP server that lets Claude drive a live SixButtonUI-based
device over a serial/UART bridge, using the 6BUI wire protocol built
into SixButtonUI. Works with any SixButtonUI app built with
`SIXBUTTONUI_ENABLE_MCP` - nothing here is specific to one particular
sketch.

It works in test sessions. `connect` opens the board's serial port,
which resets the board (DTR auto-reset), so every session starts from a
fresh boot; `disconnect` releases the port so the board can be flashed,
monitored or driven by another session. The server holds no port between
sessions, so any number of servers can be running against one board as
long as only one is in a session.

Non-protocol lines the device happens to print on the same wire (debug
output, etc.) are appended to a local log file rather than surfaced as
a tool, so they stay `tail -f`-able without polluting tool responses.

## Prerequisites

- The device's firmware must be built with `SIXBUTTONUI_ENABLE_MCP`
  defined. The protocol uses UART0 (`Serial` on Arduino). Under
  `NO_ARDUINO` the firmware must call `BareMetalHAL::Uart0::enableRx()`
  after `Uart0::begin()`.
- Node.js 18+.

## Setup

```sh
cd tools/mcp-server
npm install
npm run build
```

`npm test` builds and runs the tests (they use `python3` to create a
pseudo-terminal standing in for the device).

## Finding the board

`connect` looks for USB serial ports: serial ports with a USB vendor id,
which leaves out ports like Bluetooth or a debug console. Entries that
share a serial number are one device (some bridges appear under two
drivers' names) and are tried in turn until one sends a screen.

- One USB serial device plugged in: that's the board. No configuration
  is needed, and it can move to a different port between sessions.
- Several: `connect` fails and lists them. Pick one with its `port`
  argument, `SIXBUTTON_SERIAL_NUMBER` or `SIXBUTTON_SERIAL_PATH`.

`connect` fails at once, without waiting, if this server already has a
session open or another process has the port open (it names the
process). It never stops another process.

## Configuration

Set via environment variables, all optional:

| Variable | Default | Meaning |
|---|---|---|
| `SIXBUTTON_SERIAL_NUMBER` | - | USB serial number of the board, when several are plugged in |
| `SIXBUTTON_SERIAL_PATH` | - | Or a fixed device path, e.g. `/dev/cu.usbserial-XXXX` |
| `SIXBUTTON_BAUD_RATE` | `9600` | Must match the firmware's `Serial.begin(...)` rate |
| `SIXBUTTON_CONNECT_TIMEOUT_MS` | `5000` | How long `connect` waits for the first screen after the reset |
| `SIXBUTTON_DEBUG_LOG` | `./sixbutton-mcp-debug.log` | Where non-protocol serial lines are appended |

## Registering with Claude Code

```sh
claude mcp add sixbutton -- node /absolute/path/to/tools/mcp-server/dist/index.js
```

Add `env` entries (e.g. `-- env SIXBUTTON_BAUD_RATE=115200 node ...`) if
you need non-default values.

## Tools

- **`connect(port?)`** - starts a test session: finds the board, opens
  its port (resetting it), and returns its first screen.
- **`disconnect()`** - ends the session and releases the port. Call it
  when the session is done.
- **`press(button, long?)`** - `button` is one of `UP`/`DN`/`LF`/`RT`/`EN`/`ME`;
  `long` (default `false`) sends the long-press variant. Sends a press
  immediately followed by a release, matching a real physical press. A
  no-op if the current screen has no handler for that action.
- **`read_view_model()`** - returns the most recently received UI state
  (screen type, title, instructions, footer, cursor, nav/selection
  flags), plus `display`: the rows of text the app's render function
  drew for it, as the device's display shows them. This is the last
  state the device pushed, not a live poll. `display` is null if the
  render function doesn't send its text (see below) or it hasn't arrived
  yet.
- **`status()`** - whether a session is open, its path and baud rate,
  the last error, and when a ViewModel was last received.

`press` needs an open session. If the board is unplugged mid-session,
the session ends and `press` reports why; `connect` again once it's back,
on whatever port it comes back on.

## Sending the display text

The view model says what a screen holds; the display text shows what the
app's render function made of it: text cut to fit, marks and arrows,
centering. To send it, the render function calls
`SixButtonUI::mcpSetCursor(col, row)` wherever it moves the display's
cursor, and `SixButtonUI::mcpPrint(c)` for each character it draws:

```cpp
// Draws text at a position on the LCD, and sends it.
void draw(uint8_t col, uint8_t row, const char* text) {
  lcd.setCursor(col, row);
  SixButtonUI::mcpSetCursor(col, row);
  for (; *text; text++) {
    lcd.print(*text);
    SixButtonUI::mcpPrint(*text);
  }
}
```

Without `SIXBUTTONUI_ENABLE_MCP` both functions do nothing. SixButtonUI
sends what they're given as one `6BUI=>'...'` line right after the
screen's view model line.
