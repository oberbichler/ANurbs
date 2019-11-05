set -e
yum -y remove cmake
/opt/python/${PYTHON_VERSION}/bin/pip install cmake
export PATH=$PATH:/opt/python/${PYTHON_VERSION}/bin/
/opt/python/${PYTHON_VERSION}/bin/pip wheel .
auditwheel repair anurbs-*-${PYTHON_VERSION}-*.whl
