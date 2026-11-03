import sys, os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gk, numpy as np, pickle, os
from PIL import Image
def shot(c, name, scale=2):
    im = Image.fromarray(c.render()).resize((256*scale,240*scale), Image.NEAREST); im.save(f"shots/{name}.png")
os.makedirs('shots',exist_ok=True)
import ctypes as C
def loadstate(c,path):
    st=gk.State(); b=open(path,'rb').read(); C.memmove(C.byref(st),b,len(b)); c.load(st)
def goto(c, target, addr=0x5A6):
    for _ in range(30):
        cur=c.rd(addr)
        if cur==target: return True
        c.press(gk.DOWN if cur<target else gk.UP,4,15)
    return False
def W(c,a):
    w=bytes(c.S.wram); o=a-0x6000; return w[o]|w[o+1]<<8
