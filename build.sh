#!/bin/bash

set -e

if [ ! -z ${ALLOY_WITH_BAREMETAL+x} ]; then
	CFLAGS="${CFLAGS} -DALLOY_WITH_BAREMETAL=1"
	export CFLAGS
fi

function build_dir {
	echo "Entering ${PWD}/$1"
	original_dir="${PWD}"
	cd $1
	./build.sh
	cd "${original_dir}"
}

build_dir "fonts/source-code-pro"
build_dir "utils"
build_dir "lib"
build_dir "src"
