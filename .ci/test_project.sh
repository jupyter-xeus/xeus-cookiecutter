#!/bin/bash

echo "------------------------------------------------------------"
echo "install cookiecutter"
echo "------------------------------------------------------------"
conda install cookiecutter  -c conda-forge

echo "------------------------------------------------------------"
echo "run bash test"
echo "------------------------------------------------------------"
cd tests
./bash_test.sh || exit 1

