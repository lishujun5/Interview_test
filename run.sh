#!/bin/bash
make -j4
make clean
if [ ! -d "profile" ];then
  mkdir profile
fi

start_project0=$(date +%s.%N)
./Problem_A_1 ./sample.config
dur=$(echo "$(date +%s.%N) - $start_project0" | bc)
printf "training sample.config, Execution time: %.6f seconds\n" $dur

start_project1=$(date +%s.%N)
./Problem_A_1 ./profile.config
dur=$(echo "$(date +%s.%N) - $start_project1" | bc)
printf "training profile.config, Execution time: %.6f seconds\n" $dur

# delete trained profile
if [ -d "profile" ];then
  echo "delete trained profile for next task"
  rm -rf profile/* || exit -1
fi

start_project2=$(date +%s.%N)
./Problem_A_2 ./profile.config ./sample/sample_input.txt ./sample/your_sample_output.txt
dur=$(echo "$(date +%s.%N) - $start_project2" | bc)
printf "testing, Execution time: %.6f seconds\n" $dur