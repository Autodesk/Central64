import os
import subprocess

def run_analysis(mode, benchmark, mapname, scenario = None):
    executable = os.path.join("..", "build", "Release", "Central64Analysis")
    if not os.path.exists(executable):
        executable = os.path.join("..", "build", "Central64Analysis")
    mappath = mapname + ".map"
    inputpath = os.path.join("..", "benchmarks", benchmark, mappath)
    command = executable + " " + mode + " partial " + inputpath
    problemname = mapname
    if scenario != None:
        command += (" " + scenario)
        problemname += ("-" + scenario)
    outputpath = "analysis-" + mode + "-partial-" + benchmark + "-" + problemname + ".txt"
    print(command + " > " + outputpath)
    outfile = open(outputpath, "w")
    subprocess.call(command, stdout=outfile)
    outfile.close()

run_analysis("heuristic", "dao", "arena")
run_analysis("heuristic", "dao", "arena", "111")
run_analysis("heuristic", "dao", "arena2")
run_analysis("heuristic", "dao", "ost000a")
run_analysis("dijkstra", "dao", "arena")
run_analysis("dijkstra", "dao", "arena", "111")
run_analysis("dijkstra", "dao", "arena2")
run_analysis("dijkstra", "dao", "ost000a")
