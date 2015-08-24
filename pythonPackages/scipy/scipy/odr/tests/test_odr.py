<<<<<<< HEAD
# Scipy imports.
import numpy as np
from numpy import pi
from numpy.testing import *
=======
from __future__ import division, print_function, absolute_import

# Scipy imports.
import numpy as np
from numpy import pi
from numpy.testing import (assert_array_almost_equal, TestCase,
                           run_module_suite, assert_equal)
>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b
from scipy.odr import Data, Model, ODR, RealData, odr_stop


class TestODR(TestCase):

    # Explicit Example

    def explicit_fcn(self, B, x):
        ret = B[0] + B[1] * np.power(np.exp(B[2]*x) - 1.0, 2)
        return ret

    def explicit_fjd(self, B, x):
        eBx = np.exp(B[2]*x)
        ret = B[1] * 2.0 * (eBx-1.0) * B[2] * eBx
        return ret

    def explicit_fjb(self, B, x):
        eBx = np.exp(B[2]*x)
        res = np.vstack([np.ones(x.shape[-1]),
                         np.power(eBx-1.0, 2),
                         B[1]*2.0*(eBx-1.0)*eBx*x])
        return res

    def test_explicit(self):
        explicit_mod = Model(
            self.explicit_fcn,
            fjacb=self.explicit_fjb,
            fjacd=self.explicit_fjd,
            meta=dict(name='Sample Explicit Model',
                      ref='ODRPACK UG, pg. 39'),
        )
        explicit_dat = Data([0.,0.,5.,7.,7.5,10.,16.,26.,30.,34.,34.5,100.],
                        [1265.,1263.6,1258.,1254.,1253.,1249.8,1237.,1218.,1220.6,
                         1213.8,1215.5,1212.])
        explicit_odr = ODR(explicit_dat, explicit_mod, beta0=[1500.0, -50.0, -0.1],
                       ifixx=[0,0,1,1,1,1,1,1,1,1,1,0])
        explicit_odr.set_job(deriv=2)
<<<<<<< HEAD
=======
        explicit_odr.set_iprint(init=0, iter=0, final=0)
>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b

        out = explicit_odr.run()
        assert_array_almost_equal(
            out.beta,
<<<<<<< HEAD
            np.array([  1.2646548050648876e+03,  -5.4018409956678255e+01,
=======
            np.array([1.2646548050648876e+03, -5.4018409956678255e+01,
>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b
                -8.7849712165253724e-02]),
        )
        assert_array_almost_equal(
            out.sd_beta,
<<<<<<< HEAD
            np.array([ 1.0349270280543437,  1.583997785262061 ,  0.0063321988657267]),
        )
        assert_array_almost_equal(
            out.cov_beta,
            np.array([[  4.4949592379003039e-01,  -3.7421976890364739e-01,
                 -8.0978217468468912e-04],
               [ -3.7421976890364739e-01,   1.0529686462751804e+00,
                 -1.9453521827942002e-03],
               [ -8.0978217468468912e-04,  -1.9453521827942002e-03,
                  1.6827336938454476e-05]]),
        )


=======
            np.array([1.0349270280543437, 1.583997785262061, 0.0063321988657267]),
        )
        assert_array_almost_equal(
            out.cov_beta,
            np.array([[4.4949592379003039e-01, -3.7421976890364739e-01,
                 -8.0978217468468912e-04],
               [-3.7421976890364739e-01, 1.0529686462751804e+00,
                 -1.9453521827942002e-03],
               [-8.0978217468468912e-04, -1.9453521827942002e-03,
                  1.6827336938454476e-05]]),
        )

>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b
    # Implicit Example

    def implicit_fcn(self, B, x):
        return (B[2]*np.power(x[0]-B[0], 2) +
                2.0*B[3]*(x[0]-B[0])*(x[1]-B[1]) +
                B[4]*np.power(x[1]-B[1], 2) - 1.0)

    def test_implicit(self):
        implicit_mod = Model(
            self.implicit_fcn,
            implicit=1,
            meta=dict(name='Sample Implicit Model',
                      ref='ODRPACK UG, pg. 49'),
        )
        implicit_dat = Data([
            [0.5,1.2,1.6,1.86,2.12,2.36,2.44,2.36,2.06,1.74,1.34,0.9,-0.28,
             -0.78,-1.36,-1.9,-2.5,-2.88,-3.18,-3.44],
            [-0.12,-0.6,-1.,-1.4,-2.54,-3.36,-4.,-4.75,-5.25,-5.64,-5.97,-6.32,
             -6.44,-6.44,-6.41,-6.25,-5.88,-5.5,-5.24,-4.86]],
            1,
        )
        implicit_odr = ODR(implicit_dat, implicit_mod,
            beta0=[-1.0, -3.0, 0.09, 0.02, 0.08])

        out = implicit_odr.run()
        assert_array_almost_equal(
            out.beta,
<<<<<<< HEAD
            np.array([-0.9993809167281279, -2.9310484652026476,  0.0875730502693354,
                0.0162299708984738,  0.0797537982976416]),
        )
        assert_array_almost_equal(
            out.sd_beta,
            np.array([ 0.1113840353364371,  0.1097673310686467,  0.0041060738314314,
                0.0027500347539902,  0.0034962501532468]),
        )
        assert_array_almost_equal(
            out.cov_beta,
            np.array([[  2.1089274602333052e+00,  -1.9437686411979040e+00,
                  7.0263550868344446e-02,  -4.7175267373474862e-02,
                  5.2515575927380355e-02],
               [ -1.9437686411979040e+00,   2.0481509222414456e+00,
                 -6.1600515853057307e-02,   4.6268827806232933e-02,
                 -5.8822307501391467e-02],
               [  7.0263550868344446e-02,  -6.1600515853057307e-02,
                  2.8659542561579308e-03,  -1.4628662260014491e-03,
                  1.4528860663055824e-03],
               [ -4.7175267373474862e-02,   4.6268827806232933e-02,
                 -1.4628662260014491e-03,   1.2855592885514335e-03,
                 -1.2692942951415293e-03],
               [  5.2515575927380355e-02,  -5.8822307501391467e-02,
                  1.4528860663055824e-03,  -1.2692942951415293e-03,
                  2.0778813389755596e-03]]),
        )


=======
            np.array([-0.9993809167281279, -2.9310484652026476, 0.0875730502693354,
                0.0162299708984738, 0.0797537982976416]),
        )
        assert_array_almost_equal(
            out.sd_beta,
            np.array([0.1113840353364371, 0.1097673310686467, 0.0041060738314314,
                0.0027500347539902, 0.0034962501532468]),
        )
        assert_array_almost_equal(
            out.cov_beta,
            np.array([[2.1089274602333052e+00, -1.9437686411979040e+00,
                  7.0263550868344446e-02, -4.7175267373474862e-02,
                  5.2515575927380355e-02],
               [-1.9437686411979040e+00, 2.0481509222414456e+00,
                 -6.1600515853057307e-02, 4.6268827806232933e-02,
                 -5.8822307501391467e-02],
               [7.0263550868344446e-02, -6.1600515853057307e-02,
                  2.8659542561579308e-03, -1.4628662260014491e-03,
                  1.4528860663055824e-03],
               [-4.7175267373474862e-02, 4.6268827806232933e-02,
                 -1.4628662260014491e-03, 1.2855592885514335e-03,
                 -1.2692942951415293e-03],
               [5.2515575927380355e-02, -5.8822307501391467e-02,
                  1.4528860663055824e-03, -1.2692942951415293e-03,
                  2.0778813389755596e-03]]),
        )

>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b
    # Multi-variable Example

    def multi_fcn(self, B, x):
        if (x < 0.0).any():
            raise odr_stop
        theta = pi*B[3]/2.
        ctheta = np.cos(theta)
        stheta = np.sin(theta)
        omega = np.power(2.*pi*x*np.exp(-B[2]), B[3])
        phi = np.arctan2((omega*stheta), (1.0 + omega*ctheta))
        r = (B[0] - B[1]) * np.power(np.sqrt(np.power(1.0 + omega*ctheta, 2) +
             np.power(omega*stheta, 2)), -B[4])
        ret = np.vstack([B[1] + r*np.cos(B[4]*phi),
                         r*np.sin(B[4]*phi)])
        return ret

    def test_multi(self):
        multi_mod = Model(
            self.multi_fcn,
            meta=dict(name='Sample Multi-Response Model',
                      ref='ODRPACK UG, pg. 56'),
        )

        multi_x = np.array([30.0, 50.0, 70.0, 100.0, 150.0, 200.0, 300.0, 500.0,
            700.0, 1000.0, 1500.0, 2000.0, 3000.0, 5000.0, 7000.0, 10000.0,
            15000.0, 20000.0, 30000.0, 50000.0, 70000.0, 100000.0, 150000.0])
        multi_y = np.array([
            [4.22, 4.167, 4.132, 4.038, 4.019, 3.956, 3.884, 3.784, 3.713,
             3.633, 3.54, 3.433, 3.358, 3.258, 3.193, 3.128, 3.059, 2.984,
             2.934, 2.876, 2.838, 2.798, 2.759],
            [0.136, 0.167, 0.188, 0.212, 0.236, 0.257, 0.276, 0.297, 0.309,
             0.311, 0.314, 0.311, 0.305, 0.289, 0.277, 0.255, 0.24, 0.218,
             0.202, 0.182, 0.168, 0.153, 0.139],
        ])
        n = len(multi_x)
        multi_we = np.zeros((2, 2, n), dtype=float)
        multi_ifixx = np.ones(n, dtype=int)
        multi_delta = np.zeros(n, dtype=float)

        multi_we[0,0,:] = 559.6
        multi_we[1,0,:] = multi_we[0,1,:] = -1634.0
        multi_we[1,1,:] = 8397.0

        for i in range(n):
            if multi_x[i] < 100.0:
                multi_ifixx[i] = 0
            elif multi_x[i] <= 150.0:
<<<<<<< HEAD
                pass # defaults are fine
=======
                pass  # defaults are fine
>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b
            elif multi_x[i] <= 1000.0:
                multi_delta[i] = 25.0
            elif multi_x[i] <= 10000.0:
                multi_delta[i] = 560.0
            elif multi_x[i] <= 100000.0:
                multi_delta[i] = 9500.0
            else:
                multi_delta[i] = 144000.0
            if multi_x[i] == 100.0 or multi_x[i] == 150.0:
                multi_we[:,:,i] = 0.0

        multi_dat = Data(multi_x, multi_y, wd=1e-4/np.power(multi_x, 2),
            we=multi_we)
        multi_odr = ODR(multi_dat, multi_mod, beta0=[4.,2.,7.,.4,.5],
            delta0=multi_delta, ifixx=multi_ifixx)
        multi_odr.set_job(deriv=1, del_init=1)

        out = multi_odr.run()
        assert_array_almost_equal(
            out.beta,
<<<<<<< HEAD
            np.array([ 4.3799880305938963,  2.4333057577497703,  8.0028845899503978,
                0.5101147161764654,  0.5173902330489161]),
        )
        assert_array_almost_equal(
            out.sd_beta,
            np.array([ 0.0130625231081944,  0.0130499785273277,  0.1167085962217757,
                0.0132642749596149,  0.0288529201353984]),
        )
        assert_array_almost_equal(
            out.cov_beta,
            np.array([[ 0.0064918418231375,  0.0036159705923791,  0.0438637051470406,
                -0.0058700836512467,  0.011281212888768 ],
               [ 0.0036159705923791,  0.0064793789429006,  0.0517610978353126,
                -0.0051181304940204,  0.0130726943624117],
               [ 0.0438637051470406,  0.0517610978353126,  0.5182263323095322,
                -0.0563083340093696,  0.1269490939468611],
               [-0.0058700836512467, -0.0051181304940204, -0.0563083340093696,
                 0.0066939246261263, -0.0140184391377962],
               [ 0.011281212888768 ,  0.0130726943624117,  0.1269490939468611,
                -0.0140184391377962,  0.0316733013820852]]),
        )


=======
            np.array([4.3799880305938963, 2.4333057577497703, 8.0028845899503978,
                0.5101147161764654, 0.5173902330489161]),
        )
        assert_array_almost_equal(
            out.sd_beta,
            np.array([0.0130625231081944, 0.0130499785273277, 0.1167085962217757,
                0.0132642749596149, 0.0288529201353984]),
        )
        assert_array_almost_equal(
            out.cov_beta,
            np.array([[0.0064918418231375, 0.0036159705923791, 0.0438637051470406,
                -0.0058700836512467, 0.011281212888768],
               [0.0036159705923791, 0.0064793789429006, 0.0517610978353126,
                -0.0051181304940204, 0.0130726943624117],
               [0.0438637051470406, 0.0517610978353126, 0.5182263323095322,
                -0.0563083340093696, 0.1269490939468611],
               [-0.0058700836512467, -0.0051181304940204, -0.0563083340093696,
                 0.0066939246261263, -0.0140184391377962],
               [0.011281212888768, 0.0130726943624117, 0.1269490939468611,
                -0.0140184391377962, 0.0316733013820852]]),
        )

>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b
    # Pearson's Data
    # K. Pearson, Philosophical Magazine, 2, 559 (1901)

    def pearson_fcn(self, B, x):
        return B[0] + B[1]*x

    def test_pearson(self):
        p_x = np.array([0.,.9,1.8,2.6,3.3,4.4,5.2,6.1,6.5,7.4])
        p_y = np.array([5.9,5.4,4.4,4.6,3.5,3.7,2.8,2.8,2.4,1.5])
        p_sx = np.array([.03,.03,.04,.035,.07,.11,.13,.22,.74,1.])
        p_sy = np.array([1.,.74,.5,.35,.22,.22,.12,.12,.1,.04])

        p_dat = RealData(p_x, p_y, sx=p_sx, sy=p_sy)

        # Reverse the data to test invariance of results
        pr_dat = RealData(p_y, p_x, sx=p_sy, sy=p_sx)

        p_mod = Model(self.pearson_fcn, meta=dict(name='Uni-linear Fit'))

        p_odr = ODR(p_dat, p_mod, beta0=[1.,1.])
        pr_odr = ODR(pr_dat, p_mod, beta0=[1.,1.])

        out = p_odr.run()
        assert_array_almost_equal(
            out.beta,
<<<<<<< HEAD
            np.array([ 5.4767400299231674, -0.4796082367610305]),
        )
        assert_array_almost_equal(
            out.sd_beta,
            np.array([ 0.3590121690702467,  0.0706291186037444]),
        )
        assert_array_almost_equal(
            out.cov_beta,
            np.array([[ 0.0854275622946333, -0.0161807025443155],
               [-0.0161807025443155,  0.003306337993922 ]]),
=======
            np.array([5.4767400299231674, -0.4796082367610305]),
        )
        assert_array_almost_equal(
            out.sd_beta,
            np.array([0.3590121690702467, 0.0706291186037444]),
        )
        assert_array_almost_equal(
            out.cov_beta,
            np.array([[0.0854275622946333, -0.0161807025443155],
               [-0.0161807025443155, 0.003306337993922]]),
>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b
        )

        rout = pr_odr.run()
        assert_array_almost_equal(
            rout.beta,
<<<<<<< HEAD
            np.array([ 11.4192022410781231,  -2.0850374506165474]),
        )
        assert_array_almost_equal(
            rout.sd_beta,
            np.array([ 0.9820231665657161,  0.3070515616198911]),
        )
        assert_array_almost_equal(
            rout.cov_beta,
            np.array([[ 0.6391799462548782, -0.1955657291119177],
               [-0.1955657291119177,  0.0624888159223392]]),
=======
            np.array([11.4192022410781231, -2.0850374506165474]),
        )
        assert_array_almost_equal(
            rout.sd_beta,
            np.array([0.9820231665657161, 0.3070515616198911]),
        )
        assert_array_almost_equal(
            rout.cov_beta,
            np.array([[0.6391799462548782, -0.1955657291119177],
               [-0.1955657291119177, 0.0624888159223392]]),
>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b
        )

    # Lorentz Peak
    # The data is taken from one of the undergraduate physics labs I performed.

    def lorentz(self, beta, x):
        return (beta[0]*beta[1]*beta[2] / np.sqrt(np.power(x*x -
            beta[2]*beta[2], 2.0) + np.power(beta[1]*x, 2.0)))

    def test_lorentz(self):
        l_sy = np.array([.29]*18)
        l_sx = np.array([.000972971,.000948268,.000707632,.000706679,
            .000706074, .000703918,.000698955,.000456856,
            .000455207,.000662717,.000654619,.000652694,
            .000000859202,.00106589,.00106378,.00125483, .00140818,.00241839])

        l_dat = RealData(
            [3.9094, 3.85945, 3.84976, 3.84716, 3.84551, 3.83964, 3.82608,
             3.78847, 3.78163, 3.72558, 3.70274, 3.6973, 3.67373, 3.65982,
             3.6562, 3.62498, 3.55525, 3.41886],
            [652, 910.5, 984, 1000, 1007.5, 1053, 1160.5, 1409.5, 1430, 1122,
             957.5, 920, 777.5, 709.5, 698, 578.5, 418.5, 275.5],
            sx=l_sx,
            sy=l_sy,
        )
        l_mod = Model(self.lorentz, meta=dict(name='Lorentz Peak'))
        l_odr = ODR(l_dat, l_mod, beta0=(1000., .1, 3.8))

        out = l_odr.run()
        assert_array_almost_equal(
            out.beta,
<<<<<<< HEAD
            np.array([  1.4306780846149925e+03,   1.3390509034538309e-01,
=======
            np.array([1.4306780846149925e+03, 1.3390509034538309e-01,
>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b
                 3.7798193600109009e+00]),
        )
        assert_array_almost_equal(
            out.sd_beta,
<<<<<<< HEAD
            np.array([  7.3621186811330963e-01,   3.5068899941471650e-04,
=======
            np.array([7.3621186811330963e-01, 3.5068899941471650e-04,
>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b
                 2.4451209281408992e-04]),
        )
        assert_array_almost_equal(
            out.cov_beta,
<<<<<<< HEAD
            np.array([[  2.4714409064597873e-01,  -6.9067261911110836e-05,
                 -3.1236953270424990e-05],
               [ -6.9067261911110836e-05,   5.6077531517333009e-08,
                  3.6133261832722601e-08],
               [ -3.1236953270424990e-05,   3.6133261832722601e-08,
                  2.7261220025171730e-08]]),
        )


if __name__ == "__main__":
    run_module_suite()
#### EOF #######################################################################
=======
            np.array([[2.4714409064597873e-01, -6.9067261911110836e-05,
                 -3.1236953270424990e-05],
               [-6.9067261911110836e-05, 5.6077531517333009e-08,
                  3.6133261832722601e-08],
               [-3.1236953270424990e-05, 3.6133261832722601e-08,
                  2.7261220025171730e-08]]),
        )

    def test_ticket_1253(self):
        def linear(c, x):
            return c[0]*x+c[1]

        c = [2.0, 3.0]
        x = np.linspace(0, 10)
        y = linear(c, x)

        model = Model(linear)
        data = Data(x, y, wd=1.0, we=1.0)
        job = ODR(data, model, beta0=[1.0, 1.0])
        result = job.run()
        assert_equal(result.info, 2)


if __name__ == "__main__":
    run_module_suite()
>>>>>>> 85b42d3bbdcef5cbe0fe2390bba8b3ff1608040b
