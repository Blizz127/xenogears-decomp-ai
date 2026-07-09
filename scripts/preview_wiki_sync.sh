#!/usr/bin/env bash
# Preview which docs/wiki/*.md files would be copied to a GitHub Wiki checkout.
# Does NOT copy, commit, or push.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="$ROOT/docs/wiki"
TARGET="${1:-}"

if [[ -z "$TARGET" ]]; then
    echo "Usage: $0 <path-to-wiki-checkout>" >&2
    echo "Example: $0 ~/Projects/xenogears-decomp-ai.wiki" >&2
    exit 1
fi

# Resolve to absolute path and require a .wiki directory name.
TARGET="$(cd "$(dirname "$TARGET")" && pwd)/$(basename "$TARGET")"
case "$TARGET" in
    *.wiki) ;;
    *)
        echo "ERROR: target directory name must end with '.wiki' (got: $(basename "$TARGET"))" >&2
        exit 1
        ;;
esac

if [[ ! -d "$SRC" ]]; then
    echo "ERROR: source directory not found: $SRC" >&2
    exit 1
fi

shopt -s nullglob
files=("$SRC"/*.md)
if [[ ${#files[@]} -eq 0 ]]; then
    echo "ERROR: no .md files found in $SRC" >&2
    exit 1
fi

echo "Wiki sync preview (no files copied)"
echo "Source: $SRC"
echo "Target: $TARGET"
echo
echo "Files that would be copied:"
for f in "${files[@]}"; do
    printf '  %s -> %s/%s\n' "$f" "$TARGET" "$(basename "$f")"
done
echo
echo "To copy manually:"
echo "  rsync -av \"$SRC\"/*.md \"$TARGET/\""
