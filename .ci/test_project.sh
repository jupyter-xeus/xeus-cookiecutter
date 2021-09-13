#!/bin/bash

echo "------------------------------------------------------------"
echo "config conda"
echo "------------------------------------------------------------"

conda config --set always_yes yes --set changeps1 no
echo "------------------------------------------------------------"
echo "install cookiecutter"
echo "------------------------------------------------------------"
conda install cookiecutter  mamba -c conda-forge

echo "------------------------------------------------------------"
echo "run bash test"
echo "------------------------------------------------------------"
cd tests
./bash_test.sh || exit 1

