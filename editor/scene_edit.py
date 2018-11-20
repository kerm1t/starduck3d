#!/usr/bin/python
#
# changes
# 2018-11-18, resizeable canvas, texture
# 2018-11-18, buggyboy-flavor: store road as segments, do not store markers anymore
# 2017-07-12, get rid of NumPy (as installation is a hassle), runs with plain Python install
#             needs vec.py and __init__.py to import
#
import math

from Tkinter import *
import tkMessageBox
import Tkinter
import csv
#from scene_basic import FlatArc
from vec import Vec3

# https://stackoverflow.com/questions/22835289/how-to-get-tkinter-canvas-to-dynamically-resize-to-window-width
class ResizingCanvas(Canvas):
    def __init__(self,parent,**kwargs):
        Canvas.__init__(self,parent,**kwargs)
        self.bind("<Configure>", self.on_resize)
        self.height = self.winfo_reqheight()
        self.width = self.winfo_reqwidth()

    def on_resize(self,event):
        # determine the ratio of old width/height to new width/height
        wscale = float(event.width)/self.width
        hscale = float(event.height)/self.height
        self.width = event.width
        self.height = event.height
        # resize the canvas 
        self.config(width=self.width, height=self.height)
        # rescale all the objects tagged with the "bg" tag --> or: "all"
        self.scale("bg",0,0,wscale,hscale)

descr_file = r'.\_scene_edit.csv'
scene_file = r".\editor.scene"


aSegments = ['FlatArc','Clothoid','Straight']
aTex      = ["texRoad", "texWater"]
TEXdef    = "texRoad"


aTrack    = []


root   = Tkinter.Tk()
root.title("Buggyboy Scene Editor (C) 2018")
segtype = StringVar()
s_l1  = StringVar()      # Labels
s_l2  = StringVar()
s_l3  = StringVar()
s_l4  = StringVar()
s_l5  = StringVar()
s_l6  = StringVar()

s_e1  = StringVar()      # edit-field
s_e2  = StringVar()
s_e3  = StringVar()
s_e4  = StringVar()
s_e5  = StringVar()
s_e6  = StringVar()

s_texture = StringVar(root)

sOptMarker = StringVar() # Marker --> texture
s_e7  = StringVar()
s_e8  = StringVar()
s_e9  = StringVar()
s_e10 = StringVar()
s_e11 = StringVar()

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


# --------------------------------------------------------------------------------------------------------------->

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
        self.texture = TEXdef # texRoad, texWater, ...
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
#            xy = xInit+x/cnv.wscale,yInit+y/cnv.hscale,xInit+a.x/cnv.wscale,yInit+a.y/cnv.hscale
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
        s_texture.set(self.texture)
    def set(self,e1,e2,e3,e4,e5,e6):
        self.radius=float(e1)
        self.alpha=float(e2)
        self.direction=int(e3)
    def set_texture(self,texture):
        self.texture = texture
    def tostr(self):
        return '%s,%f,%f,%f' % (self.name,self.radius,self.alpha,self.direction)

class Clothoid:
    def __init__(self, c0, c1, l):
        self.c0 = c0
        self.c1 = c1
        self.l = l
        self.name = 'Clothoid'
        self.texture = TEXdef # texRoad, texWater, ...
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
            if col=='red': lw=3  # Hack!
            else: lw = linewidth # Hack!
            cnv.create_line(xy, fill=col, width="%d" % lw)
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
        s_texture.set(self.texture)
        for m in self.aMarker:
            if m.name == sOptMarker.get():
                m.props()
    def set(self,e1,e2,e3,e4,e5,e6):
        self.c0=float(e1)
        self.c1=float(e2)
        self.l =float(e3)
    def set_texture(self,texture):
        self.texture = texture
    def tostr(self):
        return '%s,%f,%f,%f' % (self.name,self.c0,self.c1,self.l)

class Snake:
    def __init__(self, father, amplitude, wavelength, debug=0):
        self.father = father
        self.amplitude = amplitude
        self.wavelength = wavelength
        self.debug = debug
        self.name = 'Snake'
#        self.texture = TEXdef # texRoad, texWater, ...
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
    def set_texture(self,texture):
        self.texture = texture
    def tostr(self):
        return '%s,%f,%f,%f,%f' % (self.name,self.father,self.amplitude,self.wavelength,self.debug)

class Straight:
    def __init__(self, l):
        self.l = l
        self.name = 'Straight'
        self.texture = TEXdef # texRoad, texWater, ...
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
            if col=='red': lw=3  # Hack!
            else: lw = linewidth # Hack!
            cnv.create_line(xy, fill=col, width="%d" % lw)
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
        s_texture.set(self.texture)
    def set(self,e1,e2,e3,e4,e5,e6):
        self.l = float(e1)
    def set_texture(self,texture):
        self.texture = texture
    def tostr(self):
        print self.name
        print self.l
        return '%s,%f' % (self.name,float(self.l))

class Zebra:
    def __init__(self, l):
        self.l = l
        self.name = 'Zebra'
        self.texture = 'texRoad' # texWater, ...
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
        s_texture.set(self.texture)
    def set(self,e1,e2,e3,e4,e5,e6):
        self.l = float(e1)
    def set_texture(self,texture):
        self.texture = texture
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

def geometry_properties(root):
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

    p1.pack(side=RIGHT)

def marker_properties(root):
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

    p2.pack(side=RIGHT)

def drawTrack(iSel):
    print aTrack
    cnv.create_rectangle(0, 0, cnv.width, cnv.height, fill="white", tags="bg")
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

def btnAppend(): # add segment to track
    i = int(lb.curselection()[0])
    lb2.insert(END, "%s (%s)" % (aSegments[i], TEXdef) )
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

def btnInsert(): # insert segment into track
    i = int(lb.curselection()[0])
    j = int(lb2.curselection()[0])
#    print "insert %d at %d" % i,j
    lb2.insert(j, "%s (%s)" % (aSegments[i], TEXdef) )
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
    aTrack.insert(j,seg)
    drawTrack(-1)

def lb2_click(event): # select segment within track
    i = int(lb2.curselection()[0])
    seg = aTrack[i]
    seg.props()
    drawTrack(i+1) # highlight selected segement! (can be commented out)

def marker_click(event):
    i = int(lb2.curselection()[0])
    seg = aTrack[i]
    for m in seg.aMarker:
        if m.name == sOptMarker.get():
            m.props()

def enter_geometry(): # edit
    i = int(lb2.curselection()[0])
    seg = aTrack[i]
    seg.set(e1.get(),e2.get(),e3.get(),e4.get(),e5.get(),e6.get())
    drawTrack(i)

def enter_marker(): # edit
    i = int(lb2.curselection()[0])
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

# ===========================
#  a) Trajectory (enemy autodrive help / driving limitations?)
# ===========================
    fSpeed   = 30.0
    a6DOF    = []

    P = Vec3(*Pinit) # position
    D = Vec3(*Dinit) # direction
    for seg in aTrack:
        aL = [] # hier steht das Skeleton (Zermtrum) des linken Markers
        aR = []
        (aL,aR,Pnew,Dnew) = seg.generate(P,D,NumberOfSegments,2.25) # NumberOfSegments gives resolution

        for j in range(len(aL)): # assume: r1 and r2 hold the same number of steps
            PSkel_L = aL[j] # Point L of Marker Skeleton
            PSkel_R = aR[j]
#            vLR = PSkel_R-PSkel_L
            vLR = PSkel_L-PSkel_R # <--- Fix !!!

            v1L =  vLR*0.025
            v1R = -vLR*0.025

# ===========================
#  Trajectory not centered!!
# ===========================
            if (j<(len(aL)-1)): # Hack!
                PTraj = PSkel_L+vLR*0.5
                s6DOF    = '{ %.3f , %.3f , %.3f , %.3f , %.3f , %.3f , %.3f } ,' % (PTraj.x,PTraj.y,PTraj.z, PTraj.x,PTraj.y,PTraj.z, fSpeed) # Hack!
                a6DOF.append(s6DOF) # Hack!

        P = Pnew
        D = Dnew

    # =================== Hack!!! an das letzte Element eine Traj anfuegen
    PTraj = PSkel_L+vLR*0.5
    s6DOF    = '{ %.3f , %.3f , %.3f , %.3f , %.3f , %.3f , %.3f } ,' % (PTraj.x,PTraj.y,PTraj.z, PTraj.x,PTraj.y,PTraj.z, fSpeed) # Hack!
    a6DOF.append(s6DOF) # Hack!

    print >>f, 'static S_6Dof Trajectory[] = {'
    for s in a6DOF: print >>f, s
    print >>f, '}'



# ===========================
#  b) Road
# ===========================
    P = Vec3(*Pinit) # position
    D = Vec3(*Dinit) # direction
    for seg in aTrack:
        aL = [] # hier steht das Skeleton (Zermtrum) des linken Markers
        aR = []
        (aL,aR,Pnew,Dnew) = seg.generate(P,D,NumberOfSegments,2.25) # NumberOfSegments gives resolution
        
        aRoadSeg = [] # NEU !!
        for j in range(len(aL)): # assume: r1 and r2 hold the same number of steps
            PSkel_L = aL[j] # Point L of Marker Skeleton
            PSkel_R = aR[j]
#            vLR = PSkel_R-PSkel_L
            vLR = PSkel_L-PSkel_R # <--- Fix !!!

            v1L =  vLR*0.025
            v1R = -vLR*0.025

# ===========================
#  Trajectory not centered!!
# ===========================
            if (j<(len(aL)-1)): # Hack!

                # Road (relative to markers!)
                v1L =  vLR*0.75
                v1R = -vLR*0.75
                PRoadL = PSkel_L+v1L
                PRoadR = PSkel_R+v1R
                sRoadStep = '{ %.3f , %.3f , %.3f , %.3f , %.3f , %.3f , %d } ,' % (PRoadL.x,PRoadL.y,PRoadL.z, PRoadR.x,PRoadR.y,PRoadR.z, bVisible)
                aRoadSeg.append(sRoadStep)

        P = Pnew
        D = Dnew
#        a = 'static unsigned char Road_Color = 250 250 250;'
        a = 'static string Texture = %s;' % seg.texture
        print >>f, ('static S_MarkerPoint %s[] = {' % 'Road')
        for aR in aRoadSeg:
            print >>f, aR
        print >>f, '}'
        print >>f, a # color


    # Road (relative to markers!)
    v1L =  vLR*0.75
    v1R = -vLR*0.75
    PRoadL = PSkel_L+v1L
    PRoadR = PSkel_R+v1R
    sRoadStep = '{ %.3f , %.3f , %.3f , %.3f , %.3f , %.3f , %d } ,' % (PRoadL.x,PRoadL.y,PRoadL.z, PRoadR.x,PRoadR.y,PRoadR.z, bVisible)
    aRoadSeg.append(sRoadStep)

#    a = 'static unsigned char Road_Color = 250 250 250;'
    a = 'static string Texture = %s;' % seg.texture
    print >>f, ('static S_MarkerPoint %s[] = {' % 'Road')
    for aR in aRoadSeg:
        print >>f, aR
    print >>f, '}'
    print >>f, a # color
    # =================== Hack !!!!

    print "Export done."


def change_scale(event):
    global cnvScale,cnv_scale
    print cnvScale.get()
    cnv_scale = float(cnvScale.get())
    drawTrack(-1)

def change_texture(event):
    global s_texture
    i = int(lb2.curselection()[0])
    seg = aTrack[i]
    seg.set_texture(s_texture.get())
    lb2.delete(i)
    lb2.insert(i,"%s (%s)" % (seg.name, seg.texture))


if __name__ == "__main__":
# ----------------------------------------
# do not mix pack and grid  frame manager!
# ----------------------------------------
    menubar = Menu(root)
    menubar.add_command(label="Load",  command=load)
    menubar.add_command(label="Save",  command=save)
    menubar.add_command(label="Export",command=export)
    menubar.add_command(label="Exit",  command=root.quit)
    # display the menu
    root.config(menu=menubar)

    # --> scale the image almost "properly"
    root.columnconfigure(1, weight=1) # http://www.velocityreviews.com/forums/t869039-tkinter-why-arent-my-widgets-expanding-when-i-resize-the-window.html
    root.rowconfigure(1, weight=1)  # column 1 will resize


    scrollbar = Tkinter.Scrollbar(root, orient="vertical")

    frame = Frame(root, relief=RAISED, borderwidth=1)
    frame.pack(side=LEFT,fill=BOTH,expand=YES)
    
# --- LEFT ---
    w = Label(frame, text="Scale")
    w.pack()
    cnvScale = StringVar(root)
    cnvScale.set('1')
    opt = OptionMenu(frame, cnvScale, '0.5','1','2','3','4', command=change_scale)
    opt.pack(side=TOP)

    # 1a) basic markers
    lb = Listbox(frame, width=20, height=10, yscrollcommand=scrollbar.set, exportselection=False)
    lb.pack(side=TOP,fill=BOTH,expand=YES)

    scrollbar.config(command=lb.yview)
    bA = Button(frame, text="Append", command=btnAppend)
    bA.pack(fill=BOTH)
    bI = Button(frame, text="Insert", command=btnInsert)
    bI.pack(fill=BOTH)

    # 1b) track list
    lb2 = Listbox(frame, width=20, height=14, yscrollcommand=scrollbar.set, exportselection=False)
    lb2.pack(side=TOP,fill=BOTH,expand=YES)
    bD = Button(frame, text="Delete", command=btnInsert)
    bD.pack(fill=BOTH)

# --- CENTER ---
    # 2) track image
    cnv = ResizingCanvas(root, width=canvas_w, height=canvas_h, highlightthickness=0)
    cnv.create_rectangle(0, 0, canvas_w, canvas_h, fill="white")
    cnv.pack(side=LEFT, fill=BOTH, expand=YES)
    cnv.create_rectangle(0, 0, cnv.width, cnv.height, fill="white", tags="bg")

# --- RIGHT ---
    s_texture.set('texRoad') # StringVar(), see above
    opt = OptionMenu(root, s_texture, 'texRoad','texWater', command=change_texture)
    opt.pack(side=RIGHT)
    # 3) geometry properties
    geometry_properties(root)

    # 4) marker properties
#    marker_properties(root)

    i = 0
    for seg in aSegments:
        lb.insert(i, seg)
        i+=1
# now with button    lb.bind("<ButtonRelease-1>", lb_click)
    lb2.bind("<ButtonRelease-1>", lb2_click)

    root.mainloop()
