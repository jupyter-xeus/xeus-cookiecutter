#!/usr/bin/env python
import os
import shutil


def remove(filepath):
    if os.path.isfile(filepath):
        os.remove(filepath)
    elif os.path.isdir(filepath):
        shutil.rmtree(filepath)


with_wasm = "{{cookiecutter.with_wasm}}" == "y"


if not wasm:
    remove("src/main_emscripten_kernel.cpp")
