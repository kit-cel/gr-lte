# 
# Copyright 2012 Johannes Demel
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

from gnuradio import gr
import lte

class linear_OFDM_equalizer_hier_vcvc(gr.hier_block2):
    def __init__(self, N_rb_dl):
        """
        docstring
	    """
        gr.hier_block2.__init__(self, "linear_OFDM_equalizer_hier_vcvc",
				gr.io_signature(1,1, gr.sizeof_gr_complex*12*N_rb_dl),  # Input signature
				gr.io_signature(3,3, gr.sizeof_gr_complex*12*N_rb_dl)) # Output signature

        # Define blocks
        
        self.eq   = lte.linear_OFDM_equalizer_vcvc(N_rb_dl)
        #self.eq.set_cell_id(cell_id)
        self.sh1  = gr.skiphead(gr.sizeof_gr_complex*12*N_rb_dl,7)
        self.sh2  = gr.skiphead(gr.sizeof_gr_complex*12*N_rb_dl,7)
        self.sh3  = gr.skiphead(gr.sizeof_gr_complex*12*N_rb_dl,7)
        
        self.connect(self, self.eq)
        self.connect( (self.eq,0),self.sh1, (self,0) )
        self.connect( (self.eq,1),self.sh2, (self,1) )
        self.connect( (self.eq,2),self.sh3, (self,2) )
        
    def get_eq(self):
        return self.eq
        
        
        
        

