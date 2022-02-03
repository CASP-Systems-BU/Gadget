import os

os.popen('./runmyeval.sh 121').read()

os.popen('./runmyeval.sh 122').read()

os.popen('./runmyeval.sh 123').read()

os.popen('python3 plot2.py result_121.txt result_122.txt result_123.txt ops').read()
os.popen('python3 plot2.py result_121.txt result_122.txt result_123.txt p99').read()



