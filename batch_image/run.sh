#!/bin/bash

mkdir -p /workspace
cd /workspace

git clone git@github.com:osak/ICFPC2020.git --depth=1 .
eval "$RUN_COMMAND"
