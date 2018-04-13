import pygame
import numpy
import sys

def convert_ycgco(image, imageList):
	pass
	
	converted = image.copy()
	convertedArray = pygame.surfarray.array3d(converted)
	
	conversionMatrix = numpy.array([ \
		[ 0.25, 0.5, 0.25 ], \
		[ -0.25, 0.5, -0.25 ], \
		[ 0.5, 0, -0.5 ] \
	])

	for x in range(640):
		for y in range(480):
			convertedArray[x,y,:] = conversionMatrix.dot(convertedArray[x,y,:])
			
	yComp = numpy.array(convertedArray)
	yComp[:,:,0] = convertedArray[:,:,0]
	yComp[:,:,1] = convertedArray[:,:,0]
	yComp[:,:,2] = convertedArray[:,:,0]
	
	cgComp = numpy.array(convertedArray)
	cgComp[:,:,1] = convertedArray[:,:,1] + (255.0 / 2.0)
	cgComp[:,:,0] = 0 - cgComp[:,:,1]
	cgComp[:,:,2] = 0 - cgComp[:,:,1]
	
	coComp = numpy.array(convertedArray)
	coComp[:,:,0] = (convertedArray[:,:,2] + (255.0 / 2.0)) * 0.4
	coComp[:,:,1] = (convertedArray[:,:,2] + (255.0 / 2.0)) * 0.2
	coComp[:,:,2] = 0 - cgComp[:,:,0] - cgComp[:,:,1]
	
	images.append(pygame.surfarray.make_surface(convertedArray))
	images.append(pygame.surfarray.make_surface(yComp))
	images.append(pygame.surfarray.make_surface(cgComp))
	images.append(pygame.surfarray.make_surface(coComp))

def convert_ycbcr(image, imageList):
	pass
	
	converted = image.copy()
	convertedArray = pygame.surfarray.array3d(converted)
	
	conversionMatrix = numpy.array([ \
		[ 0.299, 0.587, 0.114 ], \
		[ -0.168736, -0.331264, 0.5 ], \
		[ 0.5, -0.418688, -0.081312 ] \
	])

	for x in range(640):
		for y in range(480):
			convertedArray[x,y,:] = conversionMatrix.dot(convertedArray[x,y,:])
			
	yComp = numpy.array(convertedArray)
	yComp[:,:,0] = convertedArray[:,:,0]
	yComp[:,:,1] = convertedArray[:,:,0]
	yComp[:,:,2] = convertedArray[:,:,0]
	
	cbComp = numpy.array(convertedArray)
	cbComp[:,:,2] = convertedArray[:,:,1] + (255.0 / 2.0)
	cbComp[:,:,0] = 0 - cbComp[:,:,2]
	cbComp[:,:,1] = 0 - cbComp[:,:,2]
	
	crComp = numpy.array(convertedArray)
	crComp[:,:,0] = convertedArray[:,:,2] + (255.0 / 2.0)
	crComp[:,:,1] = 0 - crComp[:,:,0]
	crComp[:,:,2] = 0 - crComp[:,:,0]
	
	images.append(pygame.surfarray.make_surface(convertedArray))
	images.append(pygame.surfarray.make_surface(yComp))
	images.append(pygame.surfarray.make_surface(cbComp))
	images.append(pygame.surfarray.make_surface(crComp))
	
conversionFunctions = \
{ \
	"YCgCo" : convert_ycgco, \
	"YCbCr" : convert_ycbcr, \
}

usage = [__file__, "<image>", "<space (YCgCo | YCbCr)>"]
if not len(sys.argv) == len(usage):
	print("Usage: " + ' '.join(usage))
	sys.exit(1)

pygame.init()

screen = pygame.display.set_mode((640,480))
clock = pygame.time.Clock()

imageName = sys.argv[1]
colorSpace = sys.argv[2]

originalImage = pygame.image.load(imageName)
originalImage = pygame.transform.scale(originalImage, (640, 480))

images = [originalImage]
imageIndex = 0

convFunc = conversionFunctions[colorSpace]
convFunc(originalImage, images)

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
			if event.key == pygame.K_RIGHT:
				imageIndex = (imageIndex + 1) % len(images)
			if event.key == pygame.K_LEFT:
				imageIndex -= 1
				if (imageIndex < 0):
					imageIndex = len(images) - 1
				
	screen.blit(images[imageIndex], (0,0))
    
	pygame.display.update()
