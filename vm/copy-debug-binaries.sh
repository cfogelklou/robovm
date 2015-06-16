#!/bin/bash

BASE=$(cd $(dirname $0); pwd -P)

mkdir -p "$BASE/binaries"
rsync -av  --include '*.a' --include '*/' --exclude '**' "$BASE/target/binaries/" "$BASE/binaries/"
