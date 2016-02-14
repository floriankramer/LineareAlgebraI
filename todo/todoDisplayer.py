#!/usr/bin/env python3

import os
from tkinter import *
from tkinter.ttk import *
import tkinter.font as tkfont

class Tree():

    def __init__(self, name, finished, isroot = True):
        self.name = name
        self.finished = finished
        self.children = []
        self.width = 0
        self.isroot = isroot
        self.label = None
        self.parent = None
        self.off = 0
        self.visible = True#isroot
        self.visToggled = False
        pass

    def add(self, name, value):
        t = Tree(name, value, isroot = False)
        t.parent = self
        self.children.append(t)
        return t

    def hide(self):
        for c in self.children:
            c.visible = False
            c.label.place(width=0, height=0)
            c.hide()

    def show(self):
        for c in self.children:
            c.visible = True
            c.label.place(width=font.measure(c.label["text"]) + 5, height=font.metrics("linespace") + 5)

            if not c.visToggled:
                                c.show()

    def display(self, depth = 0):
        print("    " * depth, self.name, str(self.finished))
        for c in self.children:
            c.display(depth + 1)

    def updateFinished(self):
        self.finished =  all([c.finished for c in self.children])
        if self.parent:
            self.parent.updateFinished()

    def toggleFinished(self):
        self.finished = not self.finished
        self.parent.updateFinished()

    def depth(self):
        if len(self.children > 0):
            return 1 + max(c.depth for c in self.children)
        else:
            return 1

    def compute_width(self, font):
        self.width = max(sum([w.compute_width(font) for w in self.children if w.visible]), font.measure(self.name) + 30 if self.visible else 0)
        return self.width

    def __str__(self):
        return self.name + " " + str(self.finished)

    def __repr__(self):
        return "Tree(%s, %s)" %(self.name, str(self.finished))



tree = None
tk = None
canv = None
font = None
level = []


def count_tabs(s):
    for i in range(0, len(s)):
        if s[i] != "\t":
            return i
    return 0

def load_tree(path):
    global tree
    if not os.path.isabs(path):
        path = os.path.abspath(path)
    with open(path) as file:

        trace = []
        tabs = -1

        for line in file:
            if(len(line.strip()) == 0 or line.lstrip()[0] == "#"):
                continue
            t = count_tabs(line)#line.count("\t")
            line = line.strip()
            p = line.split(" ")
            name = line[0:-len(p[-1])].strip()
            finished = p[-1].strip() != "0" 


            
            for _ in range(0, tabs - t + 1):
                trace.pop()
            tabs = t

           

            if tree == None:
                tree = Tree(name, finished)
                trace.append(tree)
            else:
                try: 
                    trace.append(trace[-1].add(name, finished)) 
                except Exception as e:
                    print("error in tree structure:", e)
           
            


def build_level():
    global tree, level
    level.append([tree])
    i = 0
    while len(level[-1]) > 0:
        level.append([])
        for t in level[-2]:
            if i > 2:
                t.visible = False
            if len(t.children) > 0:
                level[-1].extend(t.children)
                t.updateFinished()
        i += 1
    level.pop()


lx = None
ly = None
def scroll(event):
    #if not event.widget == tk:
    #    return
    global lx, ly
    if lx is None: 
        lx = event.x_root
        ly = event.y_root 
   
    i = canv.place_info()
    dx = (event.x_root - lx)
    dy = (event.y_root - ly)
    
    canv.place(x=int(i['x']) + dx, y=int(i['y']) + dy)
    lx = event.x_root
    ly = event.y_root


def mouse1release(event):
    global lx
    lx = None

    
def build_node(name, t):
    height = font.metrics("linespace")
    l = Label(canv, text=name, font=font) 
    #l.bind("<Button-1>", )
    def onToggleHidden(event):
        t.visToggled = not t.visToggled

        if t.children[0] and t.children[0].visible:
            t.hide()
        else:
            t.show()      
        rebuildTree(ref=t)
    def onChangeFinished(event):
        if len(t.children) == 0: 
            t.toggleFinished()
            rebuildTree()

    l.bind("<Button-3>", onToggleHidden)
    l.bind("<Button-1>", onChangeFinished)
    return l

def rebuildTree(ref=None):
    """ 
    params:
            ref(Tree):  when ref is not None, the canvas is shifted in such a way, as to
                        ensure, that the position of ref is the same as it was before the
                        update. Ref has to be visible
        
    """
    tree.compute_width(font)
    canv.delete("all")
    offtop = 30
    linespacing = 70
    halflsp = 35
    lineheight = font.metrics("linespace")
    radcirc = lineheight * 0.6
    windowWidth = tree.width + 30
    windowHeight = len(level) * (font.metrics("linespace") + 10 + offtop + 30)
    for i in range(0, len(level)):
            #off = 0
            for t in level[i]:
                if t.visible:
                    t.off = -t.width / 2
                    if not t.parent is None:
                        x = t.parent.x + t.parent.off + t.width / 2
                        t.parent.off += t.width
                    else:
                        x = t.width / 2
                    y = i*(lineheight + linespacing) + offtop
                    if t == ref: 
                        inf = canv.place_info() 
                        canv.place(x=canv.winfo_x() + t.x - x)

                    t.x = x
                    t.y = y
                    t.label.place(x=x - font.measure(t.name) / 2, y=y)
                    col = "black"
                    parcol = "black"
                    if t.finished: 
                        col = "#888888"
                    if t.parent and t.parent.finished:
                        parcol = "#888888"
                       # canv.create_oval(x - font.measure(t.name) / 2 - radcirc - 5, y, x - font.measure(t.name) / 2 - 5, y + radcirc, fill="#33AA55")
                    t.label.config(foreground=col)
                    if not t.parent == None:
                        canv.create_line(x, y, x, y - halflsp, fill=col)
                        canv.create_line(x, y - halflsp, t.parent.x, y - halflsp, fill=parcol)
                        canv.create_line(t.parent.x, y - halflsp, t.parent.x, t.parent.y + 5, fill=parcol)
                    if len(t.children) > 0 and not t.children[0].visible:
                        canv.create_line(x, y - lineheight, x, y + lineheight + 15, fill=col)


def writeTree(f, t, depth=0):
    s = ("\t" * depth) + t.name + " " + ("1" if t.finished else "0") + "\n" 
    f.write(str(s))
    for c in t.children:
        writeTree(f, c, depth+1)

def save():
    path="./todo.txt"
    if not os.path.isabs(path):
       path = os.path.abspath(path)
    print(path)
    with open(path, "w") as f:
        writeTree(f, tree)



def init_tkinter():
    global canv

    offtop = 30
    linespacing = 70
    halflsp = 35
    windowWidth = tree.width + 30
    windowHeight = len(level) * (font.metrics("linespace") + 10 + offtop + 30)
    canv = Canvas(tk)
    canv.place(x=0, y=0, width=windowWidth, height=windowHeight)

    tk.bind("<B1-Motion>", scroll)
    tk.bind("<ButtonRelease-1>", mouse1release)

    for i in range(0, len(level)):
        for t in level[i]:
            t.label = build_node(t.name, t)
 
    tk.geometry(str(tk.winfo_screenwidth()) + "x" + str(tk.winfo_screenheight()))
  
    rebuildTree()
    
    menubar = Menu(tk)
    menubar.add_command(label="Save", command=save) 
    tk.config(menu=menubar)
    tk.mainloop()
   



if __name__ == "__main__":
    tk = Tk()
    font = tkfont.Font(family="sans serif", size=12)

    load_tree("./todo.txt")
    tree.compute_width(font)
    build_level()
    #tree.display()
    init_tkinter()

