#!/usr/bin/python
#
# changes
# 2017-07-12, get rid of NumPy (as installation is a hassle), runs with plain Python install
#             needs vec.py and __init__.py to import
#
#import numpy # replaces Numeric
import math

from Tkinter import *
import tkMessageBox
import Tkinter
import csv
#from scene_basic import FlatArc
from vec import Vec3

descr_file = r'..\cfg\lane_projector_scenes\_scene_edit.csv'
scene_file = r"..\cfg\lane_projector_scenes\editor.scene"

#aSegments = ['FlatArc','Clothoid','Snake','Straight','Exit','Roadworks','Zebra','Arrow']
aSegments = ['FlatArc','Clothoid','Straight']
aTrack    = []

top   = Tkinter.Tk()
top.title("virtualRoad Scene Editor (C) 2017")
segtype = StringVar()
s_l1  = StringVar()
s_l2  = StringVar()
s_l3  = StringVar()
s_l4  = StringVar()
s_l5  = StringVar()
s_l6  = StringVar()

s_e1  = StringVar()      # geometry
s_e2  = StringVar()
s_e3  = StringVar()
s_e4  = StringVar()
s_e5  = StringVar()
s_e6  = StringVar()

sOptMarker = StringVar() # marker
s_e7  = StringVar()
s_e8  = StringVar()
s_e9  = StringVar()
s_e10 = StringVar()
s_e11 = StringVar()

iLB2  = -1 # Hack

# ===== canvas =====
canvas_w  = 500
canvas_h  = 500
cnv_scale =   1  # pixel = m/cnv_scale
xInit     = 250
yInit     = 250-80

linewidth = 1

# ===== generation =====
Pinit     = Vec3(0,0,0)   # start (world)
Dinit     = Vec3(1,0,0)   # start
scale     = 1         # increase to "zoom-out" in editor
NumberOfSegments = 50  # <-- shall be even


# TKInter Reference:     http://infohost.nmt.edu/tcc/help/pubs/tkinter/web/index.html
# TkInter Book:          http://effbot.org/tkinterbook/tkinter-application-windows.htm
# how to create lines:   http://infohost.nmt.edu/tcc/help/pubs/tkinter/web/create_line.html
# compile .py to .exe:   http://logix4u.net/component/content/article/27-tutorials/44-how-to-create-windows-executable-exe-from-python-script


#def vector(*v):
#    return numpy.ravel(numpy.array(v, numpy.float32))
#   return array(v)
#   print v
#def v3len(*v):
#    return math.sqrt(v[0][0][0]*v[0][0][0] + v[0][0][1]*v[0][0][1] + v[0][0][2]*v[0][0][2])
#def v3norm(*v):
#    vlen = v3len(*v)
#    return vector(v[0][0][0]*vlen,v[0][0][1]*vlen,v[0][0][2]*vlen)
#def v3scale(*v):
#    vnew = v3norm(v)
#    return vector(vnew[0]*v[1],vnew[1]*v[1],vnew[2]*v[1])


# --------------------------------------------------------------------------------------------------------------->
class Marker:
    def __init__(self, P, Dir, scale, width, striped, name, track):
        self.P = P
        self.Dir = Dir
        self.scale = scale
        self.width = width
        self.striped = striped
        self.name = name
        self.track = 0 # 0 = Left, 1 = Right
    def props(self):
        s_e7.set( '%.2f,%.2f,%.2f' % (self.P.x,self.P.y,self.P.z) )
        s_e8.set( '%.2f,%.2f,%.2f' % (self.Dir.x,self.Dir.y,self.Dir.z) )
        s_e9.set(self.scale)
        s_e10.set(self.width)
        s_e11.set(self.striped)
    def set(self,e7,e8,e9,e10,e11):
        P = e7.split(',')
        self.P = [float(P.x),float(P.y),float(P.z)]
        Dir = e8.split(',')
        self.Dir = [float(Dir.x),float(Dir.y),float(Dir.z)]
        self.scale = float(e9)
        self.width = float(e10)
        self.striped = e11

# Geometric objects have two methods to be implemented
# 1. length(self): return the length of the object in meter
# 2. generate(self, PStart, DirStart, NumberOfSegments, width):
#       Starting from PStart in the direction DirStart produce (res1, res2, P, D) with
#          res1: Sampled version of left side of the object as 3D points (vectors) in the world
#          res2: Sampled version of right side of the object as 3D points (vectors) in the world
#          P:    PStart for the next segment, a guess is (res1 + res2)/2
#          D:    DirStart for the next segment

# geometry elements:
class FlatArc:
    def __init__(self, radius, alpha, direction):
        self.radius = radius
        self.alpha = alpha
        self.direction = direction
        self.name = 'FlatArc'
        # array of markers, init: 1x left, 1x right
        self.aMarker = []
        # self, P_relative, Dir_relative, scale, width, striped, name
        self.aMarker.append( Marker(Vec3(0, 1.75,0),Vec3(1,0,0),0.1,0.15,None,'Rightmarker',1) )
        self.aMarker.append( Marker(Vec3(0,-1.75,0),Vec3(1,0,0),0.1,0.15,None,'Leftmarker',0) )
#        for m in self.aMarker:
#            print m
    def length(self):
        return self.radius*self.alpha
    def generate(self, PStart, DirStart, NumberOfSegments, width): # copied from SceneGenerator.py
#        print NumberOfSegments
        D0 = Vec3(-DirStart.y, DirStart.x, 0) # orthogonal to Dir
        aLeft  = []
        aRight = []
        aLeft.append(PStart + D0*width/2.0)
        aRight.append(PStart - D0*width/2.0)

        m = PStart + D0 * self.radius * self.direction
        alpha0 = math.atan2(-D0.x, -D0.y)
        for i in range(NumberOfSegments):
            a = alpha0 - float(i+1)/float(NumberOfSegments)*self.alpha*self.direction
            D = Vec3(math.sin(a), math.cos(a), 0.0) # z

            a1 = m + D * (self.radius - self.direction * width/2.0) * self.direction
            a2 = m + D * (self.radius + self.direction * width/2.0) * self.direction
            z = 0.0
            # add fancy height (Z) function here!
            a1.z = z
            a2.z = z
            aLeft.append(a1)
            aRight.append(a2)
        P = m + D * self.radius * self.direction
        D = Vec3(-D.y, D.x, 0)
        return (aLeft, aRight, P, D)

    def draw(self,r1,r2,P,Dir,col):
        x = P.x
        y = P.y
        for a in r1:
            xy = xInit+x/cnv_scale,yInit+y/cnv_scale,xInit+a.x/cnv_scale,yInit+a.y/cnv_scale
            if col=='red': lw=3  # Hack!
            else: lw = linewidth # Hack!
            cnv.create_line(xy, fill=col, width="%d" % lw)
            x = a.x
            y = a.y
    def props(self):
        s_l1.set('radius')
        s_l2.set('alpha')
        s_l3.set('direction')
        s_l4.set('-')
        s_l5.set('-')
        s_l6.set('-')
        s_e1.set(self.radius)
        s_e2.set(self.alpha)
        s_e3.set(self.direction)
        s_e4.set('')
        s_e5.set('')
        s_e6.set('')
        for m in self.aMarker:
            if m.name == sOptMarker.get():
                m.props()
    def set(self,e1,e2,e3,e4,e5,e6):
        self.radius=float(e1)
        self.alpha=float(e2)
        self.direction=int(e3)
    def set_marker(self,sOpt,e7,e8,e9,e10,e11):
        for m in self.aMarker:
            if m.name == sOptMarker.get():
                m.set(e7,e8,e9,e10,e11)
    def tostr(self):
        return '%s,%f,%f,%f' % (self.name,self.radius,self.alpha,self.direction)

class Clothoid:
    def __init__(self, c0, c1, l):
        self.c0 = c0
        self.c1 = c1
        self.l = l
        self.name = 'Clothoid'
        # array of markers, init: 1x left, 1x right
        self.aMarker = []
        # self, P, Dir, scale, width, striped, name
        self.aMarker.append( Marker([0, 1.75,0],[1,0,0],0.1,0.15,None,'Rightmarker',1) )
        self.aMarker.append( Marker([0,-1.75,0],[1,0,0],0.1,0.15,None,'Leftmarker',0) )
        for m in self.aMarker:
            print m
    def length(self):
        return self.l
    def generate(self, PStart, DirStart, NumberOfSegments, width): # copied from SceneGenerator.py
        res1 = []
        res2 = []
        P = PStart
        D = DirStart
        l = self.l / NumberOfSegments
        beta = math.atan2(D.y, D.x)
        for i in range(NumberOfSegments):
            c = 1./((i+0.5)/float(NumberOfSegments) * (self.c1 - self.c0) + self.c0)
            alpha = l/c
            t_p = c * math.sin(alpha)
            t_s = (abs(c) - math.sqrt(c*c - t_p*t_p))
            if c < 0: t_s = -t_s
            D_s = Vec3(-D.y, D.x, 0)
            P = P + D * t_p + D_s * t_s
            beta += alpha
            D.x = math.cos(beta)
            D.y = math.sin(beta)

            res1.append(P + D_s * width/2.0)
            res2.append(P - D_s * width/2.0)
        return (res1, res2, P, D)
    def draw(self,r1,r2,P,Dir,col):
        x = P.x
        y = P.y
        for a in r1:
            xy = xInit+x/cnv_scale,yInit+y/cnv_scale,xInit+a.x/cnv_scale,yInit+a.y/cnv_scale
            cnv.create_line(xy, fill=col, width="%d" % linewidth)
            x = a.x
            y = a.y
    def props(self):
        s_l1.set('c0')
        s_l2.set('c1')
        s_l3.set('l')
        s_l4.set('-')
        s_l5.set('-')
        s_l6.set('-')
        s_e1.set(self.c0)
        s_e2.set(self.c1)
        s_e3.set(self.l)
        s_e4.set('')
        s_e5.set('')
        s_e6.set('')
        for m in self.aMarker:
            if m.name == sOptMarker.get():
                m.props()
    def set(self,e1,e2,e3,e4,e5,e6):
        self.c0=float(e1)
        self.c1=float(e2)
        self.l =float(e3)
    def set_marker(self, marker):
        for m in self.aMarker:
            if m.name == sOptMarker.get():
                m.set(e7,e8,e9,e10,e11)
    def tostr(self):
        return '%s,%f,%f,%f' % (self.name,self.c0,self.c1,self.l)

class Snake:
    def __init__(self, father, amplitude, wavelength, debug=0):
        self.father = father
        self.amplitude = amplitude
        self.wavelength = wavelength
        self.debug = debug
        self.name = 'Snake'
    def generate(self, PStart, DirStart, NumberOfSegments, width):
        print 'snake not implemented yet.'
        res1 = []
        res2 = []
        P = PStart
        D = DirStart
        return (res1, res2, P, D)
    def draw(self,r1,r2,P,Dir,col):
        pass
    def props(self):
        s_l1.set('father')
        s_l2.set('amplitude')
        s_l3.set('wavelength')
        s_l4.set('debug')
        s_l5.set('-')
        s_l6.set('-')
        s_e1.set(self.father)
        s_e2.set(self.amplitude)
        s_e3.set(self.wavelength)
        s_e4.set(self.debug)
        s_e5.set('')
        s_e6.set('')
    def set(self,e1,e2,e3,e4,e5,e6):
        self.father = e1
        self.amplitude=e2
        self.wavelength=e3
        self.debug=e4
    def set_marker(self, marker):
        self.marker = marker
    def tostr(self):
        return '%s,%f,%f,%f,%f' % (self.name,self.father,self.amplitude,self.wavelength,self.debug)

class Straight:
    def __init__(self, l):
        self.l = l
        self.name = 'Straight'
        # array of markers, init: 1x left, 1x right
        self.aMarker = []
        # self, P, Dir, scale, width, striped, name
        self.aMarker.append( Marker(Vec3(0, 1.75,0),Vec3(1,0,0),0.1,0.15,None,'Rightmarker',1) )
        self.aMarker.append( Marker(Vec3(0,-1.75,0),Vec3(1,0,0),0.1,0.15,None,'Leftmarker',0) )
        for m in self.aMarker:
            print m
    def length(self):
        return self.l
    def generate(self, PStart, DirStart, NumberOfSegments, width): # copied from SceneGenerator.py
        print
        res1 = []
        res2 = []
        D0 = Vec3(-DirStart.y, DirStart.x, 0) * width/2.0
        for i in range(NumberOfSegments):
            l = float(self.l)
#            print "(1) l"
#            print l
#            print "(2)NumberOfSegments"
#            print NumberOfSegments
            fSeg =  i/float(NumberOfSegments)
#            print "(3)fSeg"
#            print fSeg
            fL = l * fSeg
#            print "(4)fL"
#            print fL
#            x = fL*DirStart <-- so wird "*" auf den Float angewendet
            x = DirStart*fL # <-- so wird die Vec3 "__mult__" aufgerufen
#            print "(5)x"
#            print x
            res1.append(PStart + x + D0)
            res2.append(PStart + x - D0)
        return (res1, res2, PStart + DirStart*self.l, DirStart)
    def draw(self,r1,r2,P,Dir,col):
        x = P.x
        y = P.y
        for a in r1:
            xy = xInit+x/cnv_scale,yInit+y/cnv_scale,xInit+a.x/cnv_scale,yInit+a.y/cnv_scale
            cnv.create_line(xy, fill=col, width="%d" % linewidth)
            x = a.x
            y = a.y
    def props(self):
        s_l1.set('l')
        s_l2.set('-')
        s_l3.set('-')
        s_l4.set('-')
        s_l5.set('-')
        s_l6.set('-')
        s_e1.set(self.l)
        s_e2.set('')
        s_e3.set('')
        s_e4.set('')
        s_e5.set('')
        s_e6.set('')
        for m in self.aMarker:
            if m.name == sOptMarker.get():
                m.props()
    def set(self,e1,e2,e3,e4,e5,e6):
        self.l = float(e1)
    def set_marker(self, marker):
        for m in self.aMarker:
            if m.name == sOptMarker.get():
                m.set(e7,e8,e9,e10,e11)
    def tostr(self):
        print self.name
        print self.l
        return '%s,%f' % (self.name,float(self.l))

class Zebra:
    def __init__(self, l):
        self.l = l
        self.name = 'Zebra'
        # array of markers, init: 1x left, 1x right
        self.aMarker = []
        # self, P, Dir, scale, width, striped, name
        self.aMarker.append( Marker([0, 1.75,0],[1,0,0],0.1,0.15,None,'Zebra',1) ) # currently in use?
        for m in self.aMarker:
            print m
    def length(self):
        return self.l
    def generate(self, PStart, DirStart, NumberOfSegments, width): # copied from SceneGenerator.py
        res1 = []
        res2 = []
        D0 = Vec3(-DirStart.y, DirStart.x, 0) * width/2.0
        NumberOfSegments = 2
        for i in range(NumberOfSegments):
            res1.append(PStart + (i+1)/float(NumberOfSegments)*DirStart*self.l + D0)
            res2.append(PStart + (i+1)/float(NumberOfSegments)*DirStart*self.l - D0)
        return (res1, res2, PStart + DirStart*self.l, DirStart)
    def draw(self,r1,r2,P,Dir,col):
        x = P.x
        y = P.y
        for a in r1:
            xy = xInit+x/cnv_scale,yInit+y/cnv_scale,xInit+a.x/cnv_scale,yInit+a.y/cnv_scale
            cnv.create_line(xy, fill=col, width="%d" % linewidth)
            x = a.x
            y = a.y
    def props(self):
        s_l1.set('l')
        s_l2.set('-')
        s_l3.set('-')
        s_l4.set('-')
        s_l5.set('-')
        s_l6.set('-')
        s_e1.set(self.l)
        s_e2.set('')
        s_e3.set('')
        s_e4.set('')
        s_e5.set('')
        s_e6.set('')
        for m in self.aMarker:
            if m.name == sOptMarker.get():
                m.props()
    def set(self,e1,e2,e3,e4,e5,e6):
        self.l = float(e1)
    def set_marker(self, marker):
        for m in self.aMarker:
            if m.name == sOptMarker.get():
                m.set(e7,e8,e9,e10,e11)
    def tostr(self):
        print self.name
        print self.l
        return '%s,%f' % (self.name,float(self.l))

class SixDofGenerator:
    def __init__(self, speed_fun=lambda x: 30.0, nick_fun=lambda x: 0.0, roll_fun=lambda x: 0.0):
        self.speed_fun = speed_fun
        self.generate = self.generate_Nick
        self.nick_fun = nick_fun
        self.roll_fun = roll_fun
        pass

    def generate_Nick(self, p0, p1, p2, l):
        d = p2-p0
        speed = self.speed_fun(l)
        nick = self.nick_fun(l)
        roll = self.roll_fun(l)
        return [roll, nick, math.atan2(d.y, d.x), speed]

def CreateTrajectory(P, Dir, scale, objects, sixdof, name, stream = sys.stdout):
    res = []
    L = 0.0
    P = vector(*P)
    Dir = vector(*Dir)
    res = []
    resTmp = []
    print "scale=%d" % scale
    for o in objects:
        (r1, r2, P, Dir) = o.generate(P, Dir, int(round(o.length()/scale)),0.0)
        res.extend(r1)
    res[0] = res[0].tolist()
    res[0].extend(sixdof.generate(res[0], res[0], res[1], scale))
    for i in range(1,len(res)-1):
        res[i] = res[i].tolist()
        resTmp = [res[i-1][j] for j in range(3)]
        res[i].extend(sixdof.generate(resTmp, res[i], res[i+1], i*scale))
        #print res[i]
    res[-1] = res[-1].tolist()
    res[-1].extend(res[-2][3:])
    print >>stream, "static S_6Dof %s[] = {" % (name)
    for i in range(len(res)):
        print >>stream, '{',
        for j in range(6):
            print >>stream, res[i][j], ',',
        print >>stream, res[i][6], " }",
        if i != len(res)-1: print >>stream, ","
        else: print >>stream
    print >>stream, "}"
    return res

# <---------------------------------------------------------------------------------------------------------------

def geometry_properties(top):
    global l,segsel,p1,s_l1,e1,e2,e3,e4,e5,e6

    p1 = PanedWindow(orient=VERTICAL)
    caption = Label(p1,text='Geometry')
    p1.add(caption)

    l = Label(p1,textvariable=segtype)
    p1.add(l)

    l1 = Label(p1,textvariable=s_l1)
    p1.add(l1)
    e1 = Entry(p1,textvariable=s_e1)
    p1.add(e1)

    l2 = Label(p1,textvariable=s_l2)
    p1.add(l2)
    e2 = Entry(p1,textvariable=s_e2)
    p1.add(e2)

    l3 = Label(p1,textvariable=s_l3)
    p1.add(l3)
    e3 = Entry(p1,textvariable=s_e3)
    p1.add(e3)

    l4 = Label(p1,textvariable=s_l4)
    p1.add(l4)
    e4 = Entry(p1,textvariable=s_e4)
    p1.add(e4)

    l5 = Label(p1,textvariable=s_l5)
    p1.add(l5)
    e5 = Entry(p1,textvariable=s_e5)
    p1.add(e5)

    l6 = Label(p1,textvariable=s_l6)
    p1.add(l6)
    e6 = Entry(p1,textvariable=s_e6)
    p1.add(e6)

    b = Button(p1, text="set", width=10, command=enter_geometry)
    p1.add(b)

    p1.grid(row=0,column=3,rowspan=2,padx=4,sticky=N) # http://effbot.org/tkinterbook/grid.htm

def marker_properties(top):
# def CreateLaneMarker( P, Dir, scale, width, striped, objects, name, stream=sys.stdout, color=[1450, 1700, 1200] ):
# Bsp. CreateLaneMarker([0,LaneWidth/2,0], [1,0,0], 0.1, 0.15, None, skel, "RightMarker", f)
    global opt,e7,e8,e9,e10,e11

    p2 = PanedWindow(orient=VERTICAL)
    caption = Label(p2,text='Marker')
    p2.add(caption)

    opt = OptionMenu(p2, sOptMarker, 'Rightmarker','Leftmarker','< Add >','< Copy from ... >','< Copy to ... >', command=marker_click)
    p2.add(opt)

    l7 = Label(p2,text='Pos (x,y,z)')
    p2.add(l7)
    e7 = Entry(p2,textvariable=s_e7)
    p2.add(e7)

    l8 = Label(p2,text='Dir (x,y,z)')
    p2.add(l8)
    e8 = Entry(p2,textvariable=s_e8)
    p2.add(e8)

    l9 = Label(p2,text='scale [0..1]')
    p2.add(l9)
    e9 = Entry(p2,textvariable=s_e9)
    p2.add(e9)

    l10 = Label(p2,text='width')
    p2.add(l10)
    e10 = Entry(p2,textvariable=s_e10)
    p2.add(e10)

    l11 = Label(p2,text='striped')
    p2.add(l11)
    e11 = Entry(p2,textvariable=s_e11)
    p2.add(e11)

    b2 = Button(p2, text="set", width=10, command=enter_marker)
    p2.add(b2)

    p2.grid(row=0,column=4,rowspan=2,padx=4,sticky=N)

def drawTrack(iSel):
    print aTrack
    cnv.create_rectangle(0, 0, canvas_w, canvas_h, fill="white")
    r1 = []
    r2 = []
    P = Vec3(*Pinit)
    D = Vec3(*Dinit)
    i = 0
    for seg in aTrack:
        i+=1
        (r1,r2,Pnew,Dnew) = seg.generate(P,D,NumberOfSegments, 0.0)#width)
        if i==iSel:
            seg.draw(r1,r2,P,D,'red')
        else:
            seg.draw(r1,r2,P,D,'black')
        P = Pnew
        D = Dnew

def lb_click(event): # add segment to track
    i = int(lb.curselection()[0])
    lb2.insert(END, aSegments[i] )
    if aSegments[i] == 'FlatArc':
        seg = FlatArc(80.0,math.radians(90.0),1) # 1 (clockwise) or -1 (ccw)
    if aSegments[i] == 'Clothoid':
        seg = Clothoid(1.0/150,1.0/1000,80)
    if aSegments[i] == 'Snake':
        seg = Snake(1,2,3,4)
    if aSegments[i] == 'Straight':
        seg = Straight(100.0)
    if aSegments[i] == 'Zebra':
        seg = Zebra(5.0)
    aTrack.append(seg)
    drawTrack(-1)

def lb2_click(event): # select segment within track
    global iLB2
    iLB2 = int(lb2.curselection()[0])
    seg = aTrack[iLB2]
    seg.props()
    drawTrack(iLB2+1) # highlight selected segement! (can be commented out)

def marker_click(event):
    seg = aTrack[iLB2]
    for m in seg.aMarker:
        if m.name == sOptMarker.get():
            m.props()

def enter_geometry(): # edit
    i = iLB2 # <-- Hack!! das Listview behaelt nicht die Auswahl bei Focus-verlust
    seg = aTrack[i]
    seg.set(e1.get(),e2.get(),e3.get(),e4.get(),e5.get(),e6.get())
    drawTrack(i)

def enter_marker(): # edit
    i = iLB2 # <-- Hack!! das Listview behaelt nicht die Auswahl bei Focus-verlust
    seg = aTrack[i]
    seg.set_marker(sOptMarker.get(),e7.get(),e8.get(),e9.get(),e10.get(),e11.get())
    drawTrack(i)

def load():
    global aTrack
    aTrack = []
    with open(descr_file, 'rb') as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        for row in reader:
            if row[0] == 'FlatArc':
                seg =  FlatArc(float(row[1]),float(row[2]),int(float(row[3])))
            if row[0] == 'Clothoid':
                seg = Clothoid(float(row[1]),float(row[2]),    float(row[3]))
            if row[0] == 'Snake':
                seg =    Snake(float(row[1]),float(row[2]),    float(row[3]), float(row[4]))
            if row[0] == 'Straight':
                seg = Straight(float(row[1]))
            if row[0] == 'Zebra':
                seg =    Zebra(float(row[1]))
            aTrack.append(seg)
            lb2.insert(END, seg.name)
    drawTrack(0)

def save():
    f = open(descr_file, "w")
    for seg in aTrack:
        print >>f, seg.tostr()

def export():
# ---------------------------------------------------------------------------------------------------------
# S_6Dof:        s_Pos.rl_X, s_Pos.rl_Y, s_Pos.rl_Z, rl_Roll, rl_Pitch, rl_Yaw, rl_Speed
# S_MarkerPoint: s_Left.rl_X, s_Left.rl_Y, s_Left.rl_Z, s_Right.rl_X, s_Right.rl_Y, s_Right.rl_Z, b_Visible
# ---------------------------------------------------------------------------------------------------------
    if len(aTrack)==0: return
    f = open(scene_file, "w")
    bVisible = 1
    fSpeed   = 30.0
    a6DOF    = []

    aMarkers = [] # 1..x(7) aMarker
    aMarkers.append([]) # Hack!! left
    aMarkers.append([]) # Hack!! right

    aRoad = [] # NEU !!

#     road
#      +-------------------------------+
#      |                               |
#      |      markerL     markerR      |
#      |          ++---*---++          |
#      |          ||   |   ||          |
#      |          ||   |   ||          |
#      |          ++---*---++          |
#      |              P(init)          |
#      |                               |
#      +-------------------------------+

#      +-------------------------------+
#      |                               |
#      |      markerL                  |
#      |          ++---*---++          |
#      |     v1L  ||  v1R              |
#      |          ||                   |
#
#

    P = Vec3(*Pinit)
    D = Vec3(*Dinit)
    for seg in aTrack:
        aL = [] # hier steht das Skeleton (Zermtrum) des linken Markers
        aR = []
        (aL,aR,Pnew,Dnew) = seg.generate(P,D,NumberOfSegments,2.25) # NumberOfSegments gives resolution
#        print r1                                                   # --> e.g. for 8 Segments output is 9 points !!
#        print r2
# in Pnew steht das End-Element des Tracks
#        x1 = r1[0][0] # segment-start Left
#        y1 = r1[0][1]
#        z1 = r1[0][2]
#        x2 = r2[0][0] # segment-start right
#        y2 = r2[0][1]
#        z2 = r2[0][2]
# -------------------------------------------
# hier werden keine Segmente exportiert,
# sondern die Begrenzungspunkte der Segmente,
# also m?ssen 9 Punkte geschrieben werden !!!
# -------------------------------------------
        for j in range(len(aL)): # assume: r1 and r2 hold the same number of steps
#            print j
            PSkel_L = aL[j] # Point L of Marker Skeleton
            PSkel_R = aR[j]
#            vLR = PSkel_R-PSkel_L
            vLR = PSkel_L-PSkel_R # <--- Fix !!!

#            vd = vector(x1-x2,y1-y2,z1-z2)
            v1L =  vLR*0.025
            v1R = -vLR*0.025

#            x1next = r1[j][0] # segment-next <-- beim letzten Element des Tracks auf das erste Elem. des neuen Tracks zeigen
#            y1next = r1[j][1]
#            z1next = r1[j][2]

#            x2next = r2[j][0] # segment-next
#            y2next = r2[j][1]
#            z2next = r2[j][2]


            zADD = 0.02 # 2do: if smaller 0.1 markers disappear (they don't fit exactly with Road segments)
#            vdnext = vector(x1next-x2next,y1next-y2next,z1next-z2next)

# ===========================
#  Trajectory not centered!!
# ===========================
            if (j<(len(aL)-1)): # Hack!
                PTraj = PSkel_L+vLR*0.5
                s6DOF    = '{ %.3f , %.3f , %.3f , %.3f , %.3f , %.3f , %.3f } ,' % (PTraj.x,PTraj.y,PTraj.z, PTraj.x,PTraj.y,PTraj.z, fSpeed) # Hack!
                a6DOF.append(s6DOF) # Hack!

                # Marker LEFT (ML)   MLleft,MLright
                PLL = PSkel_L+v1L
                PLR = PSkel_L+v1R
                sMarkerStep = '{ %.3f , %.3f , %.3f , %.3f , %.3f , %.3f , %d } ,' % (PLL.x,PLL.y,PLL.z+zADD, PLR.x,PLR.y,PLR.z+zADD, bVisible)
                aMarkers[0].append(sMarkerStep)

                # Marker RIGHT (MR)   MRleft,MRright
                PRL = PSkel_R+v1L
                PRR = PSkel_R+v1R
                sMarkerStep = '{ %.3f , %.3f , %.3f , %.3f , %.3f , %.3f , %d } ,' % (PRL.x,PRL.y,PRL.z+zADD, PRR.x,PRR.y,PRR.z+zADD, bVisible)
                aMarkers[1].append(sMarkerStep)

                # Road (relative to markers!)
                v1L =  vLR*0.75
                v1R = -vLR*0.75
                PRoadL = PSkel_L+v1L
                PRoadR = PSkel_R+v1R
                sRoadStep = '{ %.3f , %.3f , %.3f , %.3f , %.3f , %.3f , %d } ,' % (PRoadL.x,PRoadL.y,PRoadL.z, PRoadR.x,PRoadR.y,PRoadR.z, bVisible)
                aRoad.append(sRoadStep)

#            x1 = x1next # segment-next
#            y1 = y1next
#            z1 = z1next
#            x2 = x2next # segment-next
#            y2 = y2next
#            z2 = z2next

        P = Pnew
        D = Dnew

    # =================== Hack!!! an das letzte Element eine Traj anf?gen
    PTraj = PSkel_L+vLR*0.5
    s6DOF    = '{ %.3f , %.3f , %.3f , %.3f , %.3f , %.3f , %.3f } ,' % (PTraj.x,PTraj.y,PTraj.z, PTraj.x,PTraj.y,PTraj.z, fSpeed) # Hack!
    a6DOF.append(s6DOF) # Hack!

    # Marker LEFT (ML)   MLleft,MLright
    PLL = PSkel_L+v1L
    PLR = PSkel_L+v1R
    sMarkerStep = '{ %.3f , %.3f , %.3f , %.3f , %.3f , %.3f , %d } ,' % (PLL.x,PLL.y,PLL.z+zADD, PLR.x,PLR.y,PLR.z+zADD, bVisible)
    aMarkers[0].append(sMarkerStep)

    # Marker RIGHT (MR)   MRleft,MRright
    PRL = PSkel_R+v1L
    PRR = PSkel_R+v1R
    sMarkerStep = '{ %.3f , %.3f , %.3f , %.3f , %.3f , %.3f , %d } ,' % (PRL.x,PRL.y,PRL.z+zADD, PRR.x,PRR.y,PRR.z+zADD, bVisible)
    aMarkers[1].append(sMarkerStep)

    # Road (relative to markers!)
    v1L =  vLR*0.75
    v1R = -vLR*0.75
    PRoadL = PSkel_L+v1L
    PRoadR = PSkel_R+v1R
    sRoadStep = '{ %.3f , %.3f , %.3f , %.3f , %.3f , %.3f , %d } ,' % (PRoadL.x,PRoadL.y,PRoadL.z, PRoadR.x,PRoadR.y,PRoadR.z, bVisible)
    aRoad.append(sRoadStep)
    # =================== Hack !!!!

    print >>f, 'static S_6Dof Trajectory[] = {'
    for s in a6DOF: print >>f, s
    print >>f, '}'

    a = ('static unsigned char RightMarker_Color = 4095 4095 4095;', 'static unsigned char LeftMarker_Color = 4095 4095 0;','static unsigned char trajectory_Color = 4095 0 4095;')
    aMNames  = ['RightMarker','LeftMarker','trajectory'] # Hack!!
    i=0
    for aM in aMarkers:
        print >>f, ('static S_MarkerPoint %s[] = {' % aMNames[i])
        for s in aM:
            print >>f, s
        print >>f, '}'
        print >>f, a[i] # color
        i+=1

    a = 'static unsigned char Road_Color = 250 250 250;'
    print >>f, ('static S_MarkerPoint %s[] = {' % 'Road')
    for aR in aRoad:
        print >>f, aR
    print >>f, '}'
    print >>f, a # color

    print "Export done."

if __name__ == "__main__":
    menubar = Menu(top)
    menubar.add_command(label="Load",  command=load)
    menubar.add_command(label="Save",  command=save)
    menubar.add_command(label="Export",command=export)
    menubar.add_command(label="Exit",  command=top.quit)
    # display the menu
    top.config(menu=menubar)

    scrollbar = Tkinter.Scrollbar(top, orient="vertical")

    top.columnconfigure(1, weight=1) # http://www.velocityreviews.com/forums/t869039-tkinter-why-arent-my-widgets-expanding-when-i-resize-the-window.html
    top.rowconfigure(1, weight=1)  # column 1 will resize

    # 1a) basic markers
    lb = Listbox(top, width=20, height=10, yscrollcommand=scrollbar.set)
    lb.grid(row=0,column=0,padx=4)

    scrollbar.config(command=lb.yview)

    #1b) track list
    lb2 = Listbox(top, width=20, height=14, yscrollcommand=scrollbar.set)
    lb2.grid(row=1,column=0,padx=4,sticky=N)

    #2) track image
    cnv = Canvas(top, width=canvas_w, height=canvas_h)
    cnv.create_rectangle(0, 0, canvas_w, canvas_h, fill="white")
    cnv.grid(row=0,column=1,rowspan=2,padx=4)#,sticky=N+S+E+W)

    #3) geometry properties
    geometry_properties(top)

    #4) marker properties
    marker_properties(top)

    i = 0
    for seg in aSegments:
        lb.insert(i, seg )
        i+=1
    lb.bind("<ButtonRelease-1>", lb_click)
    lb2.bind("<ButtonRelease-1>", lb2_click)

    top.mainloop()
