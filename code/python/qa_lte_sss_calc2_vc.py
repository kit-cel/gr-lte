#!/usr/bin/env python
# 
# Copyright 2012 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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
#

from gnuradio import gr, gr_unittest
import lte_swig

class qa_sss_calc2_vc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        ############################
        # define variables for test purposes
        self.fftl = fftl = 2048
        N_rb_dl = 6
        self.N_id = N_id = 124
        N_id_1 = N_id/3
        N_id_2 = N_id%3
        
        # This blocks are needed as "dummy" references
        self.tag = lte_swig.sss_tagging2_vcvc(fftl)
        self.eq = lte_swig.linear_OFDM_estimator_vcvc(N_rb_dl)
        self.demux = lte_swig.pbch_demux_vcvc(N_rb_dl)
        self.descr = lte_swig.descrambling_vfvf()
        self.daemon = lte_swig.cell_id_daemon(self.eq, self.demux, self.descr)
        
        #Source
        data = range(2*fftl)
        self.src = gr.vector_source_c(data, False, fftl)        
        # UUT
        self.calc = lte_swig.sss_calc2_vc(self.tag, self.daemon, fftl)
        
        self.tb.connect(self.src, self.calc)


    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        #####################################
        # Following part generates test data
        fftl = self.fftl
        N_id = self.N_id
        N_id_1 = N_id/3
        N_id_2 = N_id%3
        
        q_prime = N_id_1/30
        q = ( N_id_1 + (q_prime*(q_prime+1)/2) )/30
        m_prime = N_id_1 + q*(q+1)/2
        m0 = m_prime%31
        m1 = (m0 + (m_prime/31) +1)%31
        #print "m0 = " + str(m0)
        #print "m1 = " + str(m1)

        # Generate sX (or s_tilda)
        x_s = range(31)
        sX = range(31)
        # initial conditions:
        x_s[0] = x_s[1] = x_s[2] = x_s[3] = 0
        x_s[4] = 1
        for i in range(len(x_s)-5):
            x_s[i+5] = (x_s[i+2]+x_s[i])%2           
        for i in sX:
            sX[i] = 1-2*x_s[i]
        
        # Generate cX
        x_c = range(31)
        cX = range(31)
        x_c[0] = x_c[1] = x_c[2] = x_c[3] = 0
        x_c[4] = 1
        for i in range(len(x_c)-5):
            x_c[i+5] = (x_c[i+3]+x_c[i])%2
        for i in cX:
            cX[i] = 1-2*x_c[i]
                   
        # Generate zX
        x_z = range(31)
        zX = range(31)
        x_z[0] = x_z[1] = x_z[2] = x_z[3] = 0
        x_z[4] = 1
        for i in range(len(x_z)-5):
            x_z[i+5] = (x_z[i+4]+x_z[i+2]+x_z[i+1]+x_z[i])%2
        for i in zX:
            zX[i] = 1-2*x_z[i]
        
        # Generate s0 and s1
        s0 = range(31)
        s1 = range(31)
        for n in s0:
            s0[n] = sX[(n+m0)%31]
            s1[n] = sX[(n+m1)%31]

        # Generate c0 and c1
        c0 = range(31)
        c1 = range(31)
        for n in c0:
            c0[n] = cX[(n+N_id_2  )%31]
            c1[n] = cX[(n+N_id_2+3)%31]

        # Generate z0 and z1
        z1m0 = range(31)
        z1m1 = range(31)
        for n in z1m0:
            z1m0[n] = zX[(n+(m0%8))%31]
            z1m1[n] = zX[(n+(m1%8))%31]
        
        # Generate SSS_0 and SSS_5
        sss_0 = range(62)
        sss_5 = range(62)
        for n in range(31):
            sss_0[2*n  ] = s0[n]*c0[n]
            sss_0[2*n+1] = s1[n]*c1[n]*z1m0[n]
            sss_5[2*n  ] = s1[n]*c0[n]
            sss_5[2*n+1] = s0[n]*c1[n]*z1m1[n]
        
        vec0 = range(fftl)
        vec1 = range(fftl)
        for i in vec0:
            vec0[i] = 0
            vec1[i] = 0
        vec0[6:6+31] = sss_0[0:31]
        vec0[fftl-5-31:fftl-5] = sss_0[31:62]
        vec1[6:6+31] = sss_5[0:31]
        vec1[fftl-5-31:fftl-5] = sss_5[31:62]
        data = vec0 + vec1
        data = data + data + data + data + data
        
        self.src.set_data(data)
        
        # The following data generation part must be reimplemented with one of the next GR versions
        # Those will have vector sinks and sources with tag support (great for qa code)

        # set up fg
        self.tb.run ()
        # check data
        cell_id = self.calc.get_cell_id()
        frame_start = self.calc.get_frame_start()
        print "Cell_ID\tdetected = %d\tgiven = %d" % (cell_id, self.N_id)
        print "frame_start = %d" % (frame_start)
        assert(cell_id == self.N_id)
        
        
    def test_002_m_data (self):
        print "test 2"
        self.tb.run()


if __name__ == '__main__':
    gr_unittest.main ()
