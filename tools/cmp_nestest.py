import re
import sys


def fields(line):
    m = re.match(r'(\w{4}).*?A:(\w\w) X:(\w\w) Y:(\w\w) P:(\w\w) SP:(\w\w)(?:.*CYC:(\d+))?', line)
    return m.groups() if m else None


mine, ref = open(sys.argv[1]), open(sys.argv[2])
for n, (a, b) in enumerate(zip(mine, ref), 1):
    fa, fb = fields(a), fields(b)
    if fa[:6] != fb[:6]:
        print('line %d differs\n  mine: %s  ref:  %s' % (n, a, b))
        sys.exit(1)
print('ok')
