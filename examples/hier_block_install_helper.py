#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2014, 2015 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import glob
import subprocess


def main():
    for file in glob.glob("hier_blocks/*.grc"):
        proc = ["grcc", file]
        print(proc)
        try:
            subprocess.call(proc)
        except:
            pass

if __name__ == "__main__":
    main()
