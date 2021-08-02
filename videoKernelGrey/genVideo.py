#!/bin/python3

# importing the necessary libraries
from ctypes import resize
import cv2
import argparse
import os
import sys

parser = argparse.ArgumentParser(description='Convert a video to a format and size playable by Video Kernel')
parser.add_argument('in_file', help='Video file to convert')
parser.add_argument('-o', '--out', required=False, help='Output header path, defaults to `src/c++/kernel/<video_name>.hpp`')
args = parser.parse_args(sys.argv[1:])

# Creating a VideoCapture object to read the video
os.system(f"ffmpeg -i {args.in_file} -r 24 -y {args.in_file}.c.mp4")
cap = cv2.VideoCapture(f"{args.in_file}.c.mp4")
v_out_header = open(args.out or 'src/c++/kernel/' + ('.'.join(args.in_file.split('.')[:-1])).replace('/', '\\').split('\\')[-1] + '.hpp', "w+")
 
frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
framesDone = 0
resolution = (80, 25)

v_out_header.write(f"""#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
uint8_t video[] = {{                  
    {(frames & 0xff00) >> 8}, {frames & 0xff}, // number of frames, 2 bytes
    {resolution[0]}, {resolution[1]},          // width and height
// Frame data
""")

def flattenColor(c, thresh=127):
    return (1 if c > thresh else 0)

# Loop until the end of the video
while (cap.isOpened()):
    videoFrameTmp = [
    
    ]
    framesDone += 1
    # Capture frame-by-frame
    ret, frame = cap.read()
    if not ret:
        break
    frame = cv2.resize(cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY), resolution, interpolation=cv2.INTER_AREA).flatten()
    
    for i in range(resolution[1]):
        tmp = []
        for y in range(int(resolution[0]/8)):
            tmp.append(f'0b{flattenColor(frame[(i*resolution[0])+y*int(resolution[0]/10)])}{flattenColor(frame[(i*resolution[0])+y*int(resolution[0]/10)+1])}{flattenColor(frame[(i*resolution[0])+y*int(resolution[0]/10)+2])}{flattenColor(frame[(i*resolution[0])+y*int(resolution[0]/10)+3])}{flattenColor(frame[(i*resolution[0])+y*int(resolution[0]/10)+4])}{flattenColor(frame[(i*resolution[0])+y*int(resolution[0]/10)+5])}{flattenColor(frame[(i*resolution[0])+y*int(resolution[0]/10)+6])}{flattenColor(frame[(i*resolution[0])+y*int(resolution[0]/10)+7])}')
        videoFrameTmp.append(", ".join(tmp) + ",\n    ")

    v_out_header.write(f"// Frame {framesDone-1}\n    " + "".join(videoFrameTmp))
    print(f"{framesDone}/{frames}")

v_out_header.write("};")
 
# release the video capture object
cap.release()

v_out_header.close()
os.remove(f"{args.in_file}.c.mp4")