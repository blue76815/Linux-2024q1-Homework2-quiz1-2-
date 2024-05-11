reset                                                                           
set xlabel 'data number'
set ylabel 'nanosecond'
set title 'perfomance comparison'
set term png enhanced font 'Times_New_Roman, 10'
set output 'qsort.png'
set xtics 0,100
set datafile separator ","
set key left

plot \
"data.csv" using 1:2 with linespoints linewidth 1 title "qsort", \
"data.csv" using 1:3 with linespoints linewidth 1 title "non-qsort"