#!/bin/bash

echo "DEBUG GATHER"
python $1 -f $2 > report.txt
echo "ended" > report.txt
cat $2 > scheduler.json