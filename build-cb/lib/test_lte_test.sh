#!/bin/sh
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/maier/gr-lte/lib
export PATH=/home/maier/gr-lte/build-cb/lib:$PATH
export LD_LIBRARY_PATH=/home/maier/gr-lte/build-cb/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
test-lte 
