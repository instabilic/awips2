"""unit tests for sparse utility functions"""

<<<<<<< HEAD
import numpy as np
from numpy.testing import *
from scipy.sparse.sputils import *
=======
from __future__ import division, print_function, absolute_import

import numpy as np
from numpy.testing import (TestCase, run_module_suite, assert_equal,
                           assert_array_equal)
from scipy.sparse import sputils
>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b


class TestSparseUtils(TestCase):

    def test_upcast(self):
<<<<<<< HEAD
        assert_equal(upcast('intc'),np.intc)
        assert_equal(upcast('int32','float32'),np.float64)
        assert_equal(upcast('bool',complex,float),np.complex128)
        assert_equal(upcast('i','d'),np.float64)
=======
        assert_equal(sputils.upcast('intc'),np.intc)
        assert_equal(sputils.upcast('int32','float32'),np.float64)
        assert_equal(sputils.upcast('bool',complex,float),np.complex128)
        assert_equal(sputils.upcast('i','d'),np.float64)
>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b

    def test_getdtype(self):
        A = np.array([1],dtype='int8')

<<<<<<< HEAD
        assert_equal(getdtype(None,default=float),np.float)
        assert_equal(getdtype(None,a=A),np.int8)

    def test_isscalarlike(self):
        assert_equal(isscalarlike(3.0),True)
        assert_equal(isscalarlike(-4),True)
        assert_equal(isscalarlike(2.5),True)
        assert_equal(isscalarlike(1 + 3j),True)
        assert_equal(isscalarlike(np.array(3)),True)
        assert_equal(isscalarlike( "16" ), True)

        assert_equal(isscalarlike( np.array([3])), False)
        assert_equal(isscalarlike( [[3]] ), False)
        assert_equal(isscalarlike( (1,) ), False)
        assert_equal(isscalarlike( (1,2) ), False)

    def test_isintlike(self):
        assert_equal(isintlike(3.0),True)
        assert_equal(isintlike(-4),True)
        assert_equal(isintlike(np.array(3)),True)
        assert_equal(isintlike(np.array([3])), False)

        assert_equal(isintlike(2.5),False)
        assert_equal(isintlike(1 + 3j),False)
        assert_equal(isintlike( (1,) ), False)
        assert_equal(isintlike( (1,2) ), False)

    def test_isshape(self):
        assert_equal(isshape( (1,2) ),True)
        assert_equal(isshape( (5,2) ),True)

        assert_equal(isshape( (1.5,2) ),False)
        assert_equal(isshape( (2,2,2) ),False)
        assert_equal(isshape( ([2],2) ),False)

    def test_issequence(self):
        assert_equal(issequence( (1,) ),True)
        assert_equal(issequence( (1,2,3) ),True)
        assert_equal(issequence( [1] ),True)
        assert_equal(issequence( [1,2,3] ),True)
        assert_equal(issequence( np.array([1,2,3]) ),True)

        assert_equal(issequence( np.array([[1],[2],[3]]) ),False)
        assert_equal(issequence( 3 ),False)

    def test_isdense(self):
        assert_equal(isdense( np.array([1]) ),True)
        assert_equal(isdense( np.matrix([1]) ),True)
=======
        assert_equal(sputils.getdtype(None,default=float),float)
        assert_equal(sputils.getdtype(None,a=A),np.int8)

    def test_isscalarlike(self):
        assert_equal(sputils.isscalarlike(3.0),True)
        assert_equal(sputils.isscalarlike(-4),True)
        assert_equal(sputils.isscalarlike(2.5),True)
        assert_equal(sputils.isscalarlike(1 + 3j),True)
        assert_equal(sputils.isscalarlike(np.array(3)),True)
        assert_equal(sputils.isscalarlike("16"), True)

        assert_equal(sputils.isscalarlike(np.array([3])), False)
        assert_equal(sputils.isscalarlike([[3]]), False)
        assert_equal(sputils.isscalarlike((1,)), False)
        assert_equal(sputils.isscalarlike((1,2)), False)

    def test_isintlike(self):
        assert_equal(sputils.isintlike(3.0),True)
        assert_equal(sputils.isintlike(-4),True)
        assert_equal(sputils.isintlike(np.array(3)),True)
        assert_equal(sputils.isintlike(np.array([3])), False)

        assert_equal(sputils.isintlike(2.5),False)
        assert_equal(sputils.isintlike(1 + 3j),False)
        assert_equal(sputils.isintlike((1,)), False)
        assert_equal(sputils.isintlike((1,2)), False)

    def test_isshape(self):
        assert_equal(sputils.isshape((1,2)),True)
        assert_equal(sputils.isshape((5,2)),True)

        assert_equal(sputils.isshape((1.5,2)),False)
        assert_equal(sputils.isshape((2,2,2)),False)
        assert_equal(sputils.isshape(([2],2)),False)

    def test_issequence(self):
        assert_equal(sputils.issequence((1,)),True)
        assert_equal(sputils.issequence((1,2,3)),True)
        assert_equal(sputils.issequence([1]),True)
        assert_equal(sputils.issequence([1,2,3]),True)
        assert_equal(sputils.issequence(np.array([1,2,3])),True)

        assert_equal(sputils.issequence(np.array([[1],[2],[3]])),False)
        assert_equal(sputils.issequence(3),False)

    def test_isdense(self):
        assert_equal(sputils.isdense(np.array([1])),True)
        assert_equal(sputils.isdense(np.matrix([1])),True)

    def test_compat_unique(self):
        x = np.array([0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
                      2, 2, 2, 2, 2, 2, 2, 3, 3,3, 3, 3, 3, 3,
                      4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5,
                      6, 6, 6, 6,6, 6, 6, 7, 7, 7, 7, 7, 7, 7,
                      8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9,9],
                     dtype=np.int32)
        y, j1 = sputils._compat_unique_impl(x, return_index=True)
        j2 = np.array([0, 7, 14, 21, 28, 35, 42, 49, 56, 63])
        assert_array_equal(j1, j2)

>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b

if __name__ == "__main__":
    run_module_suite()
