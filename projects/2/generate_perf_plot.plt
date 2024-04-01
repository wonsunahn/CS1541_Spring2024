# Plot configurations

set style line 1 lt rgb "dark-green" lw 20 ps 5 pt 1
set style line 2 lt rgb "green" lw 20 ps 5 pt 2
set style line 3 lt rgb "light-green" lw 20 ps 5 pt 3
set style line 4 lt rgb "dark-magenta" lw 20 ps 5 pt 1
set style line 5 lt rgb "magenta" lw 20 ps 5 pt 2
set style line 6 lt rgb "light-magenta" lw 20 ps 5 pt 3
set title "IPC" 
set xlabel "data structure size (items)"
set ylabel "IPC"
set xrange [ 0 : * ] noreverse writeback
set yrange [ * : * ] noreverse writeback
set y2range [ * : * ] noreverse writeback
set cbrange [ * : * ] noreverse writeback
set rrange [ * : * ] noreverse writeback
set xtics 0,5000,30000
set mxtics 5
# Legend position
set key outside;
set key center bottom;

# Render plot
set terminal pdf transparent enhanced font "arial,10" fontscale 5.0 size 90, 45

if (!exists("inputFile")) inputFile='IPC.dat'
if (!exists("outputFile")) outputFile='IPC.pdf'
set output outputFile
plot inputFile using 1:2 with linespoints linestyle 1 t "array.wb-dl1-32k-l2-128k-64b", inputFile using 1:3 with linespoints linestyle 2 t "array.wb-dl1-32k-l2-128k-8b", inputFile using 1:4 with linespoints linestyle 3 t "array.wt-dl1-32k-l2-128k-64b", inputFile using 1:5 with linespoints linestyle 4 t "lined-list.wb-dl1-32k-l2-128k-64b", inputFile using 1:6 with linespoints linestyle 5 t "linked-list.wb-dl1-32k-l2-128k-8b", inputFile using 1:7 with linespoints linestyle 6 t "linked-list.wt-dl1-32k-l2-128k-64b"
unset out

