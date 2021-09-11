# ![{{cookiecutter.package_name}}](docs/source/xeus-python.svg)

[![Azure Pipelines](https://dev.azure.com/jupyter-xeus/jupyter-xeus/_apis/build/status/jupyter-xeus.xeus-python?branchName=master)](https://dev.azure.com/jupyter-xeus/jupyter-xeus/_build/latest?definitionId=2&branchName=master)
[![Appveyor](https://ci.appveyor.com/api/projects/status/vy6rhqdw24pjduip?svg=true)](https://ci.appveyor.com/project/jupyter-xeus/xeus-python)
[![Documentation Status](http://readthedocs.org/projects/xeus-python/badge/?version=latest)](https://xeus-python.readthedocs.io/en/latest/?badge=latest)
[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/jupyter-xeus/xeus-python/stable?urlpath=/lab/tree/notebooks/xeus-python.ipynb)
[![Join the Gitter Chat](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/QuantStack/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

`{{cookiecutter.conda_package_name}}` is a Jupyter kernel for {{cookiecutter.language}} based on the native implementation of the
Jupyter protocol [xeus](https://github.com/jupyter-xeus/xeus).

## Installation

{{cookiecutter.package_name}} has been packaged for the mamba (or conda) package manager.

To ensure that the installation works, it is preferable to install `{{cookiecutter.conda_package_name}}` in a
fresh environment. It is also needed to use a
[miniforge](https://github.com/conda-forge/miniforge#mambaforge) or
[miniconda](https://conda.io/miniconda.html) installation because with the full
[anaconda](https://www.anaconda.com/) you may have a conflict with the `zeromq` library
which is already installed in the anaconda distribution.

The safest usage is to create an environment named `{{cookiecutter.conda_package_name}}`

```bash
mamba create -n  `{{cookiecutter.conda_package_name}}`
source activate  `{{cookiecutter.conda_package_name}}`
```

### Installing from conda-forge

Then you can install in this environment `{{cookiecutter.conda_package_name}}` and its dependencies

```bash
mamba install`{{cookiecutter.conda_package_name}}` notebook -c conda-forge
```

### Installing from source

Or you can install it from the sources, you will first need to install dependencies

```bash
mamba install cmake xeus nlohmann_json cppzmq xtl pybind11 pybind11_json ipython debugpy jupyterlab -c conda-forge
```

Then you can compile the sources (replace `$CONDA_PREFIX` with a custom installation
prefix if need be)

```bash
mkdir build && cd build
cmake .. -D CMAKE_PREFIX_PATH=$CONDA_PREFIX -D CMAKE_INSTALL_PREFIX=$CONDA_PREFIX -D CMAKE_INSTALL_LIBDIR=lib -D PYTHON_EXECUTABLE=`which python`
make && make install
```

## Trying it online

To try out xeus-python interactively in your web browser, just click on the binder link:

[![Binder](binder-logo.svg)](https://mybinder.org/v2/gh/jupyter-xeus/xeus-python/stable?urlpath=/lab/tree/notebooks/xeus-python.ipynb)



## Documentation

To get started with using `xeus-python`, check out the full documentation

http://xeus-python.readthedocs.io

## What are the advantages of using xeus-python over ipykernel (IPython kernel)?

Check-out this blog post for the answer:
https://blog.jupyter.org/a-new-python-kernel-for-jupyter-fcdf211e30a8. Long story short:

- xeus-python is a lot lighter than ipykernel, which makes it a lot easier to implement
  new features on top of it.
- xeus-python already works with the **Jupyter Lab debugger**:
  https://github.com/jupyterlab/debugger
- xeus-based kernels are more versatile in that one can overload e.g. the concurrency
  model. This is something that Kitware’s SlicerJupyter project takes advantage of to
  integrate with the Qt event loop of their Qt-based desktop application.

## Dependencies

`xeus-python` depends on

- [xeus](https://github.com/jupyter-xeus/xeus)
- [xtl](https://github.com/xtensor-stack/xtl)
- [pybind11](https://github.com/pybind/pybind11)
- [pybind11_json](https://github.com/pybind/pybind11_json)
- [nlohmann_json](https://github.com/nlohmann/json)



## Contributing

See [CONTRIBUTING.md](./CONTRIBUTING.md) to know how to contribute and set up a
development environment.

## License

We use a shared copyright model that enables all contributors to maintain the copyright
on their contributions.

This software is licensed under the `{{cookiecutter.open_source_license}}`. See the [LICENSE](LICENSE)
file for details.
