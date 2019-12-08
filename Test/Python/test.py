import subprocess
import os
import sys
import glob

class Color:
    BLACK     = '\033[30m'
    RED       = '\033[31m'
    GREEN     = '\033[32m'
    YELLOW    = '\033[33m'
    BLUE      = '\033[34m'
    PURPLE    = '\033[35m'
    CYAN      = '\033[36m'
    WHITE     = '\033[37m'
    END       = '\033[0m'
    BOLD      = '\038[1m'
    UNDERLINE = '\033[4m'
    INVISIBLE = '\033[08m'
    REVERCE   = '\033[07m'



command = ["nek-ot", "test\\def_func.nk"]

os.chdir("D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot")

r = glob.glob("D:\\LLVM\\llvm-project\\build\\examples\\Kaleidoscope\\nek-ot\\test\\*.nk")

for a in r:
 subprocess.run(["rm", "a.exe"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
 res = subprocess.run(["nek-ot", a, "-O3"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
 if(os.path.isfile("a.exe")):
     print(Color.GREEN + "Compile OK -> " + a + Color.END)
 else:
     print(Color.RED + "Compile Fail -> " + a + "\r\n" + res.stdout.decode("utf8") + Color.END)


exit(0)