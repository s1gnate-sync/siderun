#!/bin/sh
set -eux
cd $(dirname $(readlink -f $0))
../bin/siderun -c 'embed("../bin/subroot", "subroot.js" ,"library.js");'
