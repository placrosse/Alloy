#!/bin/bash

set -e

if [ ! -z ${ALLOY_WITH_BAREMETAL+x} ]; then
	CFLAGS="${CFLAGS} -DALLOY_WITH_BAREMETAL=1"
	export CFLAGS
fi

cd fonts/source-code-pro
./build.sh
cd ../..

cd utils
./build.sh
cd ..

cd lib
./build.sh
cd ..

cd src
./build.sh
cd ..
