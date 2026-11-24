import { test } from "node:test";
import assert from "node:assert/strict";
import { parseDisplayLine } from "../dist/display.js";

test("places each piece of text at its position, whatever the order", () => {
  assert.deepEqual(parseDisplayLine("6BUI=>'@1,0;>Phaser@0,0;Effects'"), ["Effects", ">Phaser"]);
});

test("fills columns moved past with spaces", () => {
  assert.deepEqual(parseDisplayLine("6BUI=>'@0,0;<Step@0,15;>'"), ["<Step          >"]);
});

test("starts at row 0, column 0 before any position", () => {
  assert.deepEqual(parseDisplayLine("6BUI=>'Hi@1,2;there'"), ["Hi", "  there"]);
});

test("keeps a row that was moved to but not drawn on", () => {
  assert.deepEqual(parseDisplayLine("6BUI=>'@0,0;Saved@1,0;'"), ["Saved", ""]);
});

test("unescapes @, ' and \\", () => {
  assert.deepEqual(parseDisplayLine("6BUI=>'@0,0;a\\@b\\'c\\\\d'"), ["a@b'c\\d"]);
});

test("keeps a character outside printable ASCII as \\xHH in one column", () => {
  assert.deepEqual(parseDisplayLine("6BUI=>'@0,0;20\\xDFC@0,5;!'"), ["20\\xDFC !"]);
  assert.deepEqual(parseDisplayLine("6BUI=>'\\x00\\x01'"), ["\\x00\\x01"]);
});

test("returns null for a line whose closing quote was lost", () => {
  assert.equal(parseDisplayLine("6BUI=>'@0,0;Hi\\'"), null);
});

test("keeps what was drawn before a position with no ;", () => {
  assert.deepEqual(parseDisplayLine("6BUI=>'@0,0;Hi@1'"), ["Hi"]);
});

test("returns null for other lines", () => {
  assert.equal(parseDisplayLine("6BUI->'type=ROOT'"), null);
  assert.equal(parseDisplayLine("debug output"), null);
});
