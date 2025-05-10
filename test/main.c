/*
** Colorspace - A compact library for color mathematics.
**
** Copyright (c) 2025, Kevin Todisco
** MIT License.
*/

#include "colorspace.h"

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

static bool _almost_equalf(float a, float b)
{
	float diff = fabsf(a - b);
	a = fabsf(a);
	b = fabsf(b);
	float largest = (a > b) ? a : b;

	if (diff <= largest * FLT_EPSILON)
		return true;

	return false;
}

static bool _almost_equal_vec(color_vec_t* a, color_vec_t* b)
{
	return _almost_equalf(a->data[0], b->data[0]) &&
		_almost_equalf(a->data[1], b->data[1]) &&
		_almost_equalf(a->data[2], b->data[2]);
}

static bool _almost_equal_mat(color_mat_t* a, color_mat_t* b)
{
	return _almost_equalf(a->data[0], b->data[0]) &&
		_almost_equalf(a->data[1], b->data[1]) &&
		_almost_equalf(a->data[2], b->data[2]) &&
		_almost_equalf(a->data[3], b->data[3]) &&
		_almost_equalf(a->data[4], b->data[4]) &&
		_almost_equalf(a->data[5], b->data[5]) &&
		_almost_equalf(a->data[6], b->data[6]) &&
		_almost_equalf(a->data[7], b->data[7]) &&
		_almost_equalf(a->data[8], b->data[8]);
}

static void _print_matrix(color_mat_t* m)
{
	printf("[ %.8f, %.8f, %.8f,\n  %.8f, %.8f, %.8f, \n  %.8f, %.8f, %.8f ]\n\n",
		m->data[0], m->data[1], m->data[2],
		m->data[3], m->data[4], m->data[5],
		m->data[6], m->data[7], m->data[8]);
}

static void _test_color_mat_invert(void)
{
	color_mat_t t0 = {
		1.0f, 4.0f, 7.0f,
		2.0f, 5.0f, 2.0f,
		7.0f, 4.0f, 1.0f
	};

	color_mat_t r0 = color_mat_invert(&t0);
	color_mat_t e0 = {
		 0.020833334f, -0.166666672f,  0.187500000f,
		-0.083333336f,  0.333333334f, -0.083333336f,
		 0.187500000f, -0.166666672f,  0.020833334f
	};
	assert(_almost_equal_mat(&r0, &e0));
}

static void _test_color_mat_multiply(void)
{
	color_mat_t t0 = {
		1.0f, 2.0f, 3.0f,
		4.0f, 5.0f, 6.0f,
		7.0f, 8.0f, 9.0f
	};
	color_mat_t t1 = {
		9.0f, 8.0f, 7.0f,
		6.0f, 5.0f, 4.0f,
		3.0f, 2.0f, 1.0f
	};

	color_mat_t r0 = color_mat_multiply(&t0, &t1);
	color_mat_t e0 = {
		 30.0f,  24.0f, 18.0f,
		 84.0f,  69.0f, 54.0f,
		138.0f, 114.0f, 90.0f
	};
	assert(_almost_equal_mat(&r0, &e0));
}

static void _test_convert_color_vec(void)
{
	color_mat_t rotate = {
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f
	};

	color_vec_t t0 = { 1.0f, 2.0f, 3.0f };
	color_vec_t r0 = convert_color_vec(&rotate, &t0);
	color_vec_t e0 = { 2.0f, 3.0f, 1.0f };
	assert(_almost_equal_vec(&r0, &e0));

	color_mat_t scale = {
		5.0f, 0.0f, 0.0f,
		0.0f, 5.0f, 0.0f,
		0.0f, 0.0f, 5.0f
	};

	color_vec_t t1 = { 2.0f, 3.0f, 4.0f };
	color_vec_t r1 = convert_color_vec(&scale, &t1);
	color_vec_t e1 = { 10.0f, 15.0f, 20.0f };
	assert(_almost_equal_vec(&r1, &e1));
}

static void _test_convert_xyy_to_xyz(void)
{
	cie_xyy_t t0 = { .x = 0.640f, .y = 0.330f, .Y = 0.2126f };
	cie_xyz_t r0 = convert_xyy_to_xyz(&t0);
	cie_xyz_t e0 = { .X = 0.412315100f, .Y = 0.2126f, .Z = 0.0193272717f };
	assert(_almost_equal_vec(&r0.vec, &e0.vec));

	cie_xyy_t t1 = { .x = 0.3f, .y = 0.6f, .Y = 0.7152f };
	cie_xyz_t r1 = convert_xyy_to_xyz(&t1);
	cie_xyz_t e1 = { .X = 0.3576f, .Y = 0.7152f, .Z = 0.119199961f };
	assert(_almost_equal_vec(&r1.vec, &e1.vec));

	cie_xyy_t t2 = { .x = 0.15f, .y = 0.06f, .Y = 0.0722f };
	cie_xyz_t r2 = convert_xyy_to_xyz(&t2);
	cie_xyz_t e2 = { .X = 0.1805f, .Y = 0.0722f, .Z = 0.950633333f };
	assert(_almost_equal_vec(&r2.vec, &e2.vec));
}

static void _test_make_xyz_conversion(void)
{
	color_mat_t r0 = make_xyz_conversion(&COLORSPACE_SRGB);
	color_mat_t e0 = {
		0.412386447f, 0.35759151f, 0.180450544f,
		0.212636769f, 0.71518302f, 0.072180212f,
		0.019330615f, 0.11919712f, 0.950372815f
	};
	assert(_almost_equal_mat(&r0, &e0));

	color_mat_t r1 = make_xyz_conversion(&COLORSPACE_HDR10);
	color_mat_t e1 = {
		0.636953533f, 0.144619182f, 0.168855861f,
		0.262698352f, 0.678008735f, 0.0592928976f,
		0.0f, 0.0280731283f, 1.06082726f
	};
	assert(_almost_equal_mat(&r1, &e1));
}

static void _test_make_colorspace_conversion(void)
{
	color_mat_t r0 = make_colorspace_conversion(&COLORSPACE_SRGB, &COLORSPACE_HDR10);
	color_mat_t e0 = {
		0.627401710f, 0.32929197f, 0.0433061272f,
		0.0690954849f, 0.919544339f, 0.0113602262f,
		0.0163937043f, 0.0880281329f, 0.895578384f
	};
	assert(_almost_equal_mat(&r0, &e0));

	color_mat_t r1 = make_colorspace_conversion(&COLORSPACE_HDR10, &COLORSPACE_SRGB);
	color_mat_t e1 = {
		1.66049695f, -0.587656677f, -0.0728398860f,
		-0.124547064f, 1.13289511f, -0.00834799558f,
		-0.0181536824f, -0.100597285f, 1.11875093f
	};
	assert(_almost_equal_mat(&r1, &e1));
}

static void _test_make_protanopia_transform(void)
{
	color_mat_t r0 = make_protanopia_transform(&COLORSPACE_SRGB);
	color_mat_t e0 = {
		0.0f, 1.05114746f, -0.0511574671f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	assert(_almost_equal_mat(&r0, &e0));

	color_mat_t r1 = make_protanopia_transform(&COLORSPACE_HDR10);
	color_mat_t e1 = {
		0.0f, 1.06206989f, -0.0620817207f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	assert(_almost_equal_mat(&r1, &e1));
}

static void _test_make_deuteranopia_transform(void)
{
	color_mat_t r0 = make_deuteranopia_transform(&COLORSPACE_SRGB);
	color_mat_t e0 = {
		1.0f, 0.0f, 0.0f,
		0.951341331f, 0.0f, 0.0486682132f,
		0.0f, 0.0f, 1.0f
	};
	assert(_almost_equal_mat(&r0, &e0));

	color_mat_t r1 = make_deuteranopia_transform(&COLORSPACE_HDR10);
	color_mat_t e1 = {
		1.0f, 0.0f, 0.0f,
		0.941557586f, 0.0f, 0.0584535152f,
		0.0f, 0.0f, 1.0f
	};
	assert(_almost_equal_mat(&r1, &e1));
}

static void _test_make_tritanopia_transform(void)
{
	color_mat_t r0 = make_tritanopia_transform(&COLORSPACE_SRGB);
	color_mat_t e0 = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		-19.5474892f, 20.5472927f, 0.0f
	};
	assert(_almost_equal_mat(&r0, &e0));

	color_mat_t r1 = make_tritanopia_transform(&COLORSPACE_HDR10);
	color_mat_t e1 = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		-16.1078014f, 17.1076107f, 0.0f
	};
	assert(_almost_equal_mat(&r1, &e1));
}

int main(void)
{
	_test_color_mat_invert();
	_test_color_mat_multiply();
	_test_convert_color_vec();
	_test_convert_xyy_to_xyz();
	_test_make_xyz_conversion();
	_test_make_colorspace_conversion();
	_test_make_protanopia_transform();
	_test_make_deuteranopia_transform();
	_test_make_tritanopia_transform();

	printf("All test passed successfully.\n");

	return 0;
}