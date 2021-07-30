import os
import re

os.chdir("instances")
files = [f"umps{n}.txt" for n in [4, 6, 8, 10, 12, 14, 16, 18, 20, 30]]

for file in files:
    with open(file) as f:
        data = re.sub(r";\n+", ";\n", f.read().strip()).split(";")
