#!/bin/python3

# importing the necessary libraries
import cv2
import argparse
import os
import sys
import numpy
from PIL import Image
from vgaColors import VGA_COLOR_COMBOS
from scipy.spatial import cKDTree

class NegateAction(argparse.Action):
    def __call__(self, parser, ns, values, option):
        setattr(ns, self.dest, option[2:4] != 'no')

parser = argparse.ArgumentParser(description='Convert a video to a format and size playable by Video Kernel')
parser.add_argument('in_file', help='Video file to convert')
parser.add_argument('-o', '--out', required=False, help='Output header path, defaults to `src/videos/compiled/<video_name>.o`')
parser.add_argument('-s', '--square_pixels', required=False, help='Takes no args, if enabled will make pixels 2 characters wide so they are square and reduce max width to 40px, default disabled', dest='sqare_pixels', action=NegateAction, nargs=0)
args = parser.parse_args(sys.argv[1:])

# Creating a VideoCapture object to read the video
os.system(f"ffmpeg -i {args.in_file} -r 24 -y {args.in_file}.c.mp4")
cap = cv2.VideoCapture(f"{args.in_file}.c.mp4")
v_out_tmp_header = open(args.out or 'src/videos/src/' + ('.'.join(args.in_file.split('.')[:-1])).replace('/', '\\').split('\\')[-1] + '.cpp.tmp', "w+")
 
frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
framesDone = 0
resolution = (80/(2 if args.sqare_pixels else 1), 25)

v_out_tmp_header.write(f"""#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
uint16_t video[] = {{                  
    {frames}, // number of frames, 2 bytes
    --resolution-- // width and height
    // Frame data
    """)

def flattenColor(c, thresh=127):
    return (1 if c > thresh else 0)

vgaColors = numpy.array([numpy.array(i) for i in VGA_COLOR_COMBOS.keys()])
# Loop until the end of the video
while (cap.isOpened()):
    videoFrameTmp = [
    
    ]
    framesDone += 1
    # Capture frame-by-frame
    ret, frame = cap.read()
    if not ret:
        break
    im: Image.Image = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
    im.thumbnail(resolution, Image.LANCZOS)
    frame: numpy.array = numpy.reshape(vgaColors[cKDTree(vgaColors).query(numpy.array(im), k=1)[1]], (-1, 3))
    resolution = im.size
    
    for y in range(im.size[1]):
        tmp = []
        for x in range(im.size[0]):
            for i in range((2 if args.sqare_pixels else 1)):
                tmp.append(f'{VGA_COLOR_COMBOS[tuple(frame[x+y*im.size[0]])]}')
        videoFrameTmp.append(", ".join(tmp) + ",\n    ")

    v_out_tmp_header.write(f"// Frame {framesDone-1}\n    " + "".join(videoFrameTmp))
    print(f"{framesDone}/{frames}")

v_out_tmp_header.write("};")
 
# release the video capture object
cap.release()

v_out_header = open(args.out or 'src/videos/src/' + ('.'.join(args.in_file.split('.')[:-1])).replace('/', '\\').split('\\')[-1] + '.cpp', "w+")
v_out_tmp_header.seek(0)
tmp = v_out_tmp_header.read()
v_out_header.write(tmp.replace("--resolution--", f"{resolution[0]*(2 if args.sqare_pixels else 1)}, {resolution[1]},"))

v_out_tmp_header.close()
v_out_header.close()
os.remove(f"{args.in_file}.c.mp4")
os.remove(args.out or 'src/videos/src/' + ('.'.join(args.in_file.split('.')[:-1])).replace('/', '\\').split('\\')[-1] + '.cpp.tmp')

print("Compiling", 'src/videos/src/' + ('.'.join(args.in_file.split('.')[:-1])).replace('/', '\\').split('\\')[-1] + ".cpp", "to", (args.out or 'src/videos/compiled/' + ('.'.join(args.in_file.split('.')[:-1])).replace('/', '\\').split('\\')[-1] + ".o"))
print("i686-elf-g++ -c src/videos/src/" + ('.'.join(args.in_file.split('.')[:-1])).replace('/', '\\').split('\\')[-1] + ".cpp -o " + (args.out or 'src/videos/compiled/' + ('.'.join(args.in_file.split('.')[:-1])).replace('/', '\\').split('\\')[-1] + ".o") + " -std=gnu++11 -ffreestanding -O3 -Wall -Wextra -fno-exceptions -fno-rtti -fpermissive")
os.system("i686-elf-g++ -c src/videos/src/" + ('.'.join(args.in_file.split('.')[:-1])).replace('/', '\\').split('\\')[-1] + ".cpp -o " + (args.out or 'src/videos/compiled/' + ('.'.join(args.in_file.split('.')[:-1])).replace('/', '\\').split('\\')[-1] + ".o") + " -std=gnu++11 -ffreestanding -O3 -Wall -Wextra -fno-exceptions -fno-rtti -fpermissive")