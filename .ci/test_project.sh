#!/bin/bash

# echo "------------------------------------------------------------"
# echo " initialize micromamba"
# echo "------------------------------------------------------------"
# micromamba shell init -s bash -p ~/micromamba
# mkdir -p ~/micromamba/pkgs/

# echo "------------------------------------------------------------"
# echo " create cookiecutter env"
# echo "------------------------------------------------------------"
# export MAMBA_ROOT_PREFIX=~/micromamba
# export MAMBA_EXE=$(pwd)/micromamba

# . $MAMBA_ROOT_PREFIX/etc/profile.d/mamba.sh
# micromamba create -f .ci/microenv.yml -y
# micromamba activate microenv

ENV_NAME=xeus_cookiecutter_test_env
PACKAGE_NAME=xeus-mylang
MAMBA_BIN=$HOME/micromamba-bin/micromamba


# exit when any command fails
set -e

echo "------------------------------------------------------------"
echo " use cookiecutter"
echo "------------------------------------------------------------"
cookiecutter  . --no-input -f
cd ${PACKAGE_NAME}
echo "micromambaLoc" ${micromambaLoc} $micromambaLoc
echo "------------------------------------------------------------"
echo " create env"
echo "------------------------------------------------------------"
$MAMBA_BIN env create -f environment-dev.yml  -y --name ${ENV_NAME}

echo "------------------------------------------------------------"
echo " activate env"
echo "------------------------------------------------------------"
micromamba activate ${ENV_NAME}

echo "------------------------------------------------------------"
echo " install cxx compiler"
echo "------------------------------------------------------------"
$MAMBA_BIN  install cxx-compiler -c conda-forge -y
mkdir -p bld
cd bld
cmake .. \
     -DCMAKE_PREFIX_PATH=$CONDA_PREFIX \
     -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX \
     -Dxtl_DIR=$CONDA_PREFIX/share/cmake/xtl

make -j8