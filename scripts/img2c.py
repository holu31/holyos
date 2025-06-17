from PIL import Image
import sys
import os

if len(sys.argv) < 3:
    print(f"Usage: python {sys.argv[0]} <input.png> <output.h>")
    sys.exit(1)

input_paths = sys.argv[1].split()
output_path = sys.argv[2]

f = open(output_path, "w")

for input_path in input_paths:
    filename = os.path.splitext(os.path.basename(input_path))[0]

    img = Image.open(input_path)

    white_bg = Image.new('RGBA', img.size, (255, 255, 255, 255))
    white_bg.paste(img, (0, 0), img)
    img = white_bg

    img = img.convert("RGB")
    pixels = img.load()
    width, height = img.size
    
    f.write(f"// Generated from {input_path}\n")
    f.write(f"const unsigned int {filename}_width = {width};\n")
    f.write(f"const unsigned int {filename}_height = {height};\n")
    f.write(f"const unsigned char {filename}_data[] = {{\n")

    for y in range(height):
        for x in range(width):
            r, g, b = pixels[x, y][:3]
            f.write(f"0x{r:02x}, 0x{g:02x}, 0x{b:02x}, ")
        f.write("\n")

    f.write("};\n")

f.close()
