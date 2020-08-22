#!/bin/bash

set -e
set -x

curl -fSL https://github.com/Kitware/CMake/releases/download/v3.18.2/cmake-3.18.2-Linux-x86_64.sh -o cmake.sh \
    && bash cmake.sh --skip-license --prefix=/usr/local/ \
    && rm -f cmake.sh

echo "Adding $PYTHON_VERSION to path"
export PATH="/opt/python/${PYTHON_VERSION}/bin:$PATH"

cmake --version

python -m pip install --use-feature=2020-resolver auditwheel "setuptools>=44.0.0" "wheel>=0.34.2" "Cython>=0.29.21" "setuptools_scm[toml]>=3.4"
python setup.py bdist_wheel

python -m auditwheel repair --plat manylinux2014_x86_64 dist/*-linux_x86_64.whl -w dist
rm dist/*-linux_x86_64.whl

echo "Resulting files: "
ls -la dist/
