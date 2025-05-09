/*
** Colorspace - A compact library for color mathematics.
**
** Copyright (c) 2025, Kevin Todisco
** MIT License.
*/

#ifndef _COLORSPACE_H_
#define _COLORSPACE_H_

typedef struct _color_matrix_t
{
	float data[9];
} color_matrix_t;

typedef struct _color_vec_t
{
	float data[3];
} color_vec_t;

typedef struct _color_rgb_t
{
	union
	{
		float data[3];
		color_vec_t vec;
		struct
		{
			float r;
			float g;
			float b;
		};
	};
} color_rgb_t;

typedef struct _cie_xyy_t
{
	union
	{
		float data[3];
		color_vec_t vec;
		struct
		{
			float x;
			float y;
			float Y;
		};
	};
} cie_xyy_t;

typedef struct _cie_xyz_t
{
	union
	{
		float data[3];
		color_vec_t vec;
		struct
		{
			float X;
			float Y;
			float Z;
		};
	};
} cie_xyz_t;

typedef struct _colorspace_t
{
	cie_xyy_t primaries[3];
	cie_xyy_t whitepoint;
} colorspace_t;

/*
** Common color space operations.
*/

/*
** Inverts the 3x3 color matrix.
** @param m The matrix to invert.
*/
color_matrix_t color_matrix_invert(const color_matrix_t* m);

/*
** Multiplies two matrices.
** Multiplication takes the form:
**      * |b|  
**  |a| = |r|
** @param a First matrix to multiply.
** @param b Second matrix to multiply.
*/
color_matrix_t color_matrix_multiply(const color_matrix_t* a, const color_matrix_t* b);

/*
** Transform a vector by a matrix.
** @param mat The matrix to transform the vector with.
** @param vec The vector to transform.
*/
color_vec_t convert_color_vec(const color_matrix_t* mat, const color_vec_t* vec); 

/*
** Convert a CIE xyY triplet into a CIE XYZ color representation.
** @param color An xyY triplet.
*/
cie_xyz_t convert_xyy_to_xyz(const cie_xyy_t* xyy);

/*
** Constructs a matrix that converts a color from the color space provided into XYZ space.
** Based on: http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
** @param colorspace A Colorspace object.
*/
color_matrix_t make_xyz_conversion(const colorspace_t* colorspace);

/*
** Constructs a matrix which can be used to convert from one colorspace
** into a different colorspace. The XYZ space is used as the intermediary
** space into which colors are converted, and the final matrix is the
** combined linear transform A -> XYZ -> B.
** colorspace_a A Colorspace object to convert from.
** colorspace_b A Colorspace object to convert to.
*/
color_matrix_t make_colorspace_conversion(const colorspace_t* colorspace_a, const colorspace_t* colorspace_b);

/*
** Constructs a matrix for a given colorspace which can be used to emulate protanope
** color vision deficiency.
** Transforming ordinary colors by this matrix will yield an approximation of those
** colors as seen by someone with protanopia.
** @param colorspace The color space for which to compute the emulation matrix.
*/
color_matrix_t make_protanopia_transform(const colorspace_t* colorspace);

/*
** Constructs a matrix for a given colorspace which can be used to emulate deuteranope
** color vision deficiency.
** Transforming ordinary colors by this matrix will yield an approximation of those
** colors as seen by someone with deuteranopia.
** @param colorspace The color space for which to compute the emulation matrix.
*/
color_matrix_t make_deuteranopia_transform(const colorspace_t* colorspace);

/*
** Constructs a matrix for a given colorspace which can be used to emulate tritanope
** color vision deficiency.
** Transforming ordinary colors by this matrix will yield an approximation of those
** colors as seen by someone with tritanopia.
** @param colorspace The color space for which to compute the emulation matrix.
*/
color_matrix_t make_tritanopia_transform(const colorspace_t* colorspace);

/*
** Common color spaces and components.
*/

#define _PRIMARIES_BT709 { \
		{ .x = 0.640f, .y = 0.330f, .Y = 0.2126f }, \
		{ .x = 0.300f, .y = 0.600f, .Y = 0.7152f }, \
		{ .x = 0.150f, .y = 0.060f, .Y = 0.0722f }, \
	}
#define _PRIMARIES_BT2020 { \
		{ .x = 0.708f, .y = 0.292f, .Y = 0.2627f }, \
		{ .x = 0.170f, .y = 0.797f, .Y = 0.6780f }, \
		{ .x = 0.131f, .y = 0.046f, .Y = 0.0593f }, \
	}
#define _WHITEPOINT_D50 { .x = 0.34567f, .y = 0.35850f, .Y = 1.0f }
#define _WHITEPOINT_D65 { .x = 0.31271f, .y = 0.32902f, .Y = 1.0f }

#define _FUNDAMENTALS_SMITH_POKORNY { \
		 0.15514f,  0.54312f, -0.03286f, \
		-0.15514f,  0.45684f,  0.03286f, \
		 0.00000f,  0.00000f,  0.00801f, \
	}
#define _FUNDAMENTALS_HPE_D65 { \
		 0.40020f,  0.70760f, -0.08080f, \
		-0.22630f,  1.16530f,  0.04570f, \
		 0.00000f,  0.00000f,  0.91820f, \
	}
#define _FUNDAMENTALS_STOCKMAN_SHARPE { \
		 0.21058f,  0.85510f, -0.03970f, \
		-0.41708f,  1.17726f,  0.07863f, \
		 0.00000f,  0.00000f,  0.51684f, \
	}

extern cie_xyy_t WHITEPOINT_D50;
extern cie_xyy_t WHITEPOINT_D65;
extern colorspace_t COLORSPACE_SRGB;
extern colorspace_t COLORSPACE_HDR10;

extern color_matrix_t FUNDAMENTALS_SMITH_POKORNY;
extern color_matrix_t FUNDAMENTALS_HPE_D65;
extern color_matrix_t FUNDAMENTALS_STOCKMAN_SHARPE;

#endif
