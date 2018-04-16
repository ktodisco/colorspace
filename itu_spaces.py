import pygame
import math
import numpy
import sys

import itu_defs

class CIEGrid(object):
	def __init__(self, x, y, size, intervals):
		self.x = x
		self.y = y
		self.step = size
		self.intervals = intervals
		self.height = self.step * self.intervals
		self.origin = [self.x, self.y + self.height]
		self.label = pygame.font.Font(None, 15)
		self.label_y_offset = 15
		self.label_x_offset = 15
		
	def draw(self, screen):
		# Draw the vertical slices.
		for x in range(0, self.intervals + 1):
			pygame.draw.line(screen, (255, 255, 255), \
				(self.x + self.step * x, self.y), \
				(self.x + self.step * x, self.origin[1]))
				
			label = self.label.render("%.1f" % (x / 10.0), 1, (255, 255, 255))
			label_rect = label.get_rect()
			label_rect.center = (self.x + self.step * x, self.origin[1] + self.label_y_offset)
			screen.blit(label, label_rect)
				
		for y in range(0, self.intervals + 1):
			pygame.draw.line(screen, (255, 255, 255), \
				(self.x, self.y + self.step * y), \
				(self.x + self.step * self.intervals, self.y + self.step * y))
			
			label = self.label.render("%.1f" % ((self.intervals - y) / 10.0), 1, (255, 255, 255))
			label_rect = label.get_rect()
			label_rect.center = (self.x - self.label_x_offset, self.y + self.step * y)
			screen.blit(label, label_rect)

class ColorSpace(object):
	def __init__(self, id):
		self.space = id
		self.primaries = itu_defs.primaries[self.space]
		
	def convert(self, to):
		# TODO: Use type reflection to look up
		pass
		
	def draw(self, screen, grid):		
		pointlist = [
			( \
				grid.x + (grid.step * x[0] * 10), \
				grid.y + (grid.intervals * grid.step) - (grid.step * x[1] * 10) \
			) for x in self.primaries \
		]
		
		pygame.draw.polygon(screen, (100, 100, 255), pointlist, 1)
		
def inverse_matrix(input):
	temp = [[0,0,0],[0,0,0],[0,0,0]]
	
	temp[0][0] = input[1][1] * input[2][2] - input[2][1] * input[1][2]
	temp[1][0] = -input[0][1] * input[2][2] + input[2][1] * input[0][2]
	temp[2][0] = input[0][1] * input[1][2] - input[1][1] * input[0][2]
	temp[0][1] = -input[1][0] * input[2][2] + input[1][2] * input[2][0]
	temp[1][1] = input[0][0] * input[2][2] - input[0][2] * input[2][0]
	temp[2][1] = -input[0][0] * input[1][2] + input[0][2] * input[1][0]
	temp[0][2] = input[1][0] * input[2][1] - input[1][1] * input[2][0]
	temp[1][2] = -input[0][0] * input[2][1] + input[0][1] * input[2][0]
	temp[2][2] = input[0][0] * input[1][1] - input[0][1] * input[1][0]

	inv_det = input[0][0] * temp[0][0] + input[1][0] * temp[1][0] + input[2][0] * temp[2][0];
	inv_det = 1.0 / inv_det

	result = [[0,0,0],[0,0,0],[0,0,0]]
	result[0][0] = temp[0][0] * inv_det
	result[1][0] = temp[0][1] * inv_det
	result[2][0] = temp[0][2] * inv_det
	result[0][1] = temp[1][0] * inv_det
	result[1][1] = temp[1][1] * inv_det
	result[2][1] = temp[1][2] * inv_det
	result[0][2] = temp[2][0] * inv_det
	result[1][2] = temp[2][1] * inv_det
	result[2][2] = temp[2][2] * inv_det
	
	return result
		
def transform_vector(vector, transform):
	return [ \
		sum([x[0] * x[1] for x in zip(vector, transform[0])]),
		sum([x[0] * x[1] for x in zip(vector, transform[1])]),
		sum([x[0] * x[1] for x in zip(vector, transform[2])])
	]
		
def compute_xyz_conversion(primaries, white_point):
	# Extract the primaries, for readability.
	x_r, y_r = primaries[0]
	x_g, y_g = primaries[1]
	x_b, y_b = primaries[2]
	
	X_r = x_r / y_r
	Y_r = 1.0
	Z_r = (1.0 - x_r - y_r) / y_r
	
	X_g = x_g / y_g
	Y_g = 1.0
	Z_g = (1.0 - x_g - y_g) / y_g
	
	X_b = x_b / y_b
	Y_b = 1.0
	Z_b = (1.0 - x_b - y_b) / y_b
	
	rgb_matrix = \
	[
		[X_r, X_g, X_b],
		[Y_r, Y_g, Y_b],
		[Z_r, Z_g, Z_b]
	]
	
	rgb_matrix_inverse = inverse_matrix(rgb_matrix)
	
	S_r, S_g, S_b = transform_vector(white_point, rgb_matrix_inverse)
	
	result = \
	[
		[X_r * S_r, X_g * S_g, X_b * S_b],
		[Y_r * S_r, Y_g * S_g, Y_b * S_b],
		[Z_r * S_r, Z_g * S_g, Z_b * S_b]
	]
	
	return result
	
def plot_color_xyz(screen, point_color, color_xyz, grid):
	if sum(color_xyz) == 0:
		return

	x_coord = color_xyz[0] / sum(color_xyz)
	y_coord = color_xyz[1] / sum(color_xyz)
	
	#print([x_coord, y_coord])
	
	pygame.draw.circle(screen, [x * 255 for x in point_color], \
		(int(grid.x + (grid.step * x_coord * 10)), int(grid.y + (grid.intervals * grid.step) - (grid.step * y_coord * 10))), 2)
	
def plot_color_709(screen, color, primaries, grid):
	convert_709_to_xyz = [ \
		[0.4124, 0.3576, 0.1805], \
		[0.2126, 0.7152, 0.0722], \
		[0.0193, 0.1192, 0.9505]]
		
	color_xyz = transform_vector(color, convert_709_to_xyz)
	
	convert_xyz_to_709 = [ \
		[3.2406, -1.5372, -0.4986], \
		[-0.9689, 1.8758, 0.0415], \
		[0.0557, -0.2040, 1.0570]]
	
	plot_color_xyz(screen, color, color_xyz, grid)
	
def plot_color_2020(screen, color, primaries, grid):
	#L = (1688.0 * color[0] + 2146.0 * color[1] + 262.0 * color[2]) / 4096.0
	#M = (683.0 * color[0] + 2951.0 * color[1] + 462.0 * color[2]) / 4096.0
	#S = (99.0 * color[0] + 309.0 * color[1] + 3688.0 * color[2]) / 4096.0
	#
	#color_lms = [L, M, S]
    #
	#convert_lms_to_xyz = [ \
	#	[1.8601, -1.1295, 0.2199], \
	#	[0.3612, 0.6388, 0], \
	#	[0, 0, 1.0891]]
	#	
	#color_xyz = transform_vector(color_lms, convert_lms_to_xyz)
	#
	#convert_xyz_to_lms = [ \
	#	[0.4002, 0.7076, -0.0808], \
	#	[-0.2263, 1.1653, 0.0457], \
	#	[0, 0, 0.9182]]
	
	convert_2020_to_xyz = [ \
		[0.6372, 0.1446, 0.1689], \
		[0.2628, 0.6780, 0.0593], \
		[0.0000, 0.0281, 1.0610]]
		
	color_xyz = transform_vector(color, convert_2020_to_xyz)
	
	plot_color_xyz(screen, color, color_xyz, grid)

pygame.init()

screen = pygame.display.set_mode((1280,720))
clock = pygame.time.Clock()

primaries_709 = [(0.64, 0.33), (0.30, 0.60), (0.15, 0.06)]
primaries_2020 = [(0.708, 0.292), (0.170, 0.797), (0.131, 0.046)]
d65_illuminant = [0.9505, 1.0, 1.0888]

color_709 = [1.0, 1.0, 0.0]

convert_709_to_2020 = [ \
	[ 0.6274, 0.3293, 0.0433 ], \
	[ 0.0691, 0.9195, 0.0114 ], \
	[ 0.0164, 0.0880, 0.8956 ]]
	
color_2020 = transform_vector(color_709, convert_709_to_2020)

cie_grid = CIEGrid(50, 50, 50, 9)
sRGB = ColorSpace(itu_defs.ColorSpacesRGB)
Rec2020 = ColorSpace(itu_defs.ColorSpaceRec2020)

scale_image_x = 384
scale_image_y = 240

ref_image = pygame.image.load(sys.argv[1])
ref_image = pygame.transform.scale(ref_image, (scale_image_x, scale_image_y))
ref_image_rect = ref_image.get_rect()
ref_image_rect.topleft = (600, 50)

# TODO: Accelerate with numpy.
# Pre-compute all of the xyz coordinates.
convert_709_to_xyz = compute_xyz_conversion(primaries_709, d65_illuminant)
xyz_plots = []
for x in range(scale_image_x):
	for y in range(scale_image_y):
		r, g, b, a = ref_image.get_at((x, y))
		r /= 255.0
		g /= 255.0
		b /= 255.0
		xyz_plots.append(([r, g, b], transform_vector([r, g, b], convert_709_to_xyz)))

while True:
	# pygame stuff
	clock.tick(60)
	screen.fill((0,0,0))
    
	# event handling
	for event in pygame.event.get():
		if event.type == pygame.KEYDOWN:
			if event.key == pygame.K_ESCAPE:
				pygame.quit()
				sys.exit()
		if event.type == pygame.QUIT:
			pygame.quit()
			sys.exit()
				
	cie_grid.draw(screen)
	sRGB.draw(screen, cie_grid)
	Rec2020.draw(screen, cie_grid)
	
	#plot_color_709(screen, color_709, primaries_709, cie_grid)
	#plot_color_2020(screen, color_2020, primaries_2020, cie_grid)
	
	for plot in xyz_plots:
		plot_color_xyz(screen, plot[0], plot[1], cie_grid)
	
	screen.blit(ref_image, ref_image_rect)
    
	pygame.display.update()
