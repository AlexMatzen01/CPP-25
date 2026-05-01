from pathlib import Path
import shutil
import sys

# File extensions to process
EXTENSIONS = {".cpp", ".h"}

# Backup extension
BACKUP_EXT = ".bak"


def remove_comments(code: str) -> str:
    result = []

    i = 0
    n = len(code)

    in_single_comment = False
    in_multi_comment = False
    in_string = False
    in_char = False
    in_raw_string = False

    raw_delimiter = ""

    while i < n:
        c = code[i]
        nxt = code[i + 1] if i + 1 < n else ""

        # -----------------------------
        # SINGLE-LINE COMMENT
        # -----------------------------
        if in_single_comment:
            if c == "\n":
                in_single_comment = False
                result.append(c)
            i += 1
            continue

        # -----------------------------
        # MULTI-LINE COMMENT
        # -----------------------------
        if in_multi_comment:
            if c == "*" and nxt == "/":
                in_multi_comment = False
                i += 2
            else:
                i += 1
            continue

        # -----------------------------
        # RAW STRING LITERAL
        # -----------------------------
        if in_raw_string:
            result.append(c)

            # Detect end of raw string
            closing = ")" + raw_delimiter + '"'

            if code.startswith(closing, i):
                for j in range(1, len(closing)):
                    result.append(code[i + j])
                i += len(closing)
                in_raw_string = False
            else:
                i += 1

            continue

        # -----------------------------
        # NORMAL STRING
        # -----------------------------
        if in_string:
            result.append(c)

            if c == "\\":
                # Preserve escaped characters
                if i + 1 < n:
                    result.append(code[i + 1])
                    i += 2
                else:
                    i += 1
            elif c == '"':
                in_string = False
                i += 1
            else:
                i += 1

            continue

        # -----------------------------
        # CHARACTER LITERAL
        # -----------------------------
        if in_char:
            result.append(c)

            if c == "\\":
                if i + 1 < n:
                    result.append(code[i + 1])
                    i += 2
                else:
                    i += 1
            elif c == "'":
                in_char = False
                i += 1
            else:
                i += 1

            continue

        # -----------------------------
        # START OF RAW STRING
        # Example: R"(text)"
        # Example: R"DELIM(text)DELIM"
        # -----------------------------
        if c == "R" and nxt == '"':
            start = i + 2
            paren_pos = code.find("(", start)

            if paren_pos != -1:
                raw_delimiter = code[start:paren_pos]
                in_raw_string = True

                result.append(c)
                result.append(nxt)

                for j in range(start, paren_pos + 1):
                    result.append(code[j])

                i = paren_pos + 1
                continue

        # -----------------------------
        # START OF STRING
        # -----------------------------
        if c == '"':
            in_string = True
            result.append(c)
            i += 1
            continue

        # -----------------------------
        # START OF CHARACTER LITERAL
        # -----------------------------
        if c == "'":
            in_char = True
            result.append(c)
            i += 1
            continue

        # -----------------------------
        # START OF SINGLE-LINE COMMENT
        # -----------------------------
        if c == "/" and nxt == "/":
            in_single_comment = True
            i += 2
            continue

        # -----------------------------
        # START OF MULTI-LINE COMMENT
        # -----------------------------
        if c == "/" and nxt == "*":
            in_multi_comment = True
            i += 2
            continue

        # Normal character
        result.append(c)
        i += 1

    return "".join(result)


def process_file(path: Path):
    try:
        original = path.read_text(encoding="utf-8")

        cleaned = remove_comments(original)

        if cleaned != original:
            backup_path = path.with_suffix(path.suffix + BACKUP_EXT)

            # Create backup only once
            if not backup_path.exists():
                shutil.copy2(path, backup_path)

            path.write_text(cleaned, encoding="utf-8")

            print(f"[MODIFIED] {path}")

        else:
            print(f"[UNCHANGED] {path}")

    except Exception as e:
        print(f"[ERROR] {path}: {e}")


def main():
    root = Path.cwd()

    print(f"Scanning: {root}")
    print()

    for path in root.rglob("*"):
        if not path.is_file():
            continue

        if path.suffix.lower() not in EXTENSIONS:
            continue

        # Skip backup files
        if path.name.endswith(BACKUP_EXT):
            continue

        process_file(path)

    print()
    print("Done.")


if __name__ == "__main__":
    main()