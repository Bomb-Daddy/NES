from common import *
from adapt import runad
turns=[('down','down'),('right','right'),('down','right'),('up','right'),('right','up'),('left','left')]
steps=[]
for d1,d2 in turns:
    steps+=['a','a',d1,'a',d2,'a','b']
    steps+=['down']*5+['a']+['x']*10
runad(steps,'p41',200000,pre=BASE+PRE8[:])
