#!/bin/sh

cd ./app
python ./main.py "$@" || echo "run error code: $?"
