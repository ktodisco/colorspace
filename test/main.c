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

static bool _almost_equal_mat(color_matrix_t* a, color_matrix_t* b)
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

static void _print_matrix(color_matrix_t* m)
{
	printf("[ %.8f, %.8f, %.8f,\n  %.8f, %.8f, %.8f, \n  %.8f, %.8f, %.8f ]\n\n",
		m->data[0], m->data[1], m->data[2],
		m->data[3], m->data[4], m->data[5],
		m->data[6], m->data[7], m->data[8]);
}

static void _test_color_matrix_invert(void)
{
	color_matrix_t t0 = {
		1.0f, 4.0f, 7.0f,
		2.0f, 5.0f, 2.0f,
		7.0f, 4.0f, 1.0f
	};

	color_matrix_t r0 = color_matrix_invert(&t0);
	color_matrix_t e0 = {
		 0.020833334f, -0.166666672f,  0.187500000f,
		-0.083333336f,  0.333333334f, -0.083333336f,
		 0.187500000f, -0.166666672f,  0.020833334f
	};
	assert(_almost_equal_mat(&r0, &e0));
}

static void _test_color_matrix_multiply(void)
{
	// TODO.
}

static void _test_convert_color_vec(void)
{
	color_matrix_t rotate = {
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f
	};

	color_vec_t t0 = { 1.0f, 2.0f, 3.0f };
	color_vec_t r0 = convert_color_vec(&rotate, &t0);
	color_vec_t e0 = { 2.0f, 3.0f, 1.0f };
	assert(_almost_equal_vec(&r0, &e0));

	color_matrix_t scale = {
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
	// TODO.
}

static void _test_make_xyz_conversion(void)
{
	// TODO.
}

static void _test_make_colorspace_conversion(void)
{
	// TODO.
}

static void _test_make_protanopia_transform(void)
{
	// TODO.
}

static void _test_make_deuteranopia_transform(void)
{
	// TODO.
}

static void _test_make_tritanopia_transform(void)
{
	// TODO.
}

int main(void)
{
	_test_color_matrix_invert();
	_test_color_matrix_multiply();
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