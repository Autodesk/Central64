import numpy as np
import matplotlib.pyplot as plt

def read_data(mode, benchmark):
    resultsfile = "analysis-" + mode + "-complete-" + benchmark + ".txt"
    infile = open(resultsfile)
    lines = infile.readlines()
    infile.close()
    data = {}
    i = 0
    while i < len(lines):
        if lines[i].startswith("Overall"):        
            i += 1
            line = lines[i]
            neighborhood = line[:line.find("-")]
            i += 1
            line = lines[i]
            approach = line[len("Paths produced by "):line.find(" Search")]
            variant = approach[:approach.find(" ")]
            search = approach[len(variant) + 1:]
            smoothing = line[line.find(" with ") + len(" with "):line.find(" Smoothing")].strip()
            i += 1
            line = lines[i]
            pathlength = line[len("Mean Path Length = "):].strip()
            i += 1
            line = lines[i]
            runtime = line[len("Mean Runtime (us) = "):].strip()
            if neighborhood not in data.keys():
                data[neighborhood] = {}
            if variant not in data[neighborhood].keys():
                data[neighborhood][variant] = {}
            if search not in data[neighborhood][variant].keys():
                data[neighborhood][variant][search] = {}
            data[neighborhood][variant][search][smoothing] = {}
            data[neighborhood][variant][search][smoothing]["Pathlength"] = pathlength
            data[neighborhood][variant][search][smoothing]["Runtime"] = runtime
        i += 1
    return data
    
def report_search_statistics(mode, benchmark, data, search):
    searchname = search.replace("*", "-Star").replace(" ", "-")
    outputpath = "table-" + mode + "-" + benchmark + "-" + searchname + "-Search.md"
    outfile = open(outputpath, "w")
    outfile.write("|Neighborhood|Search Method|Smoothing|Path Length|Runtime|\n")
    outfile.write("|:----------:|:-----------:|:-------:|:--------:|:-----:|\n")
    for smoothing in ["Greedy", "Tentpole"]:
        for variant in ["Regular", "Central"]:
            for neighborhood in ["4", "8", "16", "32", "64"]:
                pathlength = data[neighborhood][variant][search][smoothing]["Pathlength"]
                runtime = data[neighborhood][variant][search][smoothing]["Runtime"]
                outfile.write("|" + neighborhood + "|" + variant + " " + search + " Search|" + smoothing + "|" + pathlength + "|" + runtime + "|\n")
    outfile.close()
        
def plot_search_statistics(mode, benchmark, data, search, runtime0, pathlength0):
    searchname = search.replace("*", "-Star").replace(" ", "-")
    outputpath = "plot-" + mode + "-" + benchmark + "-" + searchname + "-Search.png"
    neighborhoods = ["4", "8", "16", "32", "64"]
    markersizes = ["8", "8", "12", "12", "12"]
    fig, ax = plt.subplots(figsize=(8, 8))
    ax.set_xlabel("Runtime (relative to reference method)")
    ax.set_ylabel("Suboptimality (degrees relative to best)")
    if mode == "heuristic":
        xmax = 7
        ymax = 25
        if search == "Mixed A*":
            xmax = 12
    elif mode == "dijkstra":
        xmax = 9
        ymax = 25
        if search == "Mixed Dijkstra":
            xmax = 14
    ax.set_xlim([0, xmax])
    ax.set_ylim([0, ymax])
    ax.set_aspect(0.8*xmax/ymax)
    ax.set_xticks(range(xmax + 1))
    seriesnames = []
    plotdata = {}
    for smoothing in ["Greedy", "Tentpole"]:
        for variant in ["Regular", "Central"]:
            seriesname = variant + " " + search + " Search (" + smoothing + ")"
            seriesnames.append(seriesname)
            plotdata[seriesname] = {}
            plotdata[seriesname]["Speed"] = []
            plotdata[seriesname]["Quality"] = []
            for neighborhood in neighborhoods:
                L = int(neighborhood)
                runtime = float(data[neighborhood][variant][search][smoothing]["Runtime"])
                speed = runtime/runtime0
                pathlength = float(data[neighborhood][variant][search][smoothing]["Pathlength"])
                quality = (180/np.pi)*np.arccos(pathlength0/pathlength)
                plotdata[seriesname]["Speed"].append(speed)
                plotdata[seriesname]["Quality"].append(quality)
    for seriesname in seriesnames:
        ax.plot(plotdata[seriesname]["Speed"], plotdata[seriesname]["Quality"])
    ax.legend(seriesnames)
    for seriesname in seriesnames:
        for i in range(len(neighborhoods)):
            speed = plotdata[seriesname]["Speed"][i]
            quality = plotdata[seriesname]["Quality"][i]
            ax.plot([speed], [quality], marker=".", markeredgecolor="black", markerfacecolor="black")
            ax.plot([speed + 0.1], [quality + 0.5], marker="$" + neighborhoods[i] + "$", markeredgecolor="black", markerfacecolor="black", markersize=markersizes[i])
    plt.savefig(outputpath, bbox_inches = "tight")
        
def plot_comparison_statistics(mode, benchmark, data, searches, runtime0, pathlength0):
    outputpath = "plot-" + mode + "-" + benchmark + ".png"
    neighborhoods = ["4", "8", "16", "32", "64"]
    markersizes = ["8", "8", "12", "12", "12"]
    fig, ax = plt.subplots(figsize=(8, 8))
    ax.set_xlabel("Runtime (relative to standard)")
    ax.set_ylabel("Suboptimality (degrees relative to best)")
    if mode == "heuristic":
        xmax = 5
        ymax = 8
    elif mode == "dijkstra":
        xmax = 7
        ymax = 8
    ax.set_xlim([0, xmax])
    ax.set_ylim([0, ymax])
    ax.set_aspect(0.8*xmax/ymax)
    ax.set_xticks(range(xmax + 1))
    seriesnames = []
    plotdata = {}
    smoothing = "Tentpole"
    for variant in ["Regular", "Central"]:
        for search in searches:
            seriesname = variant + " " + search + " Search"
            seriesnames.append(seriesname)
            plotdata[seriesname] = {}
            plotdata[seriesname]["Speed"] = []
            plotdata[seriesname]["Quality"] = []
            for neighborhood in neighborhoods:
                L = int(neighborhood)
                runtime = float(data[neighborhood][variant][search][smoothing]["Runtime"])
                speed = runtime/runtime0
                pathlength = float(data[neighborhood][variant][search][smoothing]["Pathlength"])
                quality = (180/np.pi)*np.arccos(pathlength0/pathlength)
                plotdata[seriesname]["Speed"].append(speed)
                plotdata[seriesname]["Quality"].append(quality)
    for seriesname in seriesnames:
        ax.plot(plotdata[seriesname]["Speed"], plotdata[seriesname]["Quality"])
    ax.legend(seriesnames)
    for seriesname in seriesnames:
        for i in range(len(neighborhoods)):
            speed = plotdata[seriesname]["Speed"][i]
            quality = plotdata[seriesname]["Quality"][i]
            ax.plot([speed], [quality], marker=".", markeredgecolor="black", markerfacecolor="black")
            ax.plot([speed + 0.1], [quality + 0.2], marker="$" + neighborhoods[i] + "$", markeredgecolor="black", markerfacecolor="black", markersize=markersizes[i])
    plt.savefig(outputpath, bbox_inches = "tight")
            
def visualize_analysis(mode, benchmark):
    data = read_data(mode, benchmark)
    if mode == "heuristic":    
        runtime0 = float(data["8"]["Regular"]["A*"]["Greedy"]["Runtime"])
        pathlength0 = float(data["64"]["Central"]["Bounded Jump Point"]["Tentpole"]["Pathlength"])       
        report_search_statistics(mode, benchmark, data, "A*")
        plot_search_statistics(mode, benchmark, data, "A*", runtime0, pathlength0)
        report_search_statistics(mode, benchmark, data, "Jump Point")
        plot_search_statistics(mode, benchmark, data, "Jump Point", runtime0, pathlength0)
        report_search_statistics(mode, benchmark, data, "Bounded Jump Point")
        plot_search_statistics(mode, benchmark, data, "Bounded Jump Point", runtime0, pathlength0)
        report_search_statistics(mode, benchmark, data, "Mixed A*")
        plot_search_statistics(mode, benchmark, data, "Mixed A*", runtime0, pathlength0)
        report_search_statistics(mode, benchmark, data, "Mixed Jump Point")
        plot_search_statistics(mode, benchmark, data, "Mixed Jump Point", runtime0, pathlength0)
        searches = ["A*", "Jump Point", "Bounded Jump Point", "Mixed Jump Point"]
        plot_comparison_statistics(mode, benchmark, data, searches, runtime0, pathlength0)
    elif mode == "dijkstra":
        runtime0 = float(data["8"]["Regular"]["Dijkstra"]["Greedy"]["Runtime"])
        pathlength0 = float(data["64"]["Central"]["Bounded Canonical Dijkstra"]["Tentpole"]["Pathlength"])
        report_search_statistics(mode, benchmark, data, "Dijkstra")
        plot_search_statistics(mode, benchmark, data, "Dijkstra", runtime0, pathlength0)
        report_search_statistics(mode, benchmark, data, "Canonical Dijkstra")
        plot_search_statistics(mode, benchmark, data, "Canonical Dijkstra", runtime0, pathlength0)
        report_search_statistics(mode, benchmark, data, "Bounded Canonical Dijkstra")
        plot_search_statistics(mode, benchmark, data, "Bounded Canonical Dijkstra", runtime0, pathlength0)
        report_search_statistics(mode, benchmark, data, "Mixed Dijkstra")
        plot_search_statistics(mode, benchmark, data, "Mixed Dijkstra", runtime0, pathlength0)
        report_search_statistics(mode, benchmark, data, "Mixed Canonical Dijkstra")
        plot_search_statistics(mode, benchmark, data, "Mixed Canonical Dijkstra", runtime0, pathlength0)
        searches = ["Dijkstra", "Canonical Dijkstra", "Bounded Canonical Dijkstra", "Mixed Canonical Dijkstra"]
        plot_comparison_statistics(mode, benchmark, data, searches, runtime0, pathlength0)

visualize_analysis("heuristic", "dao")
visualize_analysis("dijkstra", "dao")
