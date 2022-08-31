from colortypes import *
import numpy

PRIMARIES_BT709  = [xyY(0.640, 0.330, 0.2126), xyY(0.300, 0.600, 0.7152), xyY(0.150, 0.060, 0.0722)]
PRIMARIES_BT2020 = [xyY(0.708, 0.292, 0.2627), xyY(0.170, 0.797, 0.6780), xyY(0.131, 0.046, 0.0593)]

WHITEPOINT_D65 = xyY(0.31271, 0.32902, 1.0000)
WHITEPOINT_D50 = xyY(0.34567, 0.35850, 1.0000)

COLORSPACE_SRGB = ColorSpace(PRIMARIES_BT709,  WHITEPOINT_D65)
COLORSPACE_HDR  = ColorSpace(PRIMARIES_BT2020, WHITEPOINT_D65)

FUNDAMENTALS_SMITH_POKORNY = numpy.array([  \
    [  0.15514,  0.54312, -0.03286 ],       \
    [ -0.15514,  0.45684,  0.03286 ],       \
    [  0.00000,  0.00000,  0.00801 ]])

FUNDAMENTALS_HPE_D65 = numpy.array([        \
    [  0.40020,  0.70760, -0.08080 ],       \
    [ -0.22630,  1.16530,  0.04570 ],       \
    [  0.00000,  0.00000,  0.91820 ]])
    
FUNDAMENTALS_STOCKMAN_SHARPE = numpy.array([\
    [  0.21058,  0.85510, -0.03970 ],       \
    [ -0.41708,  1.17726,  0.07863 ],       \
    [  0.00000,  0.00000,  0.51684 ]])
