#!/bin/bash
FILE="$1"
OFILE="a4-include.hpp"

cat "$FILE" | \
  sed -e '/#include <iostream>/d' | \
  sed -e '/#include <set>/d' | \
  sed -e '/#include <vector>/d' | \
  sed -e '/^int main/,/^[}]/d' > "$OFILE"
