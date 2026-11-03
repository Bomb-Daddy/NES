import ctl
s='/home/claude/cap/p118_01/state_000390.b64'
c=1
for d in ['down','up','left','right']:
    need=(2-c)%6
    s2,im,fs=ctl.run(s,['down']*need+['a','down','a',d,'a','x','x','x','x'],'p119_'+d)
    print(d,s2,[f[-12:] for f in fs],flush=True)
