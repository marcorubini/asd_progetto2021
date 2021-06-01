#!/bin/sh

if [ "$#" -ne 4 ]; then
  echo 'Usage: run_all.sh solution checker input_dir output_dir'
  exit
fi

solution=$1
checker=$2
input=$3
output=$4

for i in $(seq 0 19); do
  echo $i
  /usr/bin/time -f "s: %e M: %M" $solution < $input/input$i.txt > $output/output$i.txt
  $checker $input/input$i.txt $output/output$i.txt 
  echo
done