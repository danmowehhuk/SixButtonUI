// Finds which other processes have a serial port open, using lsof. A
// flock (which serialport takes) only stops other flock users, so this is
// how connect notices a flasher or serial monitor on the port.

import { execFile } from "node:child_process";

/** "name (PID n)" for each other process holding path, or null if none. */
export function findPortHolder(path: string): Promise<string | null> {
  return new Promise((resolve) => {
    execFile("lsof", ["-F", "pc", path], (err, stdout) => {
      // lsof exits 1 when nothing holds the file. Without lsof there is
      // nothing to check, and opening the port is the only test left.
      if (err && !stdout) {
        resolve(null);
        return;
      }
      const holders: string[] = [];
      let pid = "";
      for (const line of stdout.split("\n")) {
        if (line.startsWith("p")) pid = line.slice(1);
        if (line.startsWith("c") && pid !== String(process.pid)) {
          holders.push(`${line.slice(1)} (PID ${pid})`);
        }
      }
      resolve(holders.length > 0 ? holders.join(", ") : null);
    });
  });
}
