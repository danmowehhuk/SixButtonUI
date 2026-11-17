import { test } from "node:test";
import assert from "node:assert/strict";
import { chooseBoard } from "../dist/boardFinder.js";

const BLUETOOTH = { path: "/dev/tty.Bluetooth-Incoming-Port" };
const BRIDGE = {
  path: "/dev/tty.usbserial-110",
  manufacturer: "Silicon Labs",
  serialNumber: "AAA",
  vendorId: "10c4",
};
// The same bridge, listed again under its other driver's name.
const BRIDGE_ALIAS = { ...BRIDGE, path: "/dev/tty.SLAB_USBtoUART" };
const OTHER = {
  path: "/dev/tty.usbserial-A10",
  manufacturer: "FTDI",
  serialNumber: "BBB",
  vendorId: "0403",
};

test("uses the only USB serial port, skipping ports without a USB vendor", () => {
  assert.deepEqual(chooseBoard([BLUETOOTH, BRIDGE], {}), ["/dev/cu.usbserial-110"]);
});

test("treats entries with the same serial number as one board", () => {
  assert.deepEqual(chooseBoard([BRIDGE, BRIDGE_ALIAS], {}), [
    "/dev/cu.usbserial-110",
    "/dev/cu.SLAB_USBtoUART",
  ]);
});

test("fails with a list when several boards are plugged in", () => {
  assert.throws(
    () => chooseBoard([BRIDGE, BRIDGE_ALIAS, OTHER], {}),
    (err) =>
      /2 USB serial devices/.test(err.message) &&
      /AAA/.test(err.message) &&
      /BBB/.test(err.message) &&
      /SIXBUTTON_SERIAL_NUMBER/.test(err.message),
  );
});

test("fails when no USB serial port is plugged in", () => {
  assert.throws(() => chooseBoard([BLUETOOTH], {}), /No USB serial port/);
});

test("a configured serial number picks one of several boards", () => {
  assert.deepEqual(chooseBoard([BRIDGE, OTHER], { serialNumber: "BBB" }), [
    "/dev/cu.usbserial-A10",
  ]);
});

test("a configured serial number that isn't plugged in fails", () => {
  assert.throws(() => chooseBoard([BRIDGE], { serialNumber: "ZZZ" }), /ZZZ/);
});

test("a configured path is used as given, in its callout form", () => {
  assert.deepEqual(chooseBoard([], { path: "/dev/tty.usbserial-9" }), ["/dev/cu.usbserial-9"]);
  assert.deepEqual(chooseBoard([], { path: "/dev/ttys003" }), ["/dev/ttys003"]);
});
