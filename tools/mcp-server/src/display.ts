// Parses the "6BUI=>'...'" display lines emitted by SixButtonUI's
// SIXBUTTONUI_ENABLE_MCP feature (see SixButtonUI's Mcp::displayChar) into
// the display's rows of text, as the app's render function drew them.
//
// Wire format, sent right after the view model line for the same screen:
//   6BUI=>'@<row>,<col>;<text>@<row>,<col>;<text>...'
//
// Each <text> is the characters drawn from that position on. '@', '\''
// and '\\' are backslash-escaped, and characters outside printable ASCII
// are sent as "\x" plus two hex digits. Drawing before the first position
// starts at row 0, column 0. Positions the render function moved past
// without drawing (the display was cleared) read as spaces.

const LINE_PREFIX = "6BUI=>'";
const LINE_SUFFIX = "'";

/**
 * Parses one "6BUI=>'...'" line into the display's rows, each as long as
 * the drawing on it reached. Returns null if the line doesn't match that
 * format at all.
 */
export function parseDisplayLine(rawLine: string): string[] | null {
  const line = rawLine.trim();
  if (
    !line.startsWith(LINE_PREFIX) ||
    !line.endsWith(LINE_SUFFIX) ||
    line.length < LINE_PREFIX.length + LINE_SUFFIX.length
  ) {
    return null;
  }

  const body = line.slice(LINE_PREFIX.length, line.length - LINE_SUFFIX.length);
  const rows: string[][] = [];
  let row = 0;
  let col = 0;

  const draw = (c: string) => {
    while (rows.length <= row) rows.push([]);
    const cells = rows[row];
    while (cells.length < col) cells.push(" ");
    cells[col] = c;
    col += 1;
  };

  let i = 0;
  while (i < body.length) {
    const c = body[i];
    if (c === "\\" && body[i + 1] === "x" && i + 3 < body.length) {
      draw(String.fromCharCode(parseInt(body.slice(i + 2, i + 4), 16)));
      i += 4;
    } else if (c === "\\" && i + 1 < body.length) {
      draw(body[i + 1]);
      i += 2;
    } else if (c === "@") {
      const end = body.indexOf(";", i);
      if (end === -1) break; // malformed position - keep what was drawn
      const [rowText, colText] = body.slice(i + 1, end).split(",");
      row = parseInt(rowText, 10) || 0;
      col = parseInt(colText, 10) || 0;
      while (rows.length <= row) rows.push([]);
      i = end + 1;
    } else {
      draw(c);
      i += 1;
    }
  }

  return rows.map((cells) => cells.join(""));
}
