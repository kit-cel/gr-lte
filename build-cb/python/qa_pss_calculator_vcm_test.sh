#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/maier/gr-lte/python
export PATH=/home/maier/gr-lte/build-cb/python:$PATH
export LD_LIBRARY_PATH=/home/maier/gr-lte/build-cb/lib:$LD_LIBRARY_PATH
export PYTHONPATH=/home/maier/gr-lte/build-cb/swig:$PYTHONPATH
/usr/bin/python2 /home/maier/gr-lte/python/qa_pss_calculator_vcm.py 
