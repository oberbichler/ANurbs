set -e
yum -y remove cmake
/opt/python/${PYTHON_VERSION}/bin/pip install cmake
export PATH=$PATH:/opt/python/${PYTHON_VERSION}/bin/
/opt/python/${PYTHON_VERSION}/bin/python setup.py bdist_wheel
auditwheel repair ANurbs-*-${PYTHON_VERSION}-*.whl
