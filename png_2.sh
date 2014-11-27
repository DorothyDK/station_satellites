#!/bin/bash
#echo `date` 
gnuplot << METKA

set term png truecolor small size 900 900 nocrop
set output "trajectory_2.png"
unset mouse
set palette
set palette color
set pm3d map
set palette defined
set palette model RGB
set cbrange [-0.5:0.5]
set palette defined ( -0.5 "red", -0.2 "red", 0 "yellow", 0.2 "blue", 0.5 "blue")

set size 1,1
set size ratio 0.7
set xrange [      -180.:     180.]
set yrange [      -90.:     90.]

set nokey 

set xlabel "Longitude, E"
set ylabel "Latitude, N"

set bmargin 0
set lmargin 0
set rmargin 0
set tmargin 0
set tics
set title "09.0-12.0 UT"

splot "datanew.dat" u 3:2:(0) w l lw 1,  "NiceWorld-180.DAT" u 1:2:(0) w l lc rgb "grey" lw 1, "station2.dat" u 3:2:(0) w p ps 1 pt 7
#replot

METKA
exit 0
