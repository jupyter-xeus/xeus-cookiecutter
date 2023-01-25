..  Copyright (c) {% now 'utc', '%Y' %},    

   Distributed under the terms of the {{cookiecutter.open_source_license}}.  

   The full license is in the file LICENSE, distributed with this software.

Build and configuration
=======================

General Build Options
---------------------

Building the {{cookiecutter.library_name}} library
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``{{cookiecutter.package_name}}`` build supports the following options:

- ``{{cookiecutter.cmake_var_prefix}}_BUILD_SHARED``: Build the ``{{cookiecutter.library_name}}`` shared library. **Enabled by default**.
- ``{{cookiecutter.cmake_var_prefix}}_BUILD_STATIC``: Build the ``{{cookiecutter.library_name}}`` static library. **Enabled by default**.


- ``{{cookiecutter.cmake_var_prefix}}_USE_SHARED_XEUS``: Link with a `xeus` shared library (instead of the static library). **Enabled by default**.

Building the kernel
~~~~~~~~~~~~~~~~~~~

The package includes two options for producing a kernel: an executable ``{{cookiecutter.binary_name}}`` and a Python extension module, which is used to launch a kernel from Python.

- ``{{cookiecutter.cmake_var_prefix}}_BUILD_EXECUTABLE``: Build the ``{{cookiecutter.binary_name}}``  executable. **Enabled by default**.


If ``{{cookiecutter.cmake_var_prefix}}_USE_SHARED_{{cookiecutter.cmake_var_prefix}}`` is disabled, {{cookiecutter.binary_name}}  will be linked statically with ``{{cookiecutter.library_name}}``.

Building the Tests
~~~~~~~~~~~~~~~~~~

- ``{{cookiecutter.cmake_var_prefix}}_BUILD_TESTS ``: enables the tets  **Disabled by default**.

