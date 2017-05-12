#!/usr/bin/env python2.7

from distutils.core import setup
from Cython.Build   import cythonize

setup(name="Phits2dProcess", ext_modules=cythonize("Phits2dProcess.pyx"), )
