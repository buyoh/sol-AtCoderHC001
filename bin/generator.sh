#!/bin/bash

set -eu
cd `dirname $0`/..

ruby src/generator/generator.rb
