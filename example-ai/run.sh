#/bin/bash

g++ a.cc
./a.out $AWS_BATCH_JOB_ARRAY_INDEX > out.txt
echo "output:"
cat out.txt