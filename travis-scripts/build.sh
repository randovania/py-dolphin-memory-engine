#!/bin/bash

set -e

if [ -n "$PYTHON_VERSION" ]; then
    echo "Adding $PYTHON_VERSION to path"
    export PATH="/opt/python/${PYTHON_VERSION}/bin:$PATH"
    python -m pip install auditwheel
fi

python -m pip install Cython
python setup.py bdist_wheel

if [ -n "$PYTHON_VERSION" ]; then
    python -m auditwheel repair --plat manylinux2010_x86_64 dist/*-linux_x86_64.whl -w dist
    rm dist/*-linux_x86_64.whl
fi

echo "Resulting files: "
ls -la dist/