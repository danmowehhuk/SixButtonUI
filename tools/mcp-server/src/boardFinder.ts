// Picks the serial port of the board to drive. Any USB serial port (one
// with a USB vendor id) is a candidate; ports like Bluetooth or the debug
// console are not. Entries sharing a serial number are one board listed
// under several device nodes (e.g. by two drivers for the same bridge).

/** What serialport's SerialPort.list() reports for a port. */
export interface PortInfo {
  path: string;
  manufacturer?: string;
  serialNumber?: string;
  vendorId?: string;
  productId?: string;
}

/** Narrows the choice when more than one board is plugged in. */
export interface BoardPin {
  path?: string;
  serialNumber?: string;
}

/**
 * On macOS, opening a /dev/tty.* node waits for a carrier signal that a
 * UART bridge never raises; the matching /dev/cu.* node opens at once.
 */
export function calloutPath(path: string): string {
  return path.replace(/^\/dev\/tty\./, "/dev/cu.");
}

/**
 * Returns the device nodes of the one board to connect to, to try in
 * order. Throws, listing what's plugged in, unless exactly one board
 * matches.
 */
export function chooseBoard(ports: PortInfo[], pin: BoardPin): string[] {
  if (pin.path) return [calloutPath(pin.path)];

  const usbPorts = ports.filter((port) => port.vendorId);
  const candidates = pin.serialNumber
    ? usbPorts.filter((port) => port.serialNumber === pin.serialNumber)
    : usbPorts;

  const boards = new Map<string, PortInfo[]>();
  for (const port of candidates) {
    const key = port.serialNumber ?? port.path;
    boards.set(key, [...(boards.get(key) ?? []), port]);
  }

  if (boards.size === 0) {
    if (pin.serialNumber) {
      throw new Error(
        `No USB serial port has serial number ${pin.serialNumber}. ${describe(usbPorts)}`,
      );
    }
    throw new Error("No USB serial port found. Is the board plugged in?");
  }
  if (boards.size > 1) {
    throw new Error(
      `Found ${boards.size} USB serial devices: ${describe(candidates)} ` +
        "Pick one with connect's port argument, SIXBUTTON_SERIAL_NUMBER or SIXBUTTON_SERIAL_PATH.",
    );
  }
  const [board] = boards.values();
  return [...new Set(board.map((port) => calloutPath(port.path)))];
}

function describe(ports: PortInfo[]): string {
  if (ports.length === 0) return "No USB serial ports are plugged in.";
  const lines = ports.map(
    (port) =>
      `${calloutPath(port.path)} (serial number ${port.serialNumber ?? "none"}, ` +
      `${port.manufacturer ?? "unknown manufacturer"})`,
  );
  return `Plugged in: ${lines.join("; ")}.`;
}
