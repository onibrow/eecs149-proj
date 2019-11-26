#!/bin/bash

i=0

echo -ne '' > square_wave.txt

while [ $i -lt 256 ]; do
  j=0
  while [ $j -lt 16 ]; do
    echo -ne '\x7F\xFF' >> square_wave.txt
    j=$(( $j+1 ))
  done
  j=0
  while [ $j -lt 16 ]; do
    echo -ne '\x00\x00' >> square_wave.txt
    j=$(( $j+1 ))
  done
  i=$(( $i+1 ))
done
echo 'Done!'
