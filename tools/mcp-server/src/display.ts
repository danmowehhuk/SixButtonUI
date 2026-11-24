// Parses the "6BUI=>'...'" display lines emitted by SixButtonUI's
// SIXBUTTONUI_ENABLE_MCP feature (see SixButtonUI's Mcp::printDisplayChar)
// into the display's rows of text, as the app's render function drew them.
//
// Wire format, sent right after the view model line for the same screen:
//   6BUI=>'@<row>,<col>;<text>@<row>,<col>;<text>...'
//
// Each <text> is the characters drawn from that position on. '@', '\''
// and '\\' are backslash-escaped, and characters outside printable ASCII
// are sent as "\x" plus two hex digits. Drawing before the first position
// starts at row 0, column 0 of a blank display, and columns moved past
// without drawing read as spaces.
//
// What a character outside printable ASCII looks like depends on the
// display's character set (an LCD's custom or ROM characters), which this
// server can't know, so it stays "\xHH" in the row. It still takes one
// column: the columns after it are where the render function drew them.

const LINE_PREFIX = "6BUI=>'";
const LINE_SUFFIX = "'";
const HEX_PAIR = /^[0-9A-Fa-f]{2}$/;

/**
 * Parses one "6BUI=>'...'" line into the display's rows, each as long as
 * the drawing on it reached. Returns null if the line doesn't match that
 * format, including one whose closing quote was lost.
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
  if (endsInEscape(body)) return null; // the last quote was escaped text

  // Each row's cells, one per column, so a "\xHH" cell keeps the columns
  // after it in place.
  const rows: string[][] = [];
  let row = 0;
  let col = 0;

  const draw = (cell: string) => {
    while (rows.length <= row) rows.push([]);
    const cells = rows[row];
    while (cells.length < col) cells.push(" ");
    cells[col] = cell;
    col += 1;
  };

  let i = 0;
  while (i < body.length) {
    const c = body[i];
    if (c === "\\" && body[i + 1] === "x" && HEX_PAIR.test(body.slice(i + 2, i + 4))) {
      draw(body.slice(i, i + 4));
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

/** True if str ends in an unpaired '\', escaping whatever came after it. */
function endsInEscape(str: string): boolean {
  let backslashes = 0;
  for (let i = str.length - 1; i >= 0 && str[i] === "\\"; i--) backslashes++;
  return backslashes % 2 === 1;
}
