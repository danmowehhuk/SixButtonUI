import { test } from "node:test";
import assert from "node:assert/strict";
import { spawn } from "node:child_process";
import { once } from "node:events";
import { fileURLToPath } from "node:url";
import { SixButtonConnection } from "../dist/serial.js";

function viewModelLine(interactive) {
  return (
    `6BUI->'type=SELECTOR;id=2;title=Items;instr=;interactive=${interactive};footer=;` +
    "cursor=NO_CURSOR;cursorPos=0;hasNext=0;hasPrev=0;isSelected=0;isSelectable=0;isCancelable=0'"
  );
}

async function openPty() {
  const relay = spawn("python3", [fileURLToPath(new URL("pty_relay.py", import.meta.url))], {
    stdio: ["pipe", "pipe", "inherit"],
  });
  const [chunk] = await once(relay.stdout, "data");
  return { path: chunk.toString().trim(), relay };
}

function newConnection(options = {}) {
  return new SixButtonConnection({
    baudRate: 9600,
    debugLogPath: "/dev/null",
    pin: {},
    firstViewModelTimeoutMs: 500,
    findHolder: async () => null,
    ...options,
  });
}

// Starts a session on path, with the "device" rendering its first screen
// shortly after the port opens, as a board does after its reset.
async function connectWithBoot(connection, relay, path, interactive = "boot") {
  const connecting = connection.connect(path);
  setTimeout(() => relay.stdin.write(viewModelLine(interactive) + "\n"), 100);
  return connecting;
}

test("connect waits for the board's first screen", async () => {
  const { path, relay } = await openPty();
  const connection = newConnection();
  const session = await connectWithBoot(connection, relay, path);
  assert.equal(session.path, path);
  assert.equal(session.viewModel.interactive, "boot");
  assert.equal(connection.getStatus().sessionOpen, true);
  await connection.disconnect();
  relay.kill();
});

test("picks up later screens while nothing else is running", async () => {
  const { path, relay } = await openPty();
  const connection = newConnection();
  await connectWithBoot(connection, relay, path);
  relay.stdin.write(viewModelLine("beta") + "\n");
  await new Promise((resolve) => setTimeout(resolve, 300));
  assert.equal(connection.getLastViewModel().viewModel?.interactive, "beta");
  await connection.disconnect();
  relay.kill();
});

test("connect fails and releases the port when no screen arrives", async () => {
  const { path, relay } = await openPty();
  const connection = newConnection();
  await assert.rejects(connection.connect(path), /no SixButtonUI screen/);
  assert.equal(connection.getStatus().sessionOpen, false);
  relay.kill();
});

test("connect fails while a session is already open", async () => {
  const { path, relay } = await openPty();
  const connection = newConnection();
  await connectWithBoot(connection, relay, path);
  await assert.rejects(connection.connect(path), /already open/);
  await connection.disconnect();
  relay.kill();
});

test("disconnect ends the session, and connect starts a fresh one", async () => {
  const { path, relay } = await openPty();
  const connection = newConnection();
  await connectWithBoot(connection, relay, path, "first");
  assert.equal(await connection.disconnect(), true);
  assert.equal(connection.getStatus().sessionOpen, false);
  assert.equal(await connection.disconnect(), false);
  const session = await connectWithBoot(connection, relay, path, "second");
  assert.equal(session.viewModel.interactive, "second");
  await connection.disconnect();
  relay.kill();
});

test("press outside a session says to connect", async () => {
  const connection = newConnection();
  await assert.rejects(connection.writeCode("UP"), /call connect/i);
});

test("connect names another process holding the port", async () => {
  const connection = newConnection({ findHolder: async () => "screen (PID 42)" });
  await assert.rejects(connection.connect("/dev/ttys999"), /screen \(PID 42\)/);
  assert.equal(connection.getStatus().sessionOpen, false);
});

test("connect finds the only USB serial port itself", async () => {
  const { path, relay } = await openPty();
  const connection = newConnection({
    listPorts: async () => [
      { path: "/dev/tty.Bluetooth-Incoming-Port" },
      { path, vendorId: "10c4", serialNumber: "AAA" },
    ],
  });
  const session = await connectWithBoot(connection, relay, undefined);
  assert.equal(session.path, path);
  await connection.disconnect();
  relay.kill();
});

test("the session ends when the board goes away", async () => {
  const { path, relay } = await openPty();
  const connection = newConnection();
  await connectWithBoot(connection, relay, path);
  // A pseudo-terminal can't be unplugged, so raise the close event
  // serialport raises when a USB device disappears.
  connection["port"].emit("close", Object.assign(new Error("disconnected"), { disconnected: true }));
  assert.equal(connection.getStatus().sessionOpen, false);
  await assert.rejects(connection.writeCode("UP"), /ended/);
  relay.kill();
});
