import st, glob, os, sys
from PIL import Image
ROWS=[143,151,159,167,175,183]
def cursor(img):
    im=Image.open(img).convert('RGB'); best=None
    for i,y in enumerate(ROWS):
        n=0
        for yy in range(y-3,y+4):
            for xx in range(20,34):
                r,g,b=im.getpixel((xx,yy))
                if r>200 and g>200 and b>200: n+=1
        if best is None or n>best[0]: best=(n,i+1)
    return best[1] if best[0]>=4 else None
def menu_visible(img):
    im=Image.open(img).convert('RGB')
    # blue border of menu box at bottom-left around x=16,y=135..210
    n=0
    for y in range(135,215):
        r,g,b=im.getpixel((17,y))
        if b>150 and r<80: n+=1
    return n>40
def run(statefile, steps, tag):
    s=open(statefile).read()
    st.run_ad(steps,tag,s,stop=40000,maxwait=60,savesteps=True)
    fs=sorted(glob.glob('/home/claude/cap/%s/f*.png'%tag)); last=fs[-1]
    stf='/home/claude/cap/%s/state_%s.b64'%(tag,last.split('/f')[-1][:-4])
    return stf,last,fs
