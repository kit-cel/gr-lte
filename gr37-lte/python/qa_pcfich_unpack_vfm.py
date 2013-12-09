#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2013 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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

from gnuradio import gr, gr_unittest, blocks
import lte_swig as lte
import lte_test

class qa_pcfich_unpack_vfm (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

        key = "subframe"
        msg_buf_name = "cfi"
        print "setup test"

        data = []
        self.cfi_list = []
        for i in range(3):
            val = i + 1
            self.cfi_list.append(val)
            cfi = lte_test.get_cfi_sequence(val)
            data.extend(lte_test.nrz_encoding(cfi))

        reps = 5
        self.cfi_list *= reps

        srcid = "src"
        taglist = lte_test.get_tag_list(3*reps, 10, key, srcid)

        data = [float(data[i]) for i in range(len(data))]
        ext_data = []
        for i in range(reps):
            ext_data.extend(data)
        data = ext_data

        self.src = blocks.vector_source_f(data, False, 32, taglist)
        self.cfi = lte.pcfich_unpack_vfm(key, msg_buf_name)
        self.cfi.activate_debug_mode(True)

        self.tb.connect(self.src, self.cfi)
        print "setup finished"

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        res = self.cfi.cfi_results()
        print res
        print self.cfi_list
        self.assertListEqual(self.cfi_list, list(res))
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_pcfich_unpack_vfm)
