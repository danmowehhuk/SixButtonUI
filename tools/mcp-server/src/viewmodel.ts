// Parses the "6BUI->'...'" wire lines emitted by SixButtonUI's
// SIXBUTTONUI_ENABLE_MCP feature (see SixButtonUI's Mcp::serialize) into
// a structured ViewModel object.
//
// Wire format:
//   6BUI->'type=<name>;id=<n|none>;title=<text>;instr=<text>;
//          interactive=<text>;footer=<text>;cursor=<name>;
//          cursorPos=<n>;hasNext=<0|1>;hasPrev=<0|1>;isSelected=<0|1>;
//          isSelectable=<0|1>;isCancelable=<0|1>'
//
// The firmware backslash-escapes ';', '=', '\'', and '\\' inside the
// four free-text fields (title/instr/interactive/footer). This parser
// applies the same unescape uniformly to every field's value - a no-op
// for fields that never contain those characters (type, id, cursor,
// the numeric/boolean fields), so no per-field special-casing is
// needed here.

export interface ViewModel {
  type: string;
  id: number | null;
  title: string;
  instr: string;
  interactive: string;
  footer: string;
  cursor: string;
  cursorPos: number;
  hasNext: boolean;
  hasPrev: boolean;
  isSelected: boolean;
  isSelectable: boolean;
  isCancelable: boolean;
}

const LINE_PREFIX = "6BUI->'";
const LINE_SUFFIX = "'";

/**
 * Splits `str` on unescaped occurrences of `delimiter` (a single
 * character). A '\' immediately before any character escapes it - that
 * character is never treated as a delimiter, and the backslash is left
 * in place for `unescape()` to strip later.
 */
function splitUnescaped(str: string, delimiter: string): string[] {
  const parts: string[] = [];
  let current = "";
  let i = 0;
  while (i < str.length) {
    const c = str[i];
    if (c === "\\" && i + 1 < str.length) {
      current += c + str[i + 1];
      i += 2;
      continue;
    }
    if (c === delimiter) {
      parts.push(current);
      current = "";
      i += 1;
      continue;
    }
    current += c;
    i += 1;
  }
  parts.push(current);
  return parts;
}

/** Index of the first unescaped '=' in `str`, or -1 if there is none. */
function findUnescapedEquals(str: string): number {
  for (let i = 0; i < str.length; i++) {
    if (str[i] === "\\") {
      i++;
      continue;
    }
    if (str[i] === "=") return i;
  }
  return -1;
}

/** Reverses the firmware's backslash-escaping of ';', '=', '\'', '\\'. */
function unescape(str: string): string {
  let result = "";
  let i = 0;
  while (i < str.length) {
    if (str[i] === "\\" && i + 1 < str.length) {
      result += str[i + 1];
      i += 2;
      continue;
    }
    result += str[i];
    i += 1;
  }
  return result;
}

/**
 * Parses one "6BUI->'...'" line into a ViewModel. Returns null if the
 * line doesn't match that format at all (e.g. it's other debug output
 * the firmware printed on the same wire) - callers should route those
 * lines to the debug log instead.
 */
export function parseViewModelLine(rawLine: string): ViewModel | null {
  const line = rawLine.trim();
  if (
    !line.startsWith(LINE_PREFIX) ||
    !line.endsWith(LINE_SUFFIX) ||
    line.length < LINE_PREFIX.length + LINE_SUFFIX.length
  ) {
    return null;
  }

  const body = line.slice(LINE_PREFIX.length, line.length - LINE_SUFFIX.length);

  const fields: Record<string, string> = {};
  for (const part of splitUnescaped(body, ";")) {
    const eqIndex = findUnescapedEquals(part);
    if (eqIndex === -1) continue; // malformed field - skip rather than throw
    const key = part.slice(0, eqIndex);
    const value = unescape(part.slice(eqIndex + 1));
    fields[key] = value;
  }

  const idRaw = fields["id"];

  return {
    type: fields["type"] ?? "",
    id: idRaw === undefined || idRaw === "none" ? null : parseInt(idRaw, 10),
    title: fields["title"] ?? "",
    instr: fields["instr"] ?? "",
    interactive: fields["interactive"] ?? "",
    footer: fields["footer"] ?? "",
    cursor: fields["cursor"] ?? "",
    cursorPos: parseInt(fields["cursorPos"] ?? "0", 10),
    hasNext: fields["hasNext"] === "1",
    hasPrev: fields["hasPrev"] === "1",
    isSelected: fields["isSelected"] === "1",
    isSelectable: fields["isSelectable"] === "1",
    isCancelable: fields["isCancelable"] === "1",
  };
}
