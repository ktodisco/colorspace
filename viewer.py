from constants import *
from drawing import *
import sys
import pygame

if __name__ == "__main__":
    pygame.init()

    screen = pygame.display.set_mode((1280,720))
    pygame.display.set_caption("Color Spaces")
    clock = pygame.time.Clock()

    cie_grid = CIEGrid(50, 50, 400, 9)
    waveform = Waveform(50, 480, 500, 200, 80)

    scale_image_x = int(1920 / 4)
    scale_image_y = int(1200 / 4)

    ref_image = pygame.image.load(sys.argv[1])
    ref_image = pygame.transform.scale(ref_image, (scale_image_x, scale_image_y))
    ref_image_rect = ref_image.get_rect()
    ref_image_rect.topleft = (600, 50)
    
    waveform.analyze(ref_image)

    # Pre-compute all of the xyz coordinates.
    convert_709_to_XYZ = compute_xyz_conversion(COLORSPACE_SRGB)
    xyz_plots = []
    for x in range(scale_image_x):
        for y in range(scale_image_y):
            r, g, b, a = ref_image.get_at((x, y))
            r /= 255.0
            g /= 255.0
            b /= 255.0
            # TODO: Need to handle 0 values.
            r, g, b = numpy.asarray([r, g, b]) / numpy.linalg.norm(numpy.asarray([r, g, b]))
            xyz_plots.append(([r, g, b], numpy.matmul(convert_709_to_XYZ, numpy.asarray([r, g, b]))))

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
        draw_colorspace(screen, COLORSPACE_SRGB, cie_grid)
        draw_colorspace(screen, COLORSPACE_HDR, cie_grid)
        
        for plot in xyz_plots:
             plot_color_XYZ(screen, plot[0], plot[1], cie_grid)
        
        screen.blit(ref_image, ref_image_rect)
        
        waveform.draw(screen)
        
        pygame.display.update()