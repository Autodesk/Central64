# Generates "central64-technical-report.md" by concatenating
# and modifying the Mardown source files for each page of the
# online version of the report. The PDF file can then be 
# created manually using the Markdown PDF extension of Visual
# Studio Code.

import os

# Identify the source files.
filenames = os.listdir()
srcfilenames = [filename for filename in filenames if filename[:2].isnumeric()]
srcfilenames.sort()

# Read and concatenate the source files.
lines = []
for srcfilename in srcfilenames:
    srcfile = open(srcfilename)
    lines.extend(srcfile.readlines())
    srcfile.close()
    
# Remove online navigation elements.
lines = [line for line in lines if (not line.startswith("| Prev")) and (not line.startswith("| [Prev]"))]

# Mention the opposite version of the report.
for lineindex in range(len(lines)):
    line = lines[lineindex]
    if line.find("A PDF version of this report") != -1:
        line = line.replace("A PDF version of this report", "An online version of this report")
    lines[lineindex] = line

# Reformat table of contents entries.
for lineindex in range(len(lines)):
    line = lines[lineindex]
    if line.find(".md)<br/>") != -1:
        i = line.find("](") + 2
        if line[i+3:].startswith("references") or line[i+3:].startswith("appendix"):
            line = line[:i] + line[i+3:]
        if line[i] == "0":
            line = line[:i] + line[i+1:]
        line = line[:i] + "#" + line[i:]
        line = "- " + line.replace(".md)<br/>", ")")        
    lines[lineindex] = line

# Write the output file.
outfilename = "central64-technical-report.md"
outfile = open(outfilename, "w")
for line in lines:
    outfile.write(line)
outfile.close()