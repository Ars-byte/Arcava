#!/bin/sh
set -e

version="${1:-v1.0.0}"
name="Arcava-$version"

cd "$(dirname "$0")/.."
make

rm -rf "$name" "$name.tar.gz"
mkdir -p "$name"
cp Arcava Makefile README.md LICENSE "$name/"
cp -r src previews "$name/"
rm -f "$name"/src/*.o
tar -czf "$name.tar.gz" "$name"
rm -rf "$name"

echo "Wrote $name.tar.gz"
