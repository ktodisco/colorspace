from transforms import *
import pygame

class CIEGrid(object):
    def __init__(self, x, y, size, intervals):
        self.x = x
        self.y = y
        self.intervals = intervals
        self.step = size / self.intervals
        self.height = size
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
            
class Waveform(object):
    def __init__(self, x, y, width, height, maxlum):
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.maxlum = maxlum
        self.label = pygame.font.Font(None, 15)
        self.label_x_offset = 15
        self.intervals = 8
        self.data = {}
        self.image_width = 0
        self.padding = 10
        self.surf = pygame.Surface((width + self.label_x_offset + self.padding, height + self.padding))
        self.origin = [self.label_x_offset + self.padding, self.height]
        
    def analyze(self, image):
        image_array = pygame.surfarray.array3d(image)
        
        self.image_width = image.get_width()
        for x in range(self.image_width):
            vert = image_array[x,:,:]
            self.data[x] = vert
            
        self.render(self.surf)
        
    def render(self, surface):
        step = self.height / self.intervals
        
        # Draw the horizontal grid.
        for y in range(self.intervals + 1):
            pygame.draw.line(surface, (255, 255, 255), \
                (self.origin[0], self.origin[1] - step * y), \
                (self.origin[0] + self.width, self.origin[1] - step * y))
                
            label = self.label.render("%.1f" % ((self.maxlum / self.intervals) * y), 1, (255, 255, 255))
            label_rect = label.get_rect()
            label_rect.center = (self.origin[0] - self.label_x_offset, self.origin[1] - step * y)
            surface.blit(label, label_rect)
        
        lum_vec = numpy.array([0.2126, 0.7152, 0.0722])
            
        # Plot the data.
        for key in self.data.keys():
            for y in self.data[key]:
                val = numpy.dot(y, lum_vec) / 255.0
                pygame.draw.circle(surface, y, \
                    (int(self.origin[0] + (self.width * (key / self.image_width))), int(self.origin[1] - (val * self.height))), 1)
                    
    def draw(self, screen):
        surf_rect = self.surf.get_rect()
        surf_rect.topleft = (self.x - self.label_x_offset - self.padding, self.y + self.padding)
        screen.blit(self.surf, surf_rect)
            
def draw_colorspace(screen, colorspace, grid):      
    pointlist = [
        ( \
            grid.x + (grid.step * x[0] * 10), \
            grid.y + (grid.intervals * grid.step) - (grid.step * x[1] * 10) \
        ) for x in colorspace.primaries \
    ]
    
    pygame.draw.polygon(screen, (100, 100, 255), pointlist, 1)
    
def plot_color_XYZ(screen, point_color, color_xyz, grid):
    if sum(color_xyz) == 0 or numpy.isnan(sum(color_xyz)):
        return

    x_coord = color_xyz[0] / sum(color_xyz)
    y_coord = color_xyz[1] / sum(color_xyz)
    
    pygame.draw.circle(screen, [x * 255 for x in point_color], \
        (int(grid.x + (grid.step * x_coord * 10)), int(grid.y + (grid.intervals * grid.step) - (grid.step * y_coord * 10))), 2)
    
def plot_color(screen, color, colorspace, grid):
    space_to_xyz = compute_XYZ_conversion(colorspace)
    color_XYZ = numpy.matmul(space_to_xyz, numpy.asarray(color))
    plot_color_XYZ(screen, color, color_xyz, grid)
    