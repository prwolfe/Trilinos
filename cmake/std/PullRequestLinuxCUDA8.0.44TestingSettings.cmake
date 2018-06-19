# This file contains the options needed to both run the pull request testing
# for Trilinos for the Linux GCC 4.9.3 pull request testing builds, and to reproduce
# the errors reported by those builds. Prior to using this this file, the
# appropriate set of SEMS modules must be loaded and accessible through the
# SEMS NFS mount. (See the sems/PullRequestGCC*TestingEnv.sh files.)

# Usage: cmake -C PullRequestLinuxGCC4.9.3TestingSettings.cmake

# Misc options typically added by CI testing mode in TriBITS

# Use the below option only when submitting to the dashboard
#set (CTEST_USE_LAUNCHERS ON CACHE BOOL "Set by default for PR testing")

set (TPL_ENABLE_CUDA ON CACHE BOOL "Set by default for PR testing")
set (Trilinos_CXX11_FLAGS "-std=c++11 -expt-extended-lambda" CACHE STRING "Set by default for PR testing")

set (MPI_EXEC_PRE_NUMPROCS_FLAGS "--mca;orte_abort_on_non_zero_status;0" CACHE STRING "Set by default for PR testing")
set (MPI_EXEC_POST_NUMPROCS_FLAGS "-map-by;socket:PE=4" CACHE STRING "Set by default for PR testing")

set (BLAS_INCLUDE_DIRS "$ENV{BLAS_ROOT}/include" CACHE FILEPATH "Set by default for PR testing")
set (BLAS_LIBRARY_DIRS "$ENV{BLAS_ROOT}/lib" CACHE FILEPATH "Set by default for PR testing")
set (TPL_BLAS_LIBRARIES "-L$ENV{BLAS_ROOT}/lib;-lblas;-lgfortran;-lgomp;-lm" CACHE FILEPATH "Set by default for PR testing")

set (LAPACK_INCLUDE_DIRS "$ENV{LAPACK_ROOT}/include" CACHE FILEPATH "Set by default for PR testing")
set (LAPACK_LIBRARY_DIRS "$ENV{LAPACK_ROOT}/lib" CACHE FILEPATH "Set by default for PR testing")
set (TPL_LAPACK_LIBRARIES "-L$ENV{LAPACK_ROOT}/lib;-llapack;-lgfortran;-lgomp" CACHE FILEPATH "Set by default for PR testing")

set(Tpetra_INST_SERIAL ON CACHE BOOL "Set by default for PR testing")
# note: mortar uses serial mode no matter what so we need to instantiate this to get it's examples to work
set (Kokkos_ENABLE_Cuda_UVM ON CACHE BOOL "Set by default for PR testing")

## Test disables
# The Zoltan cpp driver has compile time issues with the Test_Flags struct
set (Zoltan_ENABLE_CPPDRIVER OFF CACHE BOOL "Set by default for PR testing")

set (TrilinosCouplings_IntrepidPoisson_Pamgen_EpetraAztecOO_EXE_DISABLE ON CACHE BOOL "Set by default for PR testing")
set (TrilinosCouplings_IntrepidPoisson_Pamgen_Epetra_EXE_DISABLE ON CACHE BOOL "Set by default for PR testing")

set (TPL_HDF5_LIBRARIES "-L$ENV{HDF5_ROOT}/lib;$ENV{HDF5_ROOT}/lib/libhdf5_hl.a;$ENV{HDF5_ROOT}/lib/libhdf5.a;-lz;-ldl" CACHE FILEPATH "Set by default for PR testing")
SET (TPL_Netcdf_LIBRARIES "-L$ENV{BOOST_ROOT}/lib;-L$ENV{NETCDF_ROOT}/lib;-L$ENV{NETCDF_ROOT}/lib;-L$ENV{PNETCDF_ROOT}/lib;-L$ENV{HDF5_ROOT}/lib;$ENV{BOOST_ROOT}/lib/libboost_program_options.a;$ENV{BOOST_ROOT}/lib/libboost_system.a;$ENV{NETCDF_ROOT}/lib/libnetcdf.a;$ENV{PNETCDF_ROOT}/lib/libpnetcdf.a;$ENV{HDF5_ROOT}/lib/libhdf5_hl.a;$ENV{HDF5_ROOT}/lib/libhdf5.a;-lz;-ldl" CACHE FILEPATH "Set by default for PR testing")

include("${CMAKE_CURRENT_LIST_DIR}/PullRequestLinuxCommonTestingSettings.cmake")
