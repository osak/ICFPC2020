#!/bin/bash

./a.out galaxy.txt
exec gunicorn src.main:cpp