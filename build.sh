#!/bin/bash

set -u
set -e

cd fonts/source-code-pro
./build.sh
cd ../..

cd utils
./build.sh
cd ..

cd src
./build.sh
cd ..
