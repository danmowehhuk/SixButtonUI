import { test } from "node:test";
import assert from "node:assert/strict";
import { spawn } from "node:child_process";
import { once } from "node:events";
import { fileURLToPath } from "node:url";
import { SixButtonConnection } from "../dist/serial.js";

const VIEW_MODEL_LINE =
  "6BUI->'type=SELECTOR;id=2;title=Items;instr=;interactive=beta;footer=;" +
  "cursor=NO_CURSOR;cursorPos=0;hasNext=0;hasPrev=0;isSelected=0;isSelectable=0;isCancelable=0'";

async function openPty() {
  const relay = spawn("python3", [fileURLToPath(new URL("pty_relay.py", import.meta.url))], {
    stdio: ["pipe", "pipe", "inherit"],
  });
  const [chunk] = await once(relay.stdout, "data");
  return { path: chunk.toString().trim(), relay };
}

test("picks up an incoming ViewModel line while nothing else is running", async () => {
  const { path, relay } = await openPty();
  const connection = new SixButtonConnection(path, 9600, "/dev/null");
  await connection.connect();

  relay.stdin.write(VIEW_MODEL_LINE + "\n");
  await new Promise((resolve) => setTimeout(resolve, 300));

  assert.equal(connection.getLastViewModel().viewModel?.interactive, "beta");
  relay.kill();
});
