import numpy

class xyY(object):
    def __init__(self, x, y, Y):
        self.data = [x, y, Y]
        self.x = x
        self.y = y
        self.Y = Y
        
    def __getitem__(self, key):
        return self.data[key]
        
    def asarray(self):
        return numpy.array([self.x, self.y, self.Y])
        
class XYZ(object):
    def __init__(self, X, Y, Z):
        self.data = [X, Y, Z]
        self.X = X
        self.Y = Y
        self.Z = Z
        
    def __getitem__(self, key):
        return self.data[key]
        
    def asarray(self):
        return numpy.array([self.X, self.Y, self.Z])
        
class ColorSpace(object):
    def __init__(self, primaries, white_point):
        self.primaries = primaries
        self.white_point = white_point
