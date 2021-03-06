#!/bin/bash

set -eu
cd `dirname $0`/..

g++ --std=c++17 -O3 -o out/main src/app/main.cpp
