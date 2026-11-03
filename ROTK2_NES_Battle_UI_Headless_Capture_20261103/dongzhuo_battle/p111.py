import ctl, glob, sys, shutil
from PIL import Image
def fail(img):
    im=Image.open(img).convert('RGB'); n=0
    for y in range(78,97):
        for x in range(195,246):
            r,g,b=im.getpixel((x,y))
            if r>200 and g>200 and b>200: n+=1
    return n>30
state=sorted(glob.glob('/home/claude/cap/p110/state_*.b64'))[0]  # Jia's menu, cursor 1
n=0
def go(state,steps):
    global n; n+=1
    return ctl.run(state,steps,'p111_%02d'%n)
log=[]
for day in range(3):
    # Jia waits
    stf,img,fs=go(state,['up','a'])
    # run enemy turn until menu visible
    for k in range(6):
        stf,img,fs=go(stf,['x','x'])
        if ctl.menu_visible(img): break
    c=ctl.cursor(img); log.append(('DZ menu',day,c,img))
    # DZ moves
    done=False
    for d in ['down','left','right','up','down']:
        need=(1-c)%6
        steps=['down']*need+['a','a',d,'a','x']
        s2,im2,fs2=go(stf,steps)
        if not fail(fs2[-1]) and not fail(fs2[-2]):
            state=s2; done=True; log.append(('moved',day,d,im2)); break
        log.append(('blocked',day,d))
    if not done: break
    # state is Jia's menu again?
    print(log[-3:],flush=True)
open('/home/claude/cap/p111.log','w').write(repr(log))
