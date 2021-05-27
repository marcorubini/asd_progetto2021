#!/bin/sh

if [ "$#" -ne 3 ]; then
  echo 'Usage: run_all.sh solution input_dir output_dir'
  exit
fi

solution=$1
input=$2
output=$3

for i in $(seq 0 19); do
  echo $i
  /usr/bin/time -f "%e" $1 < $input/input$i.txt > $output/output$i.txt
done