#!/bin/bash

set -e

source_code_pro_url="https://github.com/adobe-fonts/source-code-pro/archive/2.030R-ro/1.050R-it.tar.gz"
source_code_pro_tar="source-code-pro-2.030R-ro-1.050R-it.tar.gz"
source_code_pro_dir="source-code-pro-2.030R-ro-1.050R-it"

if [ ! -e "$source_code_pro_tar" ]; then
	wget --no-verbose "$source_code_pro_url" -O "$source_code_pro_tar"
fi

if [ ! -e "$source_code_pro_dir" ]; then
	tar --extract --file "$source_code_pro_tar"
fi

