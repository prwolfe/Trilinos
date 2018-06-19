# This script can be used to load the appropriate environment for the
# GCC 4.9.3 Pull Request testing build on a Linux machine that has access to
# the SEMS NFS mount.

# usage: From ride or white - $ source PullRequestNVIDIA9.0.103TestingEnv.sh

# After the environment is no longer needed, it can be purged using
# $ module purge


module load devpack/openmpi/1.10.4/gcc/5.4.0/cuda/8.0.44
module swap openblas/0.2.19/gcc/5.4.0 netlib/3.8.0/gcc/5.4.0

module swap yamlcpp/0.3.0 yaml-cpp/20170104
if [ $? ]; then module load  yaml-cpp/20170104; fi

# translate from IBM modules to expectations from sems modules
export SEMS_PARMETIS_INCLUDE_PATH="${PARMETIS_ROOT:?}/include;${METIS_ROOT:?}/include"
export SEMS_PARMETIS_LIBRARY_PATH="${PARMETIS_ROOT:?}/lib;${METIS_ROOT:?}/lib"
export SEMS_HDF5_INCLUDE_PATH=${HDF5_ROOT:?}/include
export SEMS_HDF5_LIBRARY_PATH=${HDF5_ROOT:?}/lib
export SEMS_NETCDF_INCLUDE_PATH=${NETCDF_ROOT:?}/include
export SEMS_NETCDF_LIBRARY_PATH=${NETCDF_ROOT:?}/lib
export SEMS_SUPERLU_INCLUDE_PATH=${SUPERLU_ROOT:?}/include
export SEMS_SUPERLU_LIBRARY_PATH=${SUPERLU_ROOT:?}/lib
export SEMS_BOOST_INCLUDE_PATH=${BOOST_ROOT:?}/include
export SEMS_BOOST_LIBRARY_PATH=${BOOST_ROOT:?}/lib

# Set the nvcc_wrapper
# Get the Trilins base dir
SCRIPT_DIR=`dirname ${BASH_SOURCE[0]}`
TRILNOS_DIR=`readlink -f ${SCRIPT_DIR}/../../..`
export OMPI_CXX=$TRILNOS_DIR/packages/kokkos/bin/nvcc_wrapper
if [ ! -x "$OMPI_CXX" ]; then
    echo "No nvcc_wrapper found"
    return
fi
export OMPI_CC=`which gcc`
export OMPI_FC=`which gfortran`

# Use manually installed cmake and ninja to try to avoid module loading
# problems (see TRIL-208)
export PATH=/ascldap/users/rabartl/install/white-ride/cmake-3.11.2/bin:/ascldap/users/rabartl/install/white-ride/ninja-1.8.2/bin:$PATH

# Set MPI wrappers
export MPICC=`which mpicc`
export MPICXX=`which mpicxx`
export MPIF90=`which mpif90`

