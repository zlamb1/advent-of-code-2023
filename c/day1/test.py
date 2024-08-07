f = open("day1.txt", "r")
lines = f.readlines()

digits = {
    "one":   1,
    "two":   2,
    "three": 3,
    "four":  4,
    "five":  5,
    "six":   6,
    "seven": 7,
    "eight": 8,
    "nine":  9
}

accum = 0
for i in range(len(lines)):
    line = lines[i]
    nums = []
    for j in range(len(line)):
        c = line[j]
        if (c.isnumeric()):
            nums.append(int(c))
        else:
            for digit in digits:
                if line[j : j + len(digit)] == digit:
                    nums.append(digits[digit])
    l = len(nums)
    s = 0
    if l == 1:
        s = nums[0] * 10 + nums[0]
    elif l > 1:
        s = nums[0] * 10 + nums[l - 1]
    accum += s
    print("line", i, ":", s)

print("accum:", accum)