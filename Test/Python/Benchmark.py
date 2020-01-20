import subprocess
import os
import sys
import glob
import re

print("1. nek-ot(a.exe)\n2. gcc, clang(a.exe)")
directory = int(input())
curdir = str
pattern1 = 'TotalMilliseconds : .*?(\d+.\d+)'
sum = 0.0
for i in range(10):
 if(directory == 1):
  res = subprocess.run(["powershell", "Measure-Command{D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\a.exe}"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
 else:
  res = subprocess.run(["powershell", "Measure-Command{F:\\MYFOLDER\\ONEDRIVE\\SECHACK365\\PROJ\\a.exe}"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
 tim = re.findall(pattern1, res.stdout.decode("utf8"), re.S)[0]
 print(tim + "ms")
 sum += float(tim)
print("Average : " + sum / 10 + "ms")
exit(0)