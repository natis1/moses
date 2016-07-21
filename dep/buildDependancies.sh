#!/usr/bin/env bash
#
# Attain Dependancies And Mutualities
# By Eli Stone (July 2016)
#
# This script is designed to download the required libraries to assemble
# exodusII files. This script downloads SEACAS and its dependancies and
# automatically compiles it so it will work with MOSES.
#
#
#
# This software is distributed under the GNU General Public License.
# A copy of this license should be included with this file.
#
#
#
# This script is based on the
# Goma dependency build script built by the following individuals:
#
# 2014, July: Created by Cory Miner based off of notes by Scott Roberts
#
# 2014: Weston Ortiz modified to include blacs, scalapack and mumps
# support in trilinos
#
# 2015, February: Andrew Cochrane modified to handle recent changes to
# SEACAS-latest and hdf5, also added some logic to help with troubleshooting
#
# 2016, July: Andrew added openblas and standalone SEACAS
#
#
# This script tries to build all of the libraries needed for goma
# in the specified directories.
#
# The libraries built should be suitable for a minimal settings.mk

# Set script to exit on error
# set -e

function usage() {
    echo "Usage: buildDependencies [options] [dependancy install location]"
    echo "       Options:"
    echo "               -jN  N : Number of make jobs to run"
}

MAKE_JOBS=1

while getopts ":j:h:d" opt; do
    case $opt in
        j)
            MAKE_JOBS=$OPTARG
            ;;
        h)
            usage
            ;;
        d)  TPL_LOC=${OPTARG}
            CUSTOM_LIB=true
            ;;
    esac
done

shift $((OPTIND - 1))

if [ "$CUSTOM_LIB" = true ]; then
    echo "Placing libraries in "${CUSTOM_LIB}
else
    echo "Setting library install location to current directory"
    TPL_LOC=$(dirname ${0})
fi
export TPL_LOC

OWNER=$USER

ACCESS=$TPL_LOC/seacas

function continue_check {
    echo "enter c, and only c, to continue"

    read -N 1 user_choice

    if [ $user_choice != 'c' ]
        then
        exit
    fi
}

echo "The current library configuration"
echo "Libraries will be installed at $TPL_LOC."
echo "This script will use $MAKE_JOBS make jobs."
continue_check

FORTRAN_LIBS=-lgfortran

export OWNER
export MAKE_JOBS

# Just one, Netcdf is required for seacas
ARCHIVE_NAMES=("netcdf-4.3.3.1.tar.gz")
ARCHIVE_MD5SUMS=("5c9dad3705a3408d27f696e5b31fb88c")
ARCHIVE_URLS=("ftp://ftp.unidata.ucar.edu/pub/netcdf/netcdf-4.3.3.1.tar.gz")

ARCHIVE_DIR_NAMES=("netcdf-4.3.3.1")

SITE_PATCH="4c4
< #define Owner gdsjaar
---
> #define Owner $OWNER
11c11
< #define AccessRoot /Users/gdsjaar/src/SEACAS-SF
---
> #define AccessRoot $TPL_LOC/SEACAS-2013-12-03
47a48,50
> #define ExcludeAnalysis
> #define Parallel 0
> 
48a52
> #define HasMatlab No
"

read -d '' NETCDF_PATCH << "EOF"
229c229
< #define NC_MAX_DIMS	1024
---
> #define NC_MAX_DIMS	65536
231c231
< #define NC_MAX_VARS	8192
---
> #define NC_MAX_VARS	524288
233c233
< #define NC_MAX_VAR_DIMS	1024 /**< max per variable dimensions */
---
> #define NC_MAX_VAR_DIMS	8 /**< max per variable dimensions */
EOF


read -d '' SEACAS_PATCH << "EOF"
1c1
< #!/bin/sh
---
> #!/bin/bash
21,22c21,22
< #COMPILER="gnu"
< COMPILER="clang"
---
> COMPILER="gnu"
> #COMPILER="clang"
EOF
SEACAS_PATCH=${SEACAS_PATCH//__TPL_LOC4__/$TPL_LOC}


mkdir -p ${TPL_LOC}
cd ${TPL_LOC}

function mychecksum {
    local count=$2
    local archive=$1
    local MD5SAVED=${ARCHIVE_MD5SUMS[count]}
    local MD5ARCHIVE=($(md5sum $archive))
    if [ $MD5SAVED != $MD5ARCHIVE ]; then
        echo "Issue checksum with archive:"
        echo $archive
        continue_check
    fi
}

mkdir -p tars
cd tars
#downloads
count=0
for i in ${ARCHIVE_NAMES[@]}; do
    echo "Check for $i at ${ARCHIVE_URLS[count]}"
    
    if ! [ -f $i ]
    then
        wget ${ARCHIVE_URLS[count]} -O $i
        mychecksum $i $count
    else
        mychecksum $i $count
    fi
    cd ..


    if [ -d ${ARCHIVE_DIR_NAMES[count]} ]
    then
	if [[ ${ARCHIVE_DIR_NAMES[count]} == *"ARPACK"* ]]
	    then
	    if ! tar tf $i &> /dev/null; then
		tar -xf tars/$i
	    fi
	fi
	echo "already extracted ${i}"
	echo "dirname: ${ARCHIVE_DIR_NAMES[count]}"
    else
	if ! tar tf $i &> /dev/null; then
	    tar -xf tars/$i
	fi
    fi
    count=$(( $count + 1 ))
    cd tars
done

cd ..

#seacas tpls
if [ -d seacas ]
then
    echo "already cloned seacas git repo"
else
    git clone https://github.com/gsjaardema/seacas.git seacas
fi

cd seacas/TPL


if [ -d netcdf/netcdf-4.3.3.1 ]
then
    echo "already moved netcdf source"
else
    mv ../../netcdf-4.3.3.1 netcdf/netcdf-4.3.3.1
fi
continue_check
#continue_check
#netcdf
if [ -e ../lib/libnetcdf.a ]
then
    echo "netcdf already built"
else
    echo "building netcdf"
    cd netcdf/netcdf-4.3.3.1
    cd include
    echo "$NETCDF_PATCH" > netcdf.patch
    patch -f --ignore-whitespace netcdf.h < netcdf.patch
    cd ..
    export CPPFLAGS=-I$ACCESS/include
    export LDFLAGS=-L$ACCESS/lib 
    echo $CPPFLAGS
    echo $LDFLAGS
    CC=$OPENMPI_TOP/bin/mpicc ./configure --prefix=$ACCESS --enable-shared=yes --disable-dap --enable-parallel-tests --enable-netcdf-4 --disable-fsync --disable-cdmremote
    continue_check
    make -j$MAKE_JOBS
    make install
    cd ../..
fi
continue_check
#now for seacas
if [ -f $TPL_LOC/seacas/bin/aprepro ]
then
    echo "seacas already built"
else
    cd $ACCESS
    mkdir build
    echo "$SEACAS_PATCH" > seacas.patch
    patch -f --verbose --ignore-whitespace cmake-config < seacas.patch
    sed -i '/TPL\_ENABLE\_CGNS/ c \ \-D TPL\_ENABLE\_CGNS\:BOOL\=OFF \\' cmake-config
    sed -i '/TPL\_ENABLE\_MPI/ c \ \-D TPL\_ENABLE\_MPI\:BOOL\=ON \\' cmake-config
    continue_check
    cd build
    echo $PATH
    ../cmake-config
    continue_check
    make -j $MAKE_JOBS
    #continue_check
    make install
fi


#continue_check
#make lapack
# lapack is made in openblas now
#cd $TPL_LOC/lapack-3.2.1
#if [ -f liblapack.a ]
#then
#    echo "LAPACK already built"
#else
#    mv make.inc.example make.inc
#    echo "$LAPACK_PATCH" > make.patch
#    patch make.inc < make.patch
#    make -j$MAKE_JOBS
#    cp lapack_LINUX.a liblapack.a
#fi

#continue_check
#make sparse
cd $TPL_LOC/sparse
if [ -f lib/libsparse.a ]
then 
    echo "Sparse already built"
else
    cd src
    make -j$MAKE_JOBS
    cd ../lib/
    cp sparse.a libsparse.a
    cd ..
fi

#continue_check
#make SuiteSparse
cd $TPL_LOC/SuiteSparse
if [ -f UMFPACK/Lib/libumfpack.a ]
then
    echo "SuiteSparse is already built"
else
    cd SuiteSparse_config
    echo `pwd`
    echo "$SUITESPARSE_CONFIG_PATCH" > SuiteSparse_config.patch
    patch SuiteSparse_config.mk < SuiteSparse_config.patch
    cd ..
    make
    cd UMFPACK/Include
    ln -s ../../SuiteSparse_config/SuiteSparse_config.h UFconfig.h
    ln -s ../../SuiteSparse_config/SuiteSparse_config.h SuiteSparse_config.h

fi
cd ../../..


#continue_check
#make y12m
cd $TPL_LOC/
if [ -d $TPL_LOC/y12m ]
then 
    echo "y12m directory already renamed"
else
mv $TPL_LOC/y12m-1.0 $TPL_LOC/y12m
fi

cd $TPL_LOC/y12m
if [ -f liby12m.a ]
then
    echo "y12m already built"
else
    make FC=$TPL_LOC/openmpi-1.6.4/bin/mpif77
fi

#continue_check
# make scalapack
cd $TPL_LOC/scalapack-2.0.2
if [ -f libscalapack.a ]
then
    echo "scalapack already built"
else
    cp SLmake.inc.example SLmake.inc
    echo "$SCALAPACK_PATCH" > scalapack.patch
    patch SLmake.inc < scalapack.patch
    make # scalapack only compiles with 1 make job
fi

#continue_check
# make mumps
cd $TPL_LOC/MUMPS_5.0.1
if [ -f lib/libdmumps.a ]
then
    echo "MUMPS already built"
else
    cat > Makefile.inc <<EOF
# Begin orderings
#LSCOTCHDIR = /usr/lib
#ISCOTCH   = -I/usr/include/scotch # only needed for ptscotch

#LSCOTCH   = -L\$(LSCOTCHDIR) -lptesmumps -lptscotch -lptscotcherr
#LSCOTCH   = -L\$(LSCOTCHDIR) -lesmumps -lscotch -lscotcherr

LPORDDIR = \$(topdir)/PORD/lib/
IPORD    = -I\$(topdir)/PORD/include/
LPORD    = -L\$(LPORDDIR) -lpord

LMETISDIR = $TPL_LOC/parmetis-4.0.3
IMETIS    = -I\$(LMETISDIR)/include
LMETIS    = -L\$(LMETISDIR)/lib -lparmetis -lmetis

ORDERINGSF = -Dparmetis -Dpord
ORDERINGSC  = \$(ORDERINGSF)
LORDERINGS = \$(LMETIS) \$(LPORD)
IORDERINGSF =
IORDERINGSC = \$(IMETIS) \$(IPORD)
# End orderings
################################################################################

PLAT    =
LIBEXT  = .a
OUTC    = -o
OUTF    = -o
RM = /bin/rm -f
CC = mpicc
FC = mpif90
FL = mpif90
AR = ar vr 
RANLIB = ranlib
SCALAP  = -L$TPL_LOC/scalapack-2.0.2 -lscalapack

INCPAR = -I$TPL_LOC/openmpi-1.6.4/include

LIBPAR = \$(SCALAP)  -L$TPL_LOC/openmpi-1.6.4/lib -lmpi -lmpi_f77

INCSEQ = -I\$(topdir)/libseq
LIBSEQ  =  -L\$(topdir)/libseq -lmpiseq

LIBBLAS = -L$TPL_LOC/BLAS/lib -lopenblas
LIBOTHERS = -lpthread

#Preprocessor defs for calling Fortran from C (-DAdd_ or -DAdd__ or -DUPPER)
CDEFS   = -DAdd_

#Begin Optimized options
OPTF    = -O  -DALLOW_NON_INIT
OPTL    = -O
OPTC    = -O
#End Optimized options
INCS = \$(INCPAR)
LIBS = \$(LIBPAR)
LIBSEQNEEDED =

EOF
    make -j$MAKE_JOBS
fi

#continue_check
#make trilinos
rm -rf $TPL_LOC/trilinos-12.6.3-Temp
mkdir $TPL_LOC/trilinos-12.6.3-Temp
cd $TPL_LOC/trilinos-12.6.3-Temp

rm -f CMakeCache.txt

FORTRAN_COMPILER=mpif90

FORTRAN_LIBS=-lgfortran

export MPI_BASE_DIR=$TPL_LOC/openmpi-1.6.4

# Have to set this to get TRY_RUN(...) commands to work
export LD_LIBRARY_PATH=$MPI_BASE_DIR/lib:$LD_LIBRARY_PATH

export PATH=$TPL_LOC/cmake-2.8.12.2/bin:$PATH

MPI_LIBS="-LMPI_BASE_DIR/lib -lmpi_f90 -lmpi_f77 -lmpi"
# Install directory
TRILINOS_INSTALL=$TPL_LOC/trilinos-12.6.3-Built
#continue_check


cmake \
-D CMAKE_AR=/usr/bin/ar \
-D CMAKE_RANLIB=/usr/bin/ranlib \
-D CMAKE_BUILD_TYPE:STRING=RELEASE \
-D CMAKE_CXX_COMPILER:FILEPATH=$MPI_BASE_DIR/bin/mpic++ \
-D CMAKE_C_COMPILER:FILEPATH=$MPI_BASE_DIR/bin/mpicc \
-D CMAKE_Fortran_COMPILER:FILEPATH=$MPI_BASE_DIR/bin/mpif90 \
-D CMAKE_VERBOSE_MAKEFILE:BOOL=TRUE \
-D Trilinos_ENABLE_Triutils:BOOL=ON \
-D Trilinos_ENABLE_SEACAS:BOOL=OFF \
-D Trilinos_ENABLE_Amesos:BOOL=ON \
-D Trilinos_ENABLE_Epetra:BOOL=ON \
-D Trilinos_ENABLE_Xpetra:BOOL=OFF \
-D Trilinos_ENABLE_Ifpack:BOOL=ON \
-D Trilinos_ENABLE_Teuchos:BOOL=ON \
-D Trilinos_ENABLE_ML:BOOL=ON \
-D Trilinos_ENABLE_AztecOO:BOOL=ON \
-D Trilinos_ENABLE_KokkosClassic:BOOL=OFF \
-D Trilinos_ENABLE_STK:BOOL=OFF \
-D Trilinos_ENABLE_Amesos2:BOOL=OFF \
-D Trilinos_ENABLE_Zoltan2:BOOL=OFF \
-D Trilinos_ENABLE_Belos:BOOL=ON \
-D Trilinos_ENABLE_Sacado:BOOL=ON \
-D Trilinos_ENABLE_EpetraExt:BOOL=ON \
-D Trilinos_ENABLE_Thyra:BOOL=ON \
-D Trilinos_ENABLE_ThyraTpetraAdapters:BOOL=ON \
-D Trilinos_ENABLE_Tpetra:BOOL=ON \
-D Trilinos_ENABLE_Stratimikos:BOOL=ON \
-D Trilinos_ENABLE_TESTS:BOOL=ON \
-D Trilinos_ENABLE_EXPLICIT_INSTANTIATION:BOOL=ON \
-D Trilinos_ENABLE_SECONDARY_STABLE_CODE:BOOL=ON \
 -D TPL_ENABLE_MPI:BOOL=ON \
  -D MPI_COMPILER:FILEPATH=$MPI_BASE_DIR/bin/mpiCC \
  -D MPI_EXECUTABLE:FILEPATH=$MPI_BASE_DIR/bin/mpirun \
  -D MPI_BASE_DIR:PATH=$MPI_BASE_DIR \
-D EpetraExt_BUILD_GRAPH_REORDERINGS:BOOL=ON \
-D EpetraExt_BUILD_BDF:BOOL=ON \
-D TPL_ENABLE_Boost:BOOL=OFF \
-D TPL_ENABLE_LAPACK:BOOL=ON \
-D LAPACK_LIBRARY_DIRS=$TPL_LOC/BLAS/lib \
-D LAPACK_LIBRARY_NAMES="openblas" \
-D TPL_ENABLE_BLAS:BOOL=ON \
-D BLAS_LIBRARY_DIRS=$TPL_LOC/BLAS/lib \
-D BLAS_LIBRARY_NAMES="openblas" \
-D CMAKE_INSTALL_PREFIX:PATH=$TRILINOS_INSTALL \
-D Trilinos_EXTRA_LINK_FLAGS:STRING="$FORTRAN_LIBS $MPI_LIBS" \
-D TPL_ENABLE_UMFPACK:BOOL=ON \
  -D UMFPACK_LIBRARY_NAMES:STRING="umfpack;amd;suitesparseconfig" \
  -D UMFPACK_LIBRARY_DIRS:PATH="$TPL_LOC/SuiteSparse/UMFPACK/Lib;$TPL_LOC/SuiteSparse/AMD/Lib;$TPL_LOC/SuiteSparse/SuiteSparse_config" \
  -D UMFPACK_INCLUDE_DIRS:PATH="$TPL_LOC/SuiteSparse/UMFPACK/Include;$TPL_LOC/SuiteSparse/AMD/Include;$TPL_LOC/SuiteSparse/SuiteSparse_config" \
-D TPL_ENABLE_AMD:BOOL=ON \
  -D AMD_LIBRARY_NAMES:STRING="amd;suitesparseconfig" \
  -D AMD_LIBRARY_DIRS:PATH="$TPL_LOC/SuiteSparse/AMD/Lib;$TPL_LOC/SuiteSparse/SuiteSparse_config" \
  -D AMD_INCLUDE_DIRS:PATH="$TPL_LOC/SuiteSparse/AMD/Include;$TPL_LOC/SuiteSparse/SuiteSparse_config" \
-D TPL_ENABLE_SuperLUDist:BOOL=ON \
  -D SuperLUDist_LIBRARY_NAMES:STRING="superludist" \
  -D SuperLUDist_LIBRARY_DIRS:PATH=$TPL_LOC/SuperLU_DIST_5.0.0/lib \
  -D SuperLUDist_INCLUDE_DIRS:PATH=$TPL_LOC/SuperLU_DIST_5.0.0/SRC \
-D TPL_ENABLE_ParMETIS:BOOL=ON \
  -D ParMETIS_LIBRARY_DIRS:PATH=$TPL_LOC/parmetis-4.0.3/lib\
  -D ParMETIS_INCLUDE_DIRS:PATH=$TPL_LOC/parmetis-4.0.3/include \
  -D TPL_ParMETIS_INCLUDE_DIRS:PATH=$TPL_LOC/parmetis-4.0.3/include \
  -D TPL_ENABLE_y12m:BOOL=ON \
  -D y12m_LIBRARY_NAMES:STRING="y12m" \
  -D y12m_LIBRARY_DIRS:PATH=$TPL_LOC/y12m \
-D TPL_ENABLE_MUMPS:BOOL=ON \
  -D MUMPS_LIBRARY_NAMES:STRING="dmumps;mumps_common;pord" \
  -D MUMPS_LIBRARY_DIRS:PATH=$TPL_LOC/MUMPS_5.0.1/lib \
  -D MUMPS_INCLUDE_DIRS:PATH=$TPL_LOC/MUMPS_5.0.1/include \
  -D CMAKE_CXX_FLAGS:STRING="-DMUMPS_5_0" \
  -D Amesos_ENABLE_SCALAPACK:BOOL=ON \
  -D SCALAPACK_INCLUDE_DIRS:FILEPATH="$TPL_LOC/scalapack-2.0.2/SRC" \
  -D SCALAPACK_LIBRARY_DIRS:FILEPATH="$TPL_LOC/scalapack-2.0.2" \
  -D SCALAPACK_LIBRARY_NAMES:STRING="scalapack" \
-D Amesos_ENABLE_SuperLUDist:BOOL=on \
-D Amesos_ENABLE_ParMETIS:BOOL=on \
-D Amesos_ENABLE_LAPACK:BOOL=ON \
-D Amesos_ENABLE_KLU:BOOL=ON \
-D Amesos_ENABLE_UMFPACK:BOOL=ON \
-D Amesos_ENABLE_MUMPS:BOOL=ON \
$EXTRA_ARGS \
$TPL_LOC/trilinos-12.6.3-Source



#continue_check
make -j$MAKE_JOBS
#continue_check
make install

