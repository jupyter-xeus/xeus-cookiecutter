#!/bin/bash


ENV_NAME=xeus_cookiecutter_test_env
DIR_NAME=xeus-lua

# try to remove env
conda deactivate 
conda env remove --name ${ENV_NAME}

# exit when any command fails
set -e

cookiecutter .. --no-input -f
mamba env create -f ../dev-requirements.yml   --name ${ENV_NAME}
eval "$(conda shell.bash hook)"
conda activate ${ENV_NAME}

cd ${DIR_NAME}
mkdir -p bld
cd bld
cmake .. \
     -DCMAKE_PREFIX_PATH=$CONDA_PREFIX \
     -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX \
     -Dxtl_DIR=$CONDA_PREFIX/share/cmake/xtl

make -j8

# remove env
conda deactivate
conda env remove --name ${ENV_NAME}