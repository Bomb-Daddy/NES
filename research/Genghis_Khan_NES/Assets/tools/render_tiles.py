#!/usr/bin/env python3
"""Render raw NES 2bpp tile data (.chr) to PNG.
usage: render_tiles.py in.chr out.png [--width TILES] [--scale N] [--palette RRGGBB,RRGGBB,RRGGBB,RRGGBB]
Defaults: 5 tiles wide (portraits), scale 1, portrait palette (black, orange, pale yellow, white).
Needs Pillow. Tile = 16 bytes: 8 bytes plane 0, then 8 bytes plane 1; pixel = (plane1 bit<<1)|plane0 bit."""
import sys, argparse
from PIL import Image
a = argparse.ArgumentParser()
a.add_argument("src"); a.add_argument("dst")
a.add_argument("--width", type=int, default=5)
a.add_argument("--scale", type=int, default=1)
a.add_argument("--palette", default="000000,FF9A38,FFE7A2,FFFFFF")
o = a.parse_args()
pal = [tuple(int(c[i:i+2], 16) for i in (0, 2, 4)) for c in o.palette.split(",")]
d = open(o.src, "rb").read()
n = len(d) // 16
h = -(-n // o.width)
im = Image.new("RGB", (o.width * 8, h * 8), pal[0])
for t in range(n):
    tx, ty = (t % o.width) * 8, (t // o.width) * 8
    for y in range(8):
        p0, p1 = d[t*16+y], d[t*16+8+y]
        for x in range(8):
            im.putpixel((tx+x, ty+y), pal[((p1 >> (7-x)) & 1) << 1 | ((p0 >> (7-x)) & 1)])
if o.scale > 1:
    im = im.resize((im.width*o.scale, im.height*o.scale), Image.NEAREST)
im.save(o.dst)
