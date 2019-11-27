#!/bin/bash

i=0

echo -ne '' > square_wave.txt
echo -ne '' > square_wave_raw.txt

while [ $i -lt 256 ]; do
  j=0
  while [ $j -lt 16 ]; do
    echo -ne '\x7F\xFF' >> square_wave.txt
    echo -ne '7FFF' >> square_wave_raw.txt
    j=$(( $j+1 ))
  done
  j=0
  while [ $j -lt 16 ]; do
    echo -ne '\x80\x00' >> square_wave.txt
    echo -ne '8000' >> square_wave_raw.txt
    j=$(( $j+1 ))
  done
  i=$(( $i+1 ))
done
echo 'Done!'
