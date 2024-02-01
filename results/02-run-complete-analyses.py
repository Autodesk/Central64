import os
import subprocess

def run_analysis(mode, benchmark, mapname = None):
    executable = "../build/Release/Central64Analysis.exe"
    if not os.path.exists(executable):
        executable = "../build/Central64Analysis"        
    inputpath = os.path.join("..", "benchmarks", benchmark)
    if mapname != None:
        inputpath = os.path.join(inputpath, mapname + ".map")
    command = executable + " " + mode + " complete " + inputpath
    problemname = benchmark
    if mapname != None:
        problemname += ("-" + mapname)
    outputpath = "analysis-" + mode + "-complete-" + problemname + ".txt"
    print(command + " > " + outputpath)
    outfile = open(outputpath, "w")
    subprocess.call(command, stdout=outfile)
    outfile.close()

run_analysis("heuristic", "dao")
run_analysis("dijkstra", "dao")
