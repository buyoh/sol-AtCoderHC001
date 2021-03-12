#!/bin/bash

set -eu
cd `dirname $0`/..

cd out
./main_prof
# gprof ./main_prof ./gmon.out > ./profile.txt
# gprof -A ./main_prof ./gmon.out > ./profile-a.txt
# gprof -Al ./main_prof ./gmon.out > ./profile-al.txt  # line-by-line
gcov ./main.gcda > ./coverage-a.txt
