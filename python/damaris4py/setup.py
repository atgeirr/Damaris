from setuptools import setup

# python setup.py sdist  # make tar 
# python setup.py build
# python setup.py install
# python setup.py develop

 setup(
   name='damaris4py',
   version='0.1.0',
   author='Joshua Bowden',
   author_email='joshua-charles.bowden@inria.fr',
   packages=['damaris4py', 'damaris4py.dask'],
   scripts=[''],
   url='http://pypi.python.org/pypi/damaris4py/',
   license='LICENSE.txt',
   description='A package to wrap functionality of the Damaris library for asynchronous I/O and visualisation of MPI based simulations',
   long_description=open('README.md').read(),
   install_requires=[
       "mpi4py"
   ],
)
