import os
import subprocess

command = os.path.join("..", "build", "Release", "Central64Examples")
if not os.path.exists(command):
    command = os.path.join("..", "build", "Central64Examples")
outputpath = "examples.txt"
print(command + " > " + outputpath)
outfile = open(outputpath, "w")
subprocess.call(command, stdout=outfile)
outfile.close()
