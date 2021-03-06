#!/bin/bash

set -eu
cd `dirname $0`/..

PBCOPY="xsel --clipboard --input"

cd src
cat <<EOL | xargs cat | $PBCOPY 
concater_def.txt
app/header.hpp
app/main.cpp
EOL
