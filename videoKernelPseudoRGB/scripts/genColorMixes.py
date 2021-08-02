from itertools import permutations, combinations

colors = [
    (0, 0, 0),
    (0, 0, 168),
    (0, 168, 0),
    (0, 168, 168),
    (168, 0, 0),
    (168, 0, 168),
    (168, 168, 0),
    (168, 168, 168),
    (87, 87, 87),
    (87, 87, 255),
    (87, 255, 87),
    (87, 255, 255),
    (255, 87, 87),
    (255, 87, 255),
    (255, 255, 87),
    (255, 255, 255)
]

possibile_colors = list(set(permutations(colors, 2)))
combos = []
for c in possibile_colors:
    for i in [176, 177, 178, 219]:
        combos.append( (tuple(c), i) )

possibile_colors.clear()

ratios = {
    176: 0.33333333333,
    177: 0.5,
    178: 0.73333333333,
    219: 1
}

def mergeColor(c, c1, c2):
    p = ratios[c]
    return tuple([max(0, min(255, int(a+b))) for a,b in zip(tuple([p * x for x in c1]), (tuple([(1 - p) * x for x in c2])))])

def vga_entry_color(fg, bg):
    return fg | bg << 4

def vga_entry(uc, color):
    return uc | color << 8


out_colors = {}
for i in combos:
    out_colors[mergeColor(i[1], i[0][0], i[0][1])] = vga_entry(i[1], vga_entry_color(colors.index(i[0][0]), colors.index(i[0][1])))

f = open("vgaColors.py", "w+")
f.write("VGA_COLOR_COMBOS = " + repr(out_colors))