#!/usr/bin/env bash

cd $(dirname $0)

project_root=$(pwd)
file_name=main.cpp

clang++ $project_root/$file_name -o output; ./output

rm $project_root/output
