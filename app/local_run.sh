#!/bin/sh -x

set -ue

cd `dirname $0`
g++ -Wall main.cpp
python3 ../local_battle/local_battle.py ./a.out kenkoooo_test ./a.out kenkoooo_test
