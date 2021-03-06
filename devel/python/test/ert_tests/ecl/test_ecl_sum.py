# !/usr/bin/env python
#  Copyright (C) 2014  Statoil ASA, Norway.
#   
#  The file 'test_ecl_sum.py' is part of ERT - Ensemble based Reservoir Tool.
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
import datetime
import os.path
from ert.cwrap import CFILE
from ert.ecl import EclSum, EclSumKeyWordVector
from ert.test import ExtendedTestCase , TestAreaContext


class EclSumTest(ExtendedTestCase):
    def setUp(self):
        self.test_file = self.createTestPath("Statoil/ECLIPSE/Gurbat/ECLIPSE.SMSPEC")
        self.ecl_sum = EclSum(self.test_file)

    def test_time_range_year(self):
        real_range = self.ecl_sum.timeRange(interval="1y", extend_end = False)
        extended_range = self.ecl_sum.timeRange(interval="1y", extend_end = True)
        assert real_range[-1] < extended_range[-1]

    def test_time_range_day(self):
        real_range = self.ecl_sum.timeRange(interval = "1d", extend_end = False)
        extended_range = self.ecl_sum.timeRange(interval = "1d", extend_end = True)
        assert real_range[-1] == extended_range[-1]

    def test_time_range_month(self):
        real_range = self.ecl_sum.timeRange(interval = "1m", extend_end = False)
        extended_range = self.ecl_sum.timeRange(interval = "1m", extend_end = True)
        assert real_range[-1] < extended_range[-1]

    def test_dump_csv_line(self):
        ecl_sum_vector = EclSumKeyWordVector(self.ecl_sum)
        ecl_sum_vector.addKeywords("F*")

        with self.assertRaises(KeyError):
            ecl_sum_vector.addKeyword("MISSING")

        dtime = datetime.datetime( 2002 , 01 , 01 , 0 , 0 , 0 )
        with TestAreaContext("EclSum/csv_dump"):
            test_file_name = self.createTestPath("dump.csv")
            outputH = open(test_file_name , "w")
            self.ecl_sum.dumpCSVLine( dtime, ecl_sum_vector, outputH)
            assert os.path.isfile(test_file_name)
