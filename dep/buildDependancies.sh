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
    echo "Usage: buildDependencies [options]"
    echo "       Options:"
    echo "               -j N   : Number of make jobs to run"
    echo "               -d PATH: Location to install the dependancies"
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
    TPL_LOC=$(realpath $(dirname ${0}))
fi
export TPL_LOC

OWNER=$USER

ACCESS=$TPL_LOC/seacas

function continue_check {
    echo "Press c if this is correct."

    read -N 1 user_choice

    if [ $user_choice != 'c' ]
        then
        exit
    fi
}

echo "The current library configuration"
echo "Libraries will be installed at: '$TPL_LOC'"
echo "This script will use $MAKE_JOBS make jobs."
continue_check

FORTRAN_LIBS=-lgfortran

export OWNER
export MAKE_JOBS

# Just one, Netcdf is required for seacas
ARCHIVE_NAMES=("netcdf-4.3.3.1.tar.gz" \
"hdf5-1.8.15.tar.gz")

ARCHIVE_MD5SUMS=("5c9dad3705a3408d27f696e5b31fb88c" \
"03cccb5b33dbe975fdcd8ae9dc021f24")

ARCHIVE_URLS=("ftp://ftp.unidata.ucar.edu/pub/netcdf/netcdf-4.3.3.1.tar.gz" \
"http://www.hdfgroup.org/ftp/HDF5/releases/hdf5-1.8.15/src/hdf5-1.8.15.tar.gz")

ARCHIVE_DIR_NAMES=("netcdf-4.3.3.1" \
"hdf5-1.8.15")

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

#seacas
if [ -d seacas ]
then
    echo "already cloned seacas git repo"
else
    git clone https://github.com/gsjaardema/seacas.git seacas
fi

cd seacas/TPL


if [ -d hdf5/hdf5-1.8.15 ]
then
    echo "already moved hdf5 source"
else
    pwd
    ls ../..
    mv ../../hdf5-1.8.15 hdf5/hdf5-1.8.15
fi

if [ -d netcdf/netcdf-4.3.3.1 ]
then
    echo "already moved netcdf source"
else
    mv ../../netcdf-4.3.3.1 netcdf/netcdf-4.3.3.1
fi

if [ -e ../lib/libhdf5.a ]
then
    echo "hdf5 already built"
else
    echo "building hdf5"
    cd hdf5/hdf5-1.8.15
    CC="cc -w" ./configure --enable-shared=yes --prefix=$ACCESS --enable-production --enable-debug=no
    make -j$MAKE_JOBS
    make install
    cd ../..
fi

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
    CC=cc ./configure --prefix=$ACCESS --enable-shared=yes --disable-dap --enable-netcdf-4 --disable-fsync --disable-cdmremote
    make -j$MAKE_JOBS
    make install
    cd ../..
fi

#seacas
if [ -f $TPL_LOC/seacas/bin/aprepro ]
then
    echo "seacas already built"
else
    cd $ACCESS
    mkdir build
    cd build
    echo $PATH
    ../cmake-exodus
    make -j $MAKE_JOBS
    make install
fi
echo "Seacas built"
