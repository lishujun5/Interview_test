#!/bin/bash
make -j4
make clean
if [ ! -d "profile" ];then
  mkdir profile
fi
./Problem_A_1 ./sample.config
./Problem_A_2 ./profile.config ./sample/sample_input.txt ./sample/your_sample_output.txt