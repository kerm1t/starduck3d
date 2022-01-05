# https://sites.google.com/site/dlampetest/python/vector-3d
import math

class Vec3:
    ''' A three dimensional vector '''
    def __init__(self, v_x=0, v_y=0, v_z=0):
        self.set( v_x, v_y, v_z )
    def set(self, v_x=0, v_y=0, v_z=0):
        if isinstance(v_x, tuple) or isinstance(v_x, list):
            self.x, self.y, self.z = v_x
        else:
            self.x = v_x
            self.y = v_y
            self.z = v_z
    def __getitem__(self, index):
        if index==0: return self.x
        elif index==1: return self.y
        elif index==2: return self.z
        else: raise IndexError("index out of range for Vec3")
    def __mul__(self, other): 
        '''Multiplication, supports types Vec3 and other 
        types that supports the * operator '''
        if isinstance(other, Vec3):
            return Vec3(self.x*other.x, self.y*other.y, self.z*other.z)
        else: #Raise an exception if not a float or integer
            return Vec3(self.x*other, self.y*other, self.z*other)
# python2    def __div__(self, other):
    def __truediv__(self, other):
        '''Division, supports types Vec3 and other 
        types that supports the / operator '''
        if isinstance(other, Vec3):
            return Vec3(self.x/other.x, self.y/other.y, self.z/other.z)
        else: #Raise an exception if not a float or integer
            return Vec3(self.x/other, self.y/other, self.z/other)
    def __add__(self, other):
        '''Addition, supports types Vec3 and other 
        types that supports the + operator '''        
        if isinstance(other, Vec3):
            return Vec3( self.x + other.x, self.y + other.y, self.z + other.z )
        else: #Raise an exception if not a float or integer
            return Vec3(self.x + other, self.y + other, self.z + other)
    def __sub__(self, other):
        '''Subtraction, supports types Vec3 and other 
        types that supports the - operator '''        
        if isinstance(other, Vec3):
            return Vec3(self.x - other.x, self.y - other.y, self.z - other.z)
        else: #Raise an exception if not a float or integer
            return Vec3(self.x - other, self.y - other, self.z - other )
    def __abs__(self):
        '''Absolute value: the abs() method '''        
        return Vec3( abs(self.x), abs(self.y), abs(self.z) )
    def __neg__(self):
        '''Negate this vector''' 
        return Vec3( -self.x, -self.y, -self.z )
    def __str__(self):
        return '<' +  ','.join( 
               [str(val) for val in (self.x, self.y, self.z) ] ) + '>'
    def __repr__(self):
        return str(self) + ' instance at 0x' + str(hex(id(self))[2:].upper())    
    def length(self):
        ''' This vectors length'''        
        return math.sqrt( self.x**2 + self.y**2 + self.z**2 )
    def length_squared(self):
        ''' Returns this vectors length squared
        (saves a sqrt call, usefull for vector comparisons)'''
        return self.x**2 + self.y**2 + self.z**2
    def cross(self, other):
        '''Return the cross product of this and another Vec3'''        
        return Vec3( self.y*other.z - other.y*self.z,  
                     self.z*other.x - other.z*self.x,  
                     self.x*other.y - self.y*other.x )
    def dot(self, other):
        '''Return the dot product of this and another Vec3'''
        return self.x*other.x + self.y*other.y + self.z*other.z
    def normalized(self):
        '''Return this vector normalized'''        
        return self/self.length()
    def normalize(self):
        '''Normalize this Vec3'''        
        self /= self.length()
