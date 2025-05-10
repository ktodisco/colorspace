/*
** Colorspace - A compact library for color mathematics.
**
** Copyright (c) 2025, Kevin Todisco
** MIT License.
*/

#include "colorspace.h"

cie_xyy_t WHITEPOINT_D50 = _WHITEPOINT_D50;
cie_xyy_t WHITEPOINT_D65 = _WHITEPOINT_D65;

colorspace_t COLORSPACE_SRGB = { .primaries = _PRIMARIES_BT709, .whitepoint = _WHITEPOINT_D65 };
colorspace_t COLORSPACE_HDR10 = { .primaries = _PRIMARIES_BT2020, .whitepoint = _WHITEPOINT_D65 };

color_mat_t FUNDAMENTALS_SMITH_POKORNY = _FUNDAMENTALS_SMITH_POKORNY;
color_mat_t FUNDAMENTALS_HPE_D65 = _FUNDAMENTALS_HPE_D65;
color_mat_t FUNDAMENTALS_STOCKMAN_SHARPE = _FUNDAMENTALS_STOCKMAN_SHARPE;

color_mat_t color_mat_invert(const color_mat_t* m)
{
	color_mat_t tmp;
	tmp.data[0] =  m->data[4] * m->data[8] - m->data[5] * m->data[7];
	tmp.data[1] = -m->data[3] * m->data[8] + m->data[5] * m->data[6];
	tmp.data[2] =  m->data[3] * m->data[7] - m->data[4] * m->data[6];
	tmp.data[3] = -m->data[1] * m->data[8] + m->data[7] * m->data[2];
	tmp.data[4] =  m->data[0] * m->data[8] - m->data[6] * m->data[2];
	tmp.data[5] = -m->data[0] * m->data[7] + m->data[6] * m->data[1];
	tmp.data[6] =  m->data[1] * m->data[5] - m->data[4] * m->data[2];
	tmp.data[7] = -m->data[0] * m->data[5] + m->data[3] * m->data[2];
	tmp.data[8] =  m->data[0] * m->data[4] - m->data[3] * m->data[1];

	float inv_det = m->data[0] * tmp.data[0] + m->data[1] * tmp.data[1] + m->data[2] * tmp.data[2];
	inv_det = 1.0f / inv_det;

	return (color_mat_t){
		tmp.data[0] * inv_det,
		tmp.data[3] * inv_det,
		tmp.data[6] * inv_det,
		tmp.data[1] * inv_det,
		tmp.data[4] * inv_det,
		tmp.data[7] * inv_det,
		tmp.data[2] * inv_det,
		tmp.data[5] * inv_det,
		tmp.data[8] * inv_det,
	};
}

color_mat_t color_mat_multiply(const color_mat_t* a, const color_mat_t* b)
{
	color_mat_t result;
	result.data[0] = a->data[0] * b->data[0] + a->data[1] * b->data[3] + a->data[2] * b->data[6];
	result.data[1] = a->data[0] * b->data[1] + a->data[1] * b->data[4] + a->data[2] * b->data[7];
	result.data[2] = a->data[0] * b->data[2] + a->data[1] * b->data[5] + a->data[2] * b->data[8];
	result.data[3] = a->data[3] * b->data[0] + a->data[4] * b->data[3] + a->data[5] * b->data[6];
	result.data[4] = a->data[3] * b->data[1] + a->data[4] * b->data[4] + a->data[5] * b->data[7];
	result.data[5] = a->data[3] * b->data[2] + a->data[4] * b->data[5] + a->data[5] * b->data[8];
	result.data[6] = a->data[6] * b->data[0] + a->data[7] * b->data[3] + a->data[8] * b->data[6];
	result.data[7] = a->data[6] * b->data[1] + a->data[7] * b->data[4] + a->data[8] * b->data[7];
	result.data[8] = a->data[6] * b->data[2] + a->data[7] * b->data[5] + a->data[8] * b->data[8];

	return result;
}

color_vec_t convert_color_vec(const color_mat_t* mat, const color_vec_t* vec)
{
	return (color_vec_t){
		vec->data[0] * mat->data[0] + vec->data[1] * mat->data[1] + vec->data[2] * mat->data[2],
		vec->data[0] * mat->data[3] + vec->data[1] * mat->data[4] + vec->data[2] * mat->data[5],
		vec->data[0] * mat->data[6] + vec->data[1] * mat->data[7] + vec->data[2] * mat->data[8]
	};
}

cie_xyz_t convert_xyy_to_xyz(const cie_xyy_t* xyy)
{
	float x = (xyy->data[0] * xyy->data[2]) / xyy->data[1];
	float y = xyy->data[2];
	float z = (1.0f - xyy->data[0] - xyy->data[1]) * xyy->data[2] / xyy->data[1];

	return (cie_xyz_t){ .X = x, .Y = y, .Z = z };
}

color_mat_t make_xyz_conversion(const colorspace_t* colorspace)
{
	const cie_xyy_t* pr = &colorspace->primaries[0];
	const cie_xyy_t* pg = &colorspace->primaries[1];
	const cie_xyy_t* pb = &colorspace->primaries[2];

	float x_r = pr->data[0] / pr->data[1];
	float y_r = 1.0f;
	float z_r = (1.0f - pr->data[0] - pr->data[1] ) / pr->data[1];

	float x_g = pg->data[0] / pg->data[1];
	float y_g = 1.0f;
	float z_g = (1.0f - pg->data[0] - pg->data[1] ) / pg->data[1];

	float x_b = pb->data[0] / pb->data[1];
	float y_b = 1.0f;
	float z_b = (1.0f - pb->data[0] - pb->data[1] ) / pb->data[1];

	color_mat_t m = {
		x_r, x_g, x_b,
		y_r, y_g, y_b,
		z_r, z_g, z_b
	};

	color_mat_t m_inv = color_mat_invert(&m);

	cie_xyz_t wp_xyz = convert_xyy_to_xyz(&colorspace->whitepoint);

	cie_xyz_t s;
	s.vec = convert_color_vec(&m_inv, &wp_xyz.vec);

	return (color_mat_t){
		x_r * s.X, x_g * s.Y, x_b * s.Z,
		y_r * s.X, y_g * s.Y, y_b * s.Z,
		z_r * s.X, z_g * s.Y, z_b * s.Z,
	};
}

color_mat_t make_colorspace_conversion(const colorspace_t* colorspace_a, const colorspace_t* colorspace_b)
{
	color_mat_t a_to_xyz = make_xyz_conversion(colorspace_a);
	color_mat_t b_to_xyz = make_xyz_conversion(colorspace_b);

	color_mat_t xyz_to_b = color_mat_invert(&b_to_xyz);

	return color_mat_multiply(&xyz_to_b, &a_to_xyz);
}

color_vec_t _get_alpha_beta_gamma(const colorspace_t* colorspace)
{
	const color_mat_t convert_xyz_to_lms = {
		 0.4002f, 0.7076f, -0.0808f,
		-0.2263f, 1.1653f,  0.0457f,
		 0.0000f, 0.0000f,  0.9182f
	};

	cie_xyz_t blue_xyz = convert_xyy_to_xyz(&colorspace->primaries[2]);
	cie_xyz_t wp_xyz = convert_xyy_to_xyz(&colorspace->whitepoint);

	color_vec_t blue_lms = convert_color_vec(&convert_xyz_to_lms, &blue_xyz.vec);
	color_vec_t wp_lms = convert_color_vec(&convert_xyz_to_lms, &wp_xyz.vec);

	// Pull out values for readability.
	float lb = blue_lms.data[0];
	float mb = blue_lms.data[1];
	float sb = blue_lms.data[2];
	float lw = wp_lms.data[0];
	float mw = wp_lms.data[1];
	float sw = wp_lms.data[2];

	return (color_vec_t){
		mw * sb - mb * sw,
		sw * lb - sb * lw,
		lw * mb - lb * mw
	};
}

color_mat_t make_protanopia_transform(const colorspace_t* colorspace)
{
	color_vec_t abg = _get_alpha_beta_gamma(colorspace);

	float alpha = abg.data[0];
	float beta = abg.data[1];
	float gamma = abg.data[2];

	return (color_mat_t){
		0.0f, -beta / alpha, -gamma / alpha,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

color_mat_t make_deuteranopia_transform(const colorspace_t* colorspace)
{
	color_vec_t abg = _get_alpha_beta_gamma(colorspace);

	float alpha = abg.data[0];
	float beta = abg.data[1];
	float gamma = abg.data[2];

	return (color_mat_t){
		1.0f, 0.0f, 0.0f,
		-alpha / beta, 0.0f, -gamma / beta,
		0.0f, 0.0f, 1.0f
	};
}

color_mat_t make_tritanopia_transform(const colorspace_t* colorspace)
{
	color_vec_t abg = _get_alpha_beta_gamma(colorspace);

	float alpha = abg.data[0];
	float beta = abg.data[1];
	float gamma = abg.data[2];

	return (color_mat_t){
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		-alpha / gamma, -beta / gamma, 0.0f
	};
}