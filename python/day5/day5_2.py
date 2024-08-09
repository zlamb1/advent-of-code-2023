f = open("day5.txt", "r")
l = f.readlines()
lines = []

for (i, line) in enumerate(l):
    if line.strip():
        lines.append(line.replace("\n", ""))

class seed_range:
    def __init__(self, start, end, _map = 0, _range = 0, matched = False):
        self.start = start
        self.end = end
        self.map = _map
        self.range = _range
        self.matched = matched
    def __repr__(self):
        return str(self.start) + " -> " + str(self.end) + " [" + str(self.map) + "][" + str(self.range) + "] : " + str(self.matched)

class num_range:
    def __init__(self, dst, src, length):
        self.dst = dst
        self.src = src
        self.length = length
    def __repr__(self):
        return "(" + str(self.src) + ":" + str(self.src + self.length - 1) + ") -> " + str(self.dst) 

class num_map:
    def __init__(self, name):
        self.name = name
        self.ranges = []
    def __repr__(self):
        return self.name + ": " + str(self.ranges)

def consume_whitespace(str, l = []):
    while l[0] < len(str):
        c = str[l[0]]
        if c != " ":
            break
        l[0] += 1

def parse_num(str, l = []):
    accum = ""
    if l[0] >= len(str): return 0
    c = str[l[0]]
    while l[0] < len(str) and c.isnumeric():
        c = str[l[0]]
        accum += c
        l[0] += 1
    if len(accum) == 0: return 0
    return int(accum)

seeds = []
first_line = lines[0]
first_line = first_line.replace("seeds:", "")

l = [0]
while l[0] < len(first_line):
    consume_whitespace(first_line, l)
    start = parse_num(first_line, l)
    consume_whitespace(first_line, l)
    length = parse_num(first_line, l)
    consume_whitespace(first_line, l)
    seeds.append(seed_range(start, start + length - 1))

num_maps = []
i = 1
while i < len(lines):
    l = [0]
    line = lines[i]
    if line[0].isnumeric():
        dst = parse_num(line, l)
        consume_whitespace(line, l)
        src = parse_num(line, l)
        consume_whitespace(line, l)
        length = parse_num(line, l)
        num_maps[-1].ranges.append(num_range(dst, src, length))
    else:
        num_maps.append(num_map(line.replace(" map:", "")))
    i += 1

min_range = float('inf');
while len(seeds) > 0:
    s_range = seeds.pop()
    if s_range.map >= len(num_maps):
        if s_range.start < min_range:
            min_range = s_range.start
        continue
    num_map = num_maps[s_range.map]
    if s_range.range >= len(num_map.ranges):
        if not s_range.matched:
            s_range.map += 1
            s_range.range = 0
            s_range.matched = False
            seeds.append(s_range)
        continue
    i = s_range.range
    while i < len(num_map.ranges):
        num_range = num_map.ranges[i]
        start = num_range.src
        end = num_range.src + num_range.length - 1
        if s_range.start <= start and s_range.end >= end:
            if s_range.start < start:
                seeds.append(seed_range(s_range.start, start - 1, s_range.map, i + 1, s_range.matched))
            seeds.append(seed_range(start, end, s_range.map, i + 1, True))
            seeds.append(seed_range(num_range.dst, num_range.dst + end - start, s_range.map + 1))
            if s_range.end > end:
                seeds.append(seed_range(end + 1, s_range.end, s_range.map, i + 1, s_range.matched))
            break
        elif s_range.start >= start and s_range.end <= end:
            s_range.range = i + 1
            s_range.matched = True
            seeds.append(s_range)
            seeds.append(seed_range(num_range.dst + s_range.start - start, num_range.dst + s_range.end - start, s_range.map + 1))
            break
        elif s_range.start < start and s_range.end >= start:
            seeds.append(seed_range(s_range.start, start - 1, s_range.map, i + 1, s_range.matched))
            seeds.append(seed_range(start, start + s_range.end - start, s_range.map, i + 1, True))
            seeds.append(seed_range(num_range.dst, num_range.dst + s_range.end - start, s_range.map + 1))
            break
        elif s_range.start <= end and s_range.end > end:
            seeds.append(seed_range(s_range.start, end, s_range.map, i + 1, True))
            seeds.append(seed_range(num_range.dst + s_range.start - start, num_range.dst + end - start, s_range.map + 1))
            seeds.append(seed_range(end + 1, s_range.end, s_range.map, i + 1, s_range.matched))
            break
        elif i == len(num_map.ranges) - 1 and not s_range.matched:
            s_range.map += 1
            s_range.range = 0
            s_range.matched = False
            seeds.append(s_range)
            break
        i += 1

print(min_range)