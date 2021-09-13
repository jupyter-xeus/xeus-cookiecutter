#!/bin/bash

# echo "------------------------------------------------------------"
# echo "create cookiecutter-dev-requirements conda env"
# echo "------------------------------------------------------------"
# conda env create -f cookiecutter-dev-requirements.yml || exit 1

# echo "------------------------------------------------------------"
# echo "activate cookiecutter-dev-requirements conda env"
# echo "------------------------------------------------------------"
# source activate cookiecutter-dev-requirements || exit 1

echo "------------------------------------------------------------"
echo "run bash test"
echo "------------------------------------------------------------"
cd tests
./bash_test || exit 1

