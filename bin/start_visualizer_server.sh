#!/bin/bash

set -eu
cd `dirname $0`/..

cd src/visualizer_server
echo "http server on port=8999"
python3 -m http.server 8999 > /dev/null
