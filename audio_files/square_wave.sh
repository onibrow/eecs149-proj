#!/bin/bash


echo -ne '' > sqwv.txt

i=0
while [ $i -lt 1024 ]; do
  j=0
  while [ $j -lt 32 ]; do
    echo -ne '\x7F\xFF' >> sqwv.txt
    j=$(( $j+1 ))
  done
  j=0
  while [ $j -lt 32 ]; do
    echo -ne '\x00\x00' >> sqwv.txt
    j=$(( $j+1 ))
  done
  i=$(( $i+1 ))
done

i=0
while [ $i -lt 1024 ]; do
  j=0
  while [ $j -lt 32 ]; do
    echo -ne '\x07\xFF' >> sqwv.txt
    j=$(( $j+1 ))
  done
  j=0
  while [ $j -lt 32 ]; do
    echo -ne '\x00\x00' >> sqwv.txt
    j=$(( $j+1 ))
  done
  i=$(( $i+1 ))
done

i=0
while [ $i -lt 1024 ]; do
  j=0
  while [ $j -lt 32 ]; do
    echo -ne '\x7F\xFF' >> sqwv.txt
    j=$(( $j+1 ))
  done
  j=0
  while [ $j -lt 32 ]; do
    echo -ne '\x00\x00' >> sqwv.txt
    j=$(( $j+1 ))
  done
  i=$(( $i+1 ))
done

i=0
while [ $i -lt 1024 ]; do
  j=0
  while [ $j -lt 32 ]; do
    echo -ne '\x07\xFF' >> sqwv.txt
    j=$(( $j+1 ))
  done
  j=0
  while [ $j -lt 32 ]; do
    echo -ne '\x00\x00' >> sqwv.txt
    j=$(( $j+1 ))
  done
  i=$(( $i+1 ))
done

echo 'Done!'
