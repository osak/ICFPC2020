#!/bin/bash

mkdir -p /workspace
cd /workspace

git clone git@github.com:osak/ICFPC2020.git --depth=1 .
cd "$WORK_DIR"
eval "$RUN_COMMAND"
