import struct

def check_bmp(filepath):
    with open(filepath, "rb") as f:
        data = f.read()

        pixels = data[54:] # skip BMP header
        
        # Check if all pixels are purely opaque black (A=255, R=0, G=0, B=0 ... or some mix)
        # Assuming 32-bit BGRA
        from collections import Counter
        counts = Counter()
        for i in range(0, len(pixels), 4):
            if i+4 <= len(pixels):
                chunk = struct.unpack("BBBB", pixels[i:i+4])
                counts[chunk] += 1
        
        # Sort by frequency
        for color, count in counts.most_common(10):
            print(f"{color}: {count} pixels")

check_bmp("test_frame.bmp")
