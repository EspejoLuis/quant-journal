#!/bin/bash
YEAR=$(date +%Y)
MONTH=$(date +%m)
DAY=$(date +%d)
DAY_NUM=$(date +%-d)
MONTH_NAME=$(date +%B)
DAY_NAME=$(date +%A)

case $DAY_NUM in
  1|21|31) SUFFIX="st" ;;
  2|22)    SUFFIX="nd" ;;
  3|23)    SUFFIX="rd" ;;
  *)       SUFFIX="th" ;;
esac

MONTH_FOLDER=$(date +%m-%b)
LOG_DIR="/Users/apple/github_repos/quant-journal/log/$YEAR/$MONTH_FOLDER"
FILE="$LOG_DIR/$MONTH-$DAY-$YEAR.md"

mkdir -p "$LOG_DIR"

if [ ! -f "$FILE" ]; then
  printf -- "---\ntitle: ☀️ %s %s %s\ntype: log\n---\n# ☀️ %s%s %s %s - %s\n\n" \
    "$DAY_NUM" "$MONTH_NAME" "$YEAR" \
    "$DAY_NUM" "$SUFFIX" "$MONTH_NAME" "$YEAR" "$DAY_NAME" > "$FILE"
fi

open -a "Visual Studio Code" "$FILE"
