#!/bin/bash

echo "------------------------------------------------------------"
echo "create xeus_cookiecutter-dev-requirements conda env"
echo "------------------------------------------------------------"
conda env create -f xeus_cookiecutter-dev-requirements.yml || exit 1

echo "------------------------------------------------------------"
echo "activate xeus_cookiecutter-dev-requirements conda env"
echo "------------------------------------------------------------"
source activate xeus_cookiecutter-dev-requirements || exit 1

echo "------------------------------------------------------------"
echo "run pytest"
echo "------------------------------------------------------------"
make test || exit 1

echo "------------------------------------------------------------"
echo "run coverage"
echo "------------------------------------------------------------"
make coverage-no-html || exit 1


echo "------------------------------------------------------------"
echo "make dist"
echo "------------------------------------------------------------"
make dist || exit 1

echo "------------------------------------------------------------"
echo "make clean"
echo "------------------------------------------------------------"
make clean || exit 1

echo "------------------------------------------------------------"
echo "make help"
echo "------------------------------------------------------------"
make help || exit 1


echo "------------------------------------------------------------"
echo "install via setup.py"
echo "------------------------------------------------------------"
python setup.py install || exit 1

