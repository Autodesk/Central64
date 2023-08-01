| [Prev](11-references.md) | Next |
## Appendix

The tables below list the non-normalized average path length and runtime results for all 200 variations of path planning methods tested on the *Dragon Age: Origins* benchmark set. The path lengths are in grid spacings and the runtimes are in microseconds. Tests were run on a 2.7GHz Intel Core i7 laptop with 16GB of RAM, the same machine used by [Goldstein et al. (2022)](https://jair.org/index.php/jair/article/view/13544). For the heuristic search methods, the absolute shortest possible average length is 185.778246.

### A* Search

|Neighborhood|Search Method|Smoothing|Path Length|Runtime|
|:----------:|:-----------:|:-------:|:--------:|:-----:|
|4|Regular A* Search|Greedy|193.365718|534.387950|
|8|Regular A* Search|Greedy|187.589861|826.688916|
|16|Regular A* Search|Greedy|186.271375|1626.100770|
|32|Regular A* Search|Greedy|185.984016|2881.977007|
|64|Regular A* Search|Greedy|185.879095|4973.799355|
|4|Central A* Search|Greedy|186.671385|1217.876808|
|8|Central A* Search|Greedy|185.986902|1197.944923|
|16|Central A* Search|Greedy|185.847056|1989.447834|
|32|Central A* Search|Greedy|185.807597|3275.682891|
|64|Central A* Search|Greedy|185.792561|5494.900880|
|4|Regular A* Search|Tentpole|191.092794|546.832701|
|8|Regular A* Search|Tentpole|186.628553|847.940695|
|16|Regular A* Search|Tentpole|185.959140|1636.882757|
|32|Regular A* Search|Tentpole|185.840904|2878.747123|
|64|Regular A* Search|Tentpole|185.801373|4967.486637|
|4|Central A* Search|Tentpole|185.974571|1293.145968|
|8|Central A* Search|Tentpole|185.819156|1247.897377|
|16|Central A* Search|Tentpole|185.793087|2008.612702|
|32|Central A* Search|Tentpole|185.784785|3302.039216|
|64|Central A* Search|Tentpole|185.779853|5551.712036|

### Jump Point Search (JPS)

|Neighborhood|Search Method|Smoothing|Path Length|Runtime|
|:----------:|:-----------:|:-------:|:--------:|:-----:|
|4|Regular JPS|Greedy|201.039027|408.654190|
|8|Regular JPS|Greedy|188.477175|321.472344|
|16|Regular JPS|Greedy|186.243174|627.678783|
|32|Regular JPS|Greedy|185.918231|1180.227494|
|64|Regular JPS|Greedy|185.833812|2425.068509|
|4|Central JPS|Greedy|186.671385|1084.345122|
|8|Central JPS|Greedy|185.986902|697.907750|
|16|Central JPS|Greedy|185.847056|933.688681|
|32|Central JPS|Greedy|185.807597|1517.915286|
|64|Central JPS|Greedy|185.792561|2786.210819|
|4|Regular JPS|Tentpole|197.096915|418.021204|
|8|Regular JPS|Tentpole|187.170755|327.009705|
|16|Regular JPS|Tentpole|186.038859|634.523693|
|32|Regular JPS|Tentpole|185.863202|1180.834018|
|64|Regular JPS|Tentpole|185.810860|2419.373892|
|4|Central JPS|Tentpole|185.974571|1106.927825|
|8|Central JPS|Tentpole|185.819156|702.821126|
|16|Central JPS|Tentpole|185.793087|923.437070|
|32|Central JPS|Tentpole|185.784785|1472.852775|
|64|Central JPS|Tentpole|185.779853|2802.757366|

### Bounded Jump Point Search

|Neighborhood|Search Method|Smoothing|Path Length|Runtime|
|:----------:|:-----------:|:-------:|:--------:|:-----:|
|4|Regular Bounded JPS|Greedy|200.433325|347.863703|
|8|Regular Bounded JPS|Greedy|188.430357|286.518214|
|16|Regular Bounded JPS|Greedy|186.237935|526.341604|
|32|Regular Bounded JPS|Greedy|185.916391|973.875580|
|64|Regular Bounded JPS|Greedy|185.833489|1956.143089|
|4|Central Bounded JPS|Greedy|186.671385|1021.085621|
|8|Central Bounded JPS|Greedy|185.986902|693.019877|
|16|Central Bounded JPS|Greedy|185.847056|814.974841|
|32|Central Bounded JPS|Greedy|185.807597|1245.191221|
|64|Central Bounded JPS|Greedy|185.792561|2293.949523|
|4|Regular Bounded JPS|Tentpole|196.551539|358.722630|
|8|Regular Bounded JPS|Tentpole|187.140066|293.213547|
|16|Regular Bounded JPS|Tentpole|186.035377|530.480487|
|32|Regular Bounded JPS|Tentpole|185.862085|968.245246|
|64|Regular Bounded JPS|Tentpole|185.810589|1960.168195|
|4|Central Bounded JPS|Tentpole|185.974571|1027.304275|
|8|Central Bounded JPS|Tentpole|185.819156|663.766774|
|16|Central Bounded JPS|Tentpole|185.793087|815.973386|
|32|Central Bounded JPS|Tentpole|185.784785|1250.654212|
|64|Central Bounded JPS|Tentpole|185.779853|2339.332641|

### Mixed A* Search

|Neighborhood|Search Method|Smoothing|Path Length|Runtime|
|:----------:|:-----------:|:-------:|:--------:|:-----:|
|4|Regular Mixed A* Search|Greedy|191.597298|914.615398|
|8|Regular Mixed A* Search|Greedy|187.961801|1192.996757|
|16|Regular Mixed A* Search|Greedy|186.429905|2681.311567|
|32|Regular Mixed A* Search|Greedy|186.021187|4695.020902|
|64|Regular Mixed A* Search|Greedy|185.889545|8335.502693|
|4|Central Mixed A* Search|Greedy|186.671385|1655.436572|
|8|Central Mixed A* Search|Greedy|185.986902|1610.217601|
|16|Central Mixed A* Search|Greedy|185.847056|3071.352716|
|32|Central Mixed A* Search|Greedy|185.807597|5216.238722|
|64|Central Mixed A* Search|Greedy|185.792561|9029.816636|
|4|Regular Mixed A* Search|Tentpole|189.139226|929.717603|
|8|Regular Mixed A* Search|Tentpole|186.732358|1197.940252|
|16|Regular Mixed A* Search|Tentpole|185.984840|2687.409610|
|32|Regular Mixed A* Search|Tentpole|185.842622|4719.345070|
|64|Regular Mixed A* Search|Tentpole|185.801543|8390.656742|
|4|Central Mixed A* Search|Tentpole|185.974571|1628.300374|
|8|Central Mixed A* Search|Tentpole|185.819156|1619.197273|
|16|Central Mixed A* Search|Tentpole|185.793087|3130.666958|
|32|Central Mixed A* Search|Tentpole|185.784785|5204.472437|
|64|Central Mixed A* Search|Tentpole|185.779853|9104.802187|

### Mixed Jump Point Search

|Neighborhood|Search Method|Smoothing|Path Length|Runtime|
|:----------:|:-----------:|:-------:|:--------:|:-----:|
|4|Regular Mixed JPS|Greedy|200.396710|413.707733|
|8|Regular Mixed JPS|Greedy|188.423878|377.340498|
|16|Regular Mixed JPS|Greedy|186.242714|774.322450|
|32|Regular Mixed JPS|Greedy|185.916234|1352.991110|
|64|Regular Mixed JPS|Greedy|185.833216|2669.136886|
|4|Central Mixed JPS|Greedy|186.671385|1108.899596|
|8|Central Mixed JPS|Greedy|185.986902|765.950713|
|16|Central Mixed JPS|Greedy|185.847056|1073.335710|
|32|Central Mixed JPS|Greedy|185.807597|1649.392664|
|64|Central Mixed JPS|Greedy|185.792561|3060.202022|
|4|Regular Mixed JPS|Tentpole|196.558070|423.913765|
|8|Regular Mixed JPS|Tentpole|187.128776|383.347960|
|16|Regular Mixed JPS|Tentpole|186.037636|783.730371|
|32|Regular Mixed JPS|Tentpole|185.861571|1353.814327|
|64|Regular Mixed JPS|Tentpole|185.810542|2669.384104|
|4|Central Mixed JPS|Tentpole|185.974571|1094.356620|
|8|Central Mixed JPS|Tentpole|185.819156|755.812120|
|16|Central Mixed JPS|Tentpole|185.793087|1081.203811|
|32|Central Mixed JPS|Tentpole|185.784785|1658.353979|
|64|Central Mixed JPS|Tentpole|185.779853|3047.275998|

### Dijkstra Search

|Neighborhood|Search Method|Smoothing|Path Length|Runtime|
|:----------:|:-----------:|:-------:|:--------:|:-----:|
|4|Regular Dijkstra Search|Greedy|157.251873|83.935235|
|8|Regular Dijkstra Search|Greedy|154.188250|97.630645|
|16|Regular Dijkstra Search|Greedy|152.511089|184.842748|
|32|Regular Dijkstra Search|Greedy|152.116603|348.246898|
|64|Regular Dijkstra Search|Greedy|151.992054|649.513103|
|4|Central Dijkstra Search|Greedy|152.612682|603.239848|
|8|Central Dijkstra Search|Greedy|152.055011|387.013216|
|16|Central Dijkstra Search|Greedy|151.945177|391.497509|
|32|Central Dijkstra Search|Greedy|151.914072|537.626518|
|64|Central Dijkstra Search|Greedy|151.902037|832.434908|
|4|Regular Dijkstra Search|Tentpole|155.144687|94.182410|
|8|Regular Dijkstra Search|Tentpole|152.982804|105.024445|
|16|Regular Dijkstra Search|Tentpole|152.091715|189.579379|
|32|Regular Dijkstra Search|Tentpole|151.951272|351.292129|
|64|Regular Dijkstra Search|Tentpole|151.912229|644.473922|
|4|Central Dijkstra Search|Tentpole|152.045678|606.499334|
|8|Central Dijkstra Search|Tentpole|151.919454|386.555890|
|16|Central Dijkstra Search|Tentpole|151.901590|407.713721|
|32|Central Dijkstra Search|Tentpole|151.895579|531.521951|
|64|Central Dijkstra Search|Tentpole|151.891721|833.537815|

### Canonical Dijkstra Search (CDS)

|Neighborhood|Search Method|Smoothing|Path Length|Runtime|
|:----------:|:-----------:|:-------:|:--------:|:-----:|
|4|Regular CDS|Greedy|163.785354|49.419697|
|8|Regular CDS|Greedy|154.021722|40.695473|
|16|Regular CDS|Greedy|152.263140|67.386703|
|32|Regular CDS|Greedy|151.999688|133.528896|
|64|Regular CDS|Greedy|151.934463|282.740716|
|4|Central CDS|Greedy|152.612682|568.013680|
|8|Central CDS|Greedy|152.055011|327.128106|
|16|Central CDS|Greedy|151.945177|270.731337|
|32|Central CDS|Greedy|151.914072|311.776342|
|64|Central CDS|Greedy|151.902037|481.571763|
|4|Regular CDS|Tentpole|160.698606|62.099451|
|8|Regular CDS|Tentpole|152.982259|45.466031|
|16|Regular CDS|Tentpole|152.096336|69.221868|
|32|Regular CDS|Tentpole|151.956872|135.285486|
|64|Regular CDS|Tentpole|151.916566|284.632733|
|4|Central CDS|Tentpole|152.045678|569.287323|
|8|Central CDS|Tentpole|151.919454|326.639556|
|16|Central CDS|Tentpole|151.901590|270.704879|
|32|Central CDS|Tentpole|151.895579|319.175782|
|64|Central CDS|Tentpole|151.891721|488.185994|

### Bounded Canonical Dijkstra Search

|Neighborhood|Search Method|Smoothing|Path Length|Runtime|
|:----------:|:-----------:|:-------:|:--------:|:-----:|
|4|Regular Bounded CDS|Greedy|163.734843|53.116133|
|8|Regular Bounded CDS|Greedy|153.997510|45.630085|
|16|Regular Bounded CDS|Greedy|152.255571|71.991570|
|32|Regular Bounded CDS|Greedy|151.997883|138.643241|
|64|Regular Bounded CDS|Greedy|151.933452|288.416628|
|4|Central Bounded CDS|Greedy|152.612682|575.341513|
|8|Central Bounded CDS|Greedy|152.055011|332.383648|
|16|Central Bounded CDS|Greedy|151.945177|276.129461|
|32|Central Bounded CDS|Greedy|151.914072|317.685700|
|64|Central Bounded CDS|Greedy|151.902037|478.414886|
|4|Regular Bounded CDS|Tentpole|160.645438|61.299084|
|8|Regular Bounded CDS|Tentpole|152.961026|50.510666|
|16|Regular Bounded CDS|Tentpole|152.091280|74.255481|
|32|Regular Bounded CDS|Tentpole|151.955915|139.588242|
|64|Regular Bounded CDS|Tentpole|151.915896|289.984725|
|4|Central Bounded CDS|Tentpole|152.045678|574.648894|
|8|Central Bounded CDS|Tentpole|151.919454|333.728516|
|16|Central Bounded CDS|Tentpole|151.901590|276.993713|
|32|Central Bounded CDS|Tentpole|151.895579|318.847111|
|64|Central Bounded CDS|Tentpole|151.891721|480.248572|

### Mixed Dijkstra Search

|Neighborhood|Search Method|Smoothing|Path Length|Runtime|
|:----------:|:-----------:|:-------:|:--------:|:-----:|
|4|Regular Mixed Dijkstra Search|Greedy|156.693050|129.041187|
|8|Regular Mixed Dijkstra Search|Greedy|154.187286|157.753866|
|16|Regular Mixed Dijkstra Search|Greedy|152.510867|326.545713|
|32|Regular Mixed Dijkstra Search|Greedy|152.116534|583.580117|
|64|Regular Mixed Dijkstra Search|Greedy|151.992024|1075.996507|
|4|Central Mixed Dijkstra Search|Greedy|152.612682|646.726533|
|8|Central Mixed Dijkstra Search|Greedy|152.055011|453.257861|
|16|Central Mixed Dijkstra Search|Greedy|151.945177|533.009315|
|32|Central Mixed Dijkstra Search|Greedy|151.914072|766.801755|
|64|Central Mixed Dijkstra Search|Greedy|151.902037|1257.514665|
|4|Regular Mixed Dijkstra Search|Tentpole|154.581298|137.759488|
|8|Regular Mixed Dijkstra Search|Tentpole|152.982715|166.405091|
|16|Regular Mixed Dijkstra Search|Tentpole|152.091707|332.616671|
|32|Regular Mixed Dijkstra Search|Tentpole|151.951266|586.751369|
|64|Regular Mixed Dijkstra Search|Tentpole|151.912223|1073.546127|
|4|Central Mixed Dijkstra Search|Tentpole|152.045678|660.254336|
|8|Central Mixed Dijkstra Search|Tentpole|151.919454|448.443992|
|16|Central Mixed Dijkstra Search|Tentpole|151.901590|532.594152|
|32|Central Mixed Dijkstra Search|Tentpole|151.895579|766.372283|
|64|Central Mixed Dijkstra Search|Tentpole|151.891721|1267.957524|

### Mixed Canonical Dijkstra Search

|Neighborhood|Search Method|Smoothing|Path Length|Runtime|
|:----------:|:-----------:|:-------:|:--------:|:-----:|
|4|Regular Mixed CDS|Greedy|163.687226|60.396260|
|8|Regular Mixed CDS|Greedy|153.996164|54.694633|
|16|Regular Mixed CDS|Greedy|152.261676|86.865039|
|32|Regular Mixed CDS|Greedy|151.997833|167.970011|
|64|Regular Mixed CDS|Greedy|151.933743|357.694420|
|4|Central Mixed CDS|Greedy|152.612682|586.939997|
|8|Central Mixed CDS|Greedy|152.055011|341.734454|
|16|Central Mixed CDS|Greedy|151.945177|289.804511|
|32|Central Mixed CDS|Greedy|151.914072|345.511306|
|64|Central Mixed CDS|Greedy|151.902037|549.027167|
|4|Regular Mixed CDS|Tentpole|160.605347|68.535977|
|8|Regular Mixed CDS|Tentpole|152.966280|59.410840|
|16|Regular Mixed CDS|Tentpole|152.095247|88.370204|
|32|Regular Mixed CDS|Tentpole|151.955045|168.158011|
|64|Regular Mixed CDS|Tentpole|151.916153|357.132278|
|4|Central Mixed CDS|Tentpole|152.045678|582.268455|
|8|Central Mixed CDS|Tentpole|151.919454|341.649351|
|16|Central Mixed CDS|Tentpole|151.901590|290.972385|
|32|Central Mixed CDS|Tentpole|151.895579|348.633828|
|64|Central Mixed CDS|Tentpole|151.891721|548.413355|

| [Prev](11-references.md) | Next |
