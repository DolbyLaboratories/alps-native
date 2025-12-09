#!/bin/bash

IGNORED_DIRS=( ".git" ".cache" "build" "third-party/dlb_mp4demux")

check_copyright() {
    local file="$1"
    local ext

    basename="${file##*/}"

    if [[ "$basename" == *.* && "$basename" != .* ]]; then
        ext="${basename##*.}"
    else
        ext="$basename"
    fi

    local notice_file="copyright/notice.$ext"

    [[ -f "$notice_file" ]] || return 0

    local notice_content=$(cat "$notice_file")
    local notice_lines=$(echo "$notice_content" | wc -l)
    local file_head=$(head -n "$notice_lines" "$file")

    if ! diff -w <(echo "$file_head") <(echo "$notice_content"); then
        echo "Missing or incorrect copyright notice in $file"
        return 1
    fi
    return 0
}

is_ignored_dir() {
    local dir="$1"
    for ignored in "${IGNORED_DIRS[@]}"; do
        [[ "$dir" == *"$ignored"* ]] && return 0
    done
    return 1
}

while read -r file; do
    dir=$(dirname "$file")
    is_ignored_dir "$dir" && continue
    echo "$file"
    if ! check_copyright "$file"; then
        exit 1
    fi
done < <(find . -type f)

echo "OK"

