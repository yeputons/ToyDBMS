#!/bin/bash
./demo >.out || exit 1
diff .out answer.txt || exit 1
rm .out
echo OK