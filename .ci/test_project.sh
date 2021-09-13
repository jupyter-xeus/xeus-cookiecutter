#!/bin/bash

echo "------------------------------------------------------------"
echo "create dev-requirements conda env"
echo "------------------------------------------------------------"
conda env create -f dev-requirements.yml || exit 1

echo "------------------------------------------------------------"
echo "activate dev-requirements conda env"
echo "------------------------------------------------------------"
source activate dev-requirements || exit 1

echo "------------------------------------------------------------"
echo "run bash test"
echo "------------------------------------------------------------"
cd tests
./bash_test.sh || exit 1

