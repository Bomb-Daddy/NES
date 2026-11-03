import ctl, glob
from p111fail import fail
n=[30]
def go(s,steps):
    n[0]+=1; return ctl.run(s,steps,'p117_%02d'%n[0])
stf='/home/claude/cap/p116_left/state_000837.b64'
def dz_menu(stf):
    stf,img,fs=go(stf,['up','a'])
    for k in range(6):
        stf,img,fs=go(stf,['x','x'])
        if ctl.menu_visible(img): break
    return stf,img
stf,img=dz_menu(stf); c=ctl.cursor(img); print('menu',c,flush=True)
need=(1-c)%6
s2,im2,fs2=go(stf,['down']*need+['a','a','left','a','x'])
print('moved left fail=',fail(fs2[-1]),fail(fs2[-2]),im2,flush=True)
stf,img=dz_menu(s2); c=ctl.cursor(img); print('menu2',c,img,flush=True)
need=(2-c)%6
s3,im3,fs3=go(stf,['down']*need+['a','down','a','down','a','x','x','x'])
print('attack',s3,fs3,flush=True)
