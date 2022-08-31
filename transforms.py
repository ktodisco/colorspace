import numpy

from colortypes import *

# Convert an xyY triplet into an XYZ color representation.
# color: An xyY triplet.
def xyY_to_XYZ(color):
    X = (color[0] * color[2]) / color[1]
    Y = color[2]
    Z = (1 - color[0] - color[1]) * color[2] / color[1]
    
    return XYZ(X, Y, Z)

# http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
# Computes a matrix the converts a color from the color space provided
# into XYZ space.
# space: A Colorspace object.
def compute_XYZ_conversion(space):
    # Extract the primaries, for readability.
    x_r, y_r, _ = space.primaries[0]
    x_g, y_g, _ = space.primaries[1]
    x_b, y_b, _ = space.primaries[2]
    
    X_r = x_r / y_r
    Y_r = 1.0
    Z_r = (1.0 - x_r - y_r) / y_r
    
    X_g = x_g / y_g
    Y_g = 1.0
    Z_g = (1.0 - x_g - y_g) / y_g
    
    X_b = x_b / y_b
    Y_b = 1.0
    Z_b = (1.0 - x_b - y_b) / y_b
    
    rgb_matrix = numpy.array( \
    [
        [X_r, X_g, X_b],
        [Y_r, Y_g, Y_b],
        [Z_r, Z_g, Z_b]
    ])
    
    rgb_matrix_inverse = numpy.linalg.inv(rgb_matrix)
    
    white_point_XYZ = xyY_to_XYZ(space.white_point)
    
    S_r, S_g, S_b = numpy.matmul(rgb_matrix_inverse, white_point_XYZ.asarray())
    
    result = numpy.array( \
    [
        [X_r * S_r, X_g * S_g, X_b * S_b],
        [Y_r * S_r, Y_g * S_g, Y_b * S_b],
        [Z_r * S_r, Z_g * S_g, Z_b * S_b]
    ])
    
    return result
    
def compute_LMS_conversion(space, XYZ_to_LMS):
    space_to_XYZ = compute_XYZ_conversion(space)
    return numpy.matmul(XYZ_to_LMS, space_to_XYZ)

# http://vision.psychol.cam.ac.uk/jdmollon/papers/colourmaps.pdf
# Computes the matrices that daltonize colors from the color space provided.
# The matrices are returned in an array in the order:
#   [ Protanopia, Deuteranopia, Tritanopia ]
# space: A Colorspace object.
def compute_daltonization_transforms(space):
    convert_xyz_to_lms = numpy.array( \
        [ \
            [ 0.4002, 0.7076, -0.0808 ],
            [ -0.2263, 1.1653, 0.0457 ],
            [ 0, 0, 0.9182 ]
        ])
        
    blue_XYZ = xyY_to_XYZ(space.primaries[2])
    white_XYZ = xyY_to_XYZ(space.white_point)

    blue_LMS = numpy.matmul(convert_xyz_to_lms, blue_XYZ.asarray())
    white_LMS = numpy.matmul(convert_xyz_to_lms, white_XYZ.asarray())

    # Extract for readability.
    Lb = blue_LMS[0]
    Mb = blue_LMS[1]
    Sb = blue_LMS[2]
    Lw = white_LMS[0]
    Mw = white_LMS[1]
    Sw = white_LMS[2]

    alpha = Mw * Sb - Mb * Sw
    beta = Sw * Lb - Sb * Lw
    gamma = Lw * Mb - Lb * Mw
    
    protanope = numpy.array( \
        [ \
            [ 0, -beta / alpha, -gamma / alpha ],
            [ 0, 1, 0 ],
            [ 0, 0, 1 ]
        ])
        
    deuteranope = numpy.array( \
        [ \
            [ 1, 0, 0 ],
            [ -alpha / beta, 0, -gamma / beta ],
            [ 0, 0, 1 ]
        ])
        
    tritanope = numpy.array( \
        [ \
            [ 1, 0, 0 ],
            [ 0, 1, 0 ],
            [ -alpha / gamma, -beta / gamma, 0 ]
        ])
    
    return [protanope, deuteranope, tritanope]

# Computes a matrix which can be used to convert from one colorspace
# into a different colorspace. The XYZ space is used as the intermediary
# space into which colors are converted, and the final matrix is the
# combined linear transform space1 -> XYZ -> space2.
# space1: A Colorspace object to convert from.
# space2: A Colorspace object to convert to.
def compute_colorspace_conversion(space1, space2):
    # Calculate the conversion matrices for both spaces into XYZ space.
    space1_to_xyz = compute_XYZ_conversion(space1)
    space2_to_xyz = compute_XYZ_conversion(space2)
    
    # Calcuate the conversion matrix from XYZ to the second space.
    xyz_to_space2 = numpy.linalg.inv(space2_to_xyz)
    
    # Use XYZ as the intermediary space to derive a single conversion matrix.
    return numpy.matmul(xyz_to_space2, space1_to_xyz)
    