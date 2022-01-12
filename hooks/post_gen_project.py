#!/usr/bin/env python
import os

with_wasm = "{{cookiecutter.with_wasm}}" == "yes"

if not with_wasm:
    os.remove("src/main_emscripten_kernel.cpp")
