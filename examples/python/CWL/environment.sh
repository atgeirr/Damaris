# Install anaconda/miniconda; install xsdcxx from system package manager
conda create -n across python=3.8
conda activate across
conda install dask mpi4py boost xerces-c nodejs
conda install -c conda-forge cppunit
pip install streamflow

cd $HOME/repos/damaris
export CMAKE_INSTALL_PREFIX=/home/$USER/opt/damaris
export PYTHON_USER_SITE=$(python3 -m site --user-site)
export MPICXX_PATH=/usr/bin/mpicxx
export MPICC_PATH=/usr/bin/mpicc

cmake . -DCMAKE_INSTALL_PREFIX:PATH=$CMAKE_INSTALL_PREFIX       -DCMAKE_BUILD_TYPE=Debug       -DCMAKE_CXX_COMPILER=$MPICXX_PATH       -DCMAKE_C_COMPILER=$MPICC_PATH       -DENABLE_TESTS=OFF       -DENABLE_EXAMPLES=ON       -DENABLE_PYTHON=ON       -DBUILD_SHARED_LIBS=ON       -DENABLE_FORTRAN=OFF       -DGENERATE_MODEL=ON       -DENABLE_HDF5=OFF        -DENABLE_VISIT=OFF       -DENABLE_CATALYST=OFF       -DENABLE_PYTHONMOD=ON -DPYTHON_MODULE_INSTALL_PATH=/home/$USER/opt/damarispy -DCMAKE_PREFIX_PATH=$CONDA_PREFIX

make -j8
make install