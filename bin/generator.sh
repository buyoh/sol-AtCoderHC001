#!/bin/bash

set -eu
cd `dirname $0`/..

mkdir -p out
cd out
../third_party/tools/target/release/gen <<EOL
$RANDOM
EOL
cat in/0000.txt
cd - > /dev/null
