import re,sys,csv,json
rom=open(sys.argv[1],"rb").read()
out=[]
for b in range(16):
    base=0x10+b*0x4000; d=rom[base:base+0x4000]; ad=0xC000 if b==15 else 0x8000
    for m in re.finditer(b"\xe9\x88\xf9\x0a",d):
        i=m.start(); t=d[max(0,i-12):i]
        rec={"site_bank":b,"site_addr":ad+i+2,"static":False}
        # t[-1]=flag t[-2]=bank t[-5:-2]=8E src t[-8:-5]=8E dst, count before
        def lit(x): return 0x60<=x<=0x6F
        if len(t)>=10 and lit(t[-1]) and lit(t[-2]) and t[-5]==0x8E and t[-8]==0x8E:
            src=t[-4]|t[-3]<<8; dst=t[-7]|t[-6]<<8
            if t[-10]==0x8D: cnt=t[-9]
            elif lit(t[-9]): cnt=t[-9]&0xF
            else: cnt=None
            if cnt is not None:
                rec.update(static=True,count=cnt,dst=dst,src=src,src_bank=t[-2]&0xF,flag=t[-1]&0xF)
        # dst given as literal small const (60) before src
        if not rec["static"] and len(t)>=7 and lit(t[-1]) and lit(t[-2]) and t[-5]==0x8E and t[-6]==0x60 and t[-8]==0x8D:
            rec.update(static=True,count=t[-7],dst=0,src=t[-4]|t[-3]<<8,src_bank=t[-2]&0xF,flag=t[-1]&0xF)
        rec["raw_tail"]=t.hex()
        out.append(rec)
json.dump(out,open(sys.argv[2],"w"),indent=1)
st=[r for r in out if r["static"]]
print(len(out),"sites",len(st),"static")
for r in st: print(r["site_bank"],hex(r["site_addr"]),"bank",r["src_bank"],hex(r["src"]),"->",hex(r["dst"]),"n",r["count"],"flag",r["flag"])
