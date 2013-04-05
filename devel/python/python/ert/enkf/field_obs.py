#  Copyright (C) 2012  Statoil ASA, Norway. 
#   
#  The file 'field_obs.py' is part of ERT - Ensemble based Reservoir Tool. 
#   
#  ERT is free software: you can redistribute it and/or modify 
#  it under the terms of the GNU General Public License as published by 
#  the Free Software Foundation, either version 3 of the License, or 
#  (at your option) any later version. 
#   
#  ERT is distributed in the hope that it will be useful, but WITHOUT ANY 
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or 
#  FITNESS FOR A PARTICULAR PURPOSE.   
#   
#  See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html> 
#  for more details. 

import  ctypes
from    ert.cwrap.cwrap       import *
from    ert.cwrap.cclass      import CClass
from    ert.util.tvector      import * 
from    enkf_enum             import *
import  libenkf
class FieldObs(CClass):
    
    def __init__(self , c_ptr = None):
        self.owner = False
        self.c_ptr = c_ptr
        
        
    def __del__(self):
        if self.owner:
            cfunc.free( self )


    def has_key(self , key):
        return cfunc.has_key( self ,key )



##################################################################

cwrapper = CWrapper( libenkf.lib )
cwrapper.registerType( "field_obs" , FieldObs )

# 3. Installing the c-functions used to manipulate ecl_kw instances.
#    These functions are used when implementing the EclKW class, not
#    used outside this scope.
cfunc = CWrapperNameSpace("field_obs")


cfunc.free                = cwrapper.prototype("void field_obs_free( field_obs )")
cfunc.get_i               = cwrapper.prototype("int* field_obs_get_i(field_obs)")
cfunc.get_j               = cwrapper.prototype("int* field_obs_get_j( field_obs )")
cfunc.get_k               = cwrapper.prototype("int* field_obs_get_k( field_obs )")
cfunc.get_size            = cwrapper.prototype("int field_obs_get_size( field_obs )")
cfunc.iget                = cwrapper.prototype("void field_obs_iget( field_obs, int, double*, double*)")