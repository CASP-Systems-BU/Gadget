#!/bin/bash

./runmyeval.sh 121
./runmyeval.sh 122
./runmyeval.sh 123
python3 plot2.py result_121.txt result_122.txt result_123.txt ops
python3 plot2.py result_121.txt result_122.txt result_123.txt p99
