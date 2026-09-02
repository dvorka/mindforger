#!/usr/bin/env python3
"""Generate the Snapcraft store banner for MindForger at 2160x720 (3:1)."""
import math
import os
import random
from pathlib import Path

from PIL import Image, ImageDraw, ImageFilter, ImageFont

W, H = 2160, 720
# Ubuntu system fonts, not repo content - same as mytral's make/make_seo_assets.py
FONT_BOLD = "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf"
FONT_REG = "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf"
ROOT = Path(__file__).resolve().parent.parent.parent
LOGO = str(ROOT / "media" / "logo" / "mind-forger-512x512.png")
SCREENSHOT = str(ROOT / "media" / "screenshots" / "tayr.png")
OUT = str(ROOT / "media" / "banners") + os.sep

MAX_BYTES = 2 * 1024 * 1024


def vgrad3(c1, c2, c3, w=W, h=H):
    # diagonal-ish feel via vertical gradient with 3 stops, cheap + smooth
    import numpy as np
    arr = np.zeros((h, w, 3), dtype=np.uint8)
    mid = h // 2
    for y in range(h):
        if y <= mid:
            t = y / mid
            src, dst = c1, c2
        else:
            t = (y - mid) / max(h - mid, 1)
            src, dst = c2, c3
        arr[y, :] = [round(src[i] + (dst[i] - src[i]) * t) for i in range(3)]
    return Image.fromarray(arr, "RGB").convert("RGBA")


def glow(canvas, cx, cy, radius, color, alpha, blur=110):
    layer = Image.new("RGBA", canvas.size, (0, 0, 0, 0))
    ImageDraw.Draw(layer).ellipse(
        [(cx - radius, cy - radius), (cx + radius, cy + radius)],
        fill=(*color, alpha),
    )
    return Image.alpha_composite(canvas, layer.filter(ImageFilter.GaussianBlur(blur)))


def load_img(path, target_h, rotate=0):
    img = Image.open(path).convert("RGBA")
    scale = target_h / img.height
    img = img.resize((int(img.width * scale), target_h), Image.LANCZOS)
    if rotate:
        img = img.rotate(rotate, expand=True, resample=Image.BICUBIC)
    return img


def with_shadow(canvas, img, x, y, offset=18, blur=24, alpha=150):
    shadow = Image.new("RGBA", canvas.size, (0, 0, 0, 0))
    mask = img.split()[3]
    shadow_img = Image.new("RGBA", img.size, (0, 0, 0, alpha))
    shadow.paste(shadow_img, (x + offset, y + offset), mask)
    shadow = shadow.filter(ImageFilter.GaussianBlur(blur))
    canvas = Image.alpha_composite(canvas, shadow)
    canvas.paste(img, (x, y), img)
    return canvas


def t(draw, text, x, y, font_path, size, color):
    draw.text((x, y), text, font=ImageFont.truetype(font_path, size), fill=color)


def save(canvas, name):
    path_png = OUT + name + ".png"
    canvas.convert("RGB").save(path_png, "PNG", optimize=True, compress_level=9)
    sz = os.path.getsize(path_png)
    if sz > MAX_BYTES:
        path_jpg = OUT + name + ".jpg"
        canvas.convert("RGB").save(path_jpg, "JPEG", quality=88, optimize=True)
        os.remove(path_png)
        print(f"  {name}.jpg  {os.path.getsize(path_jpg)//1024} KB")
    else:
        print(f"  {name}.png  {sz//1024} KB  {canvas.size}")


def banner_thinking_notebook():
    # near-black -> dark slate -> deep indigo, echoes MindForger's dark UI theme
    canvas = vgrad3((10, 11, 15), (17, 19, 26), (14, 16, 28))

    # teal glow (left, behind logo/text) + violet glow (right, behind screenshot)
    canvas = glow(canvas, int(W * 0.14), int(H * 0.35), 380, (45, 212, 191), 40)
    canvas = glow(canvas, int(W * 0.82), int(H * 0.65), 420, (139, 92, 246), 36)

    draw = ImageDraw.Draw(canvas)

    # decorative knowledge-graph motif: scattered nodes + connecting edges,
    # mirrors MindForger's own Knowledge Graph Navigator visual language
    random.seed(7)
    nodes = []
    for _ in range(26):
        nx = random.randint(int(W * 0.52), W - 60)
        ny = random.randint(40, H - 40)
        nodes.append((nx, ny))
    for i, (ax, ay) in enumerate(nodes):
        for bx, by in nodes[i + 1:]:
            d = math.hypot(ax - bx, ay - by)
            if d < 190:
                draw.line([(ax, ay), (bx, by)], fill=(0, 140, 0, 22), width=1)
    for nx, ny in nodes:
        r = random.choice([3, 3, 4, 5])
        c = random.choice([(45, 212, 191), (139, 92, 246), (226, 232, 240)])
        draw.ellipse([(nx - r, ny - r), (nx + r, ny + r)], fill=(*c, 130))

    # screenshot: Knowledge Graph Navigator, slightly rotated with a soft shadow
    shot = load_img(SCREENSHOT, 620, rotate=-3)
    x_shot = W - shot.width + 40
    y_shot = (H - shot.height) // 2
    canvas = with_shadow(canvas, shot, x_shot, y_shot, offset=20, blur=26, alpha=150)

    draw = ImageDraw.Draw(canvas)

    # logo
    logo = load_img(LOGO, 96)
    canvas.paste(logo, (90, 64), logo)
    draw = ImageDraw.Draw(canvas)

    # headline
    t(draw, "MindForger", 210, 68, FONT_BOLD, 92, (255, 255, 255, 255))
    t(draw, "Thinking notebook & Markdown IDE", 92, 200, FONT_BOLD, 44, (0, 140, 0, 255))

    t(draw, "Grow, link and search your ideas with a knowledge graph,", 94, 290, FONT_REG, 30, (203, 213, 225, 220))
    t(draw, "full-text search and Wingman AI - all running locally.", 94, 330, FONT_REG, 30, (203, 213, 225, 220))

    # divider + url
    draw.rectangle([(94, 420), (700, 424)], fill=(0, 140, 0, 190))
    t(draw, "mindforger.com", 94, 438, FONT_REG, 26, (148, 163, 184, 200))

    save(canvas, "banner-snapcraft-2160x720")


os.makedirs(OUT, exist_ok=True)
print("Generating MindForger snapcraft banner...")
banner_thinking_notebook()
print("Done.")
