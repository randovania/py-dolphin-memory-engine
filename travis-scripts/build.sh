#!/bin/bash

set -e

if [ -n "$PYTHON_VERSION" ]; then
    curl -fSL https://github.com/Kitware/CMake/releases/download/v3.18.2/cmake-3.18.2-Linux-x86_64.sh -o cmake.sh \
        && bash cmake.sh --skip-license --prefix=/usr/local/ \
        && rm -f cmake.sh

    echo "Adding $PYTHON_VERSION to path"
    export PATH="/opt/python/${PYTHON_VERSION}/bin:$PATH"
    python -m pip install auditwheel
fi

python -m pip install Cython
cmake --version
python setup.py bdist_wheel

if [ -n "$PYTHON_VERSION" ]; then
    python -m auditwheel repair --plat manylinux2014_x86_64 dist/*-linux_x86_64.whl -w dist
    rm dist/*-linux_x86_64.whl
fi

echo "Resulting files: "
ls -la dist/
