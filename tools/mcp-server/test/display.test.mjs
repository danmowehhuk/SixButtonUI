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

test("unescapes @, ' and \\, and decodes \\xHH", () => {
  assert.deepEqual(parseDisplayLine("6BUI=>'@0,0;a\\@b\\'c\\\\d\\xDF'"), ["a@b'c\\dß"]);
});

test("returns null for other lines", () => {
  assert.equal(parseDisplayLine("6BUI->'type=ROOT'"), null);
  assert.equal(parseDisplayLine("debug output"), null);
});
