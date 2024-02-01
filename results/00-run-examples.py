import os
import subprocess

command = "../build/Release/Central64Examples.exe"
if not os.path.exists(command):
    command = "../build/Central64Examples"
outputpath = "examples.txt"
print(command + " > " + outputpath)
outfile = open(outputpath, "w")
subprocess.call(command, stdout=outfile)
outfile.close()
