#!/bin/bash

set -e   # stop whenever we got an error...

pth=$PWD

# true = compile, false = do not compile
gsl=true
fftw=true
hdf5=true
sprng=true
g9p=true

file_hdf5=`ls -1t | grep hdf5*gz | head -n 1`
file_fftw=`ls -1t | grep fftw*gz | head -n 1`

echo $file_fftw

if [ ! -f gsl-1.16.tar.gz ] && [ $gsl = true ]
then
echo
echo === Downloading GSL ===
echo

wget   http://mirrors.ustc.edu.cn/gnu/gsl/gsl-1.16.tar.gz

fi

if [ ! -d gsl-1.16 ] && [ $gsl = true ]
then
tar -xzf gsl-1.16.tar.gz
fi


if [ -z $file_fftw ] && [ $fftw = true ]
then
echo
echo Trying to get http://www.fftw.org/fftw-3.3.4.tar.gz...
echo If it fails, please, download the latest version yourself!
echo

wget   http://www.fftw.org/fftw-3.3.4.tar.gz
file_fftw=fftw-3.3.4.tar.gz

fi

folder_fftw="${file_fftw%.tar.gz}"
if [ ! -d $folder_fftw ] && [ $fftw = true ]
then
tar -xzf $file_fftw
fi


if [ -z $file_hdf5 ] && [ $hdf5 = true ]
then
echo
echo Trying to get https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.8/hdf5-1.8.20/src/hdf5-1.8.20.tar.gz...
echo If it fails, please, download the latest version yourself!
echo


wget   https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.8/hdf5-1.8.20/src/hdf5-1.8.20.tar.gz
file_hdf5=hdf5-1.8.20.tar.gz
fi

folder_hdf5="${file_hdf5%.tar.gz}"
if [ ! -d $folder_hdf5 ] && [ $hdf5 = true ]
then
tar -xzf $file_hdf5
fi



if [ ! -f sprng2.0b.tar.gz ] && [ $sprng = true ]
then
echo
echo === Downloading SPRNG ===
echo

wget   http://www.sprng.org/Version2.0/sprng2.0b.tar.gz

fi


if [ ! -d sprng2.0 ] && [ $sprng = true ]
then
tar -xzf sprng2.0b.tar.gz
fi


if [ ! -d ginnungagap ] && [ $g9p = true ]
then
echo
echo === Downloading Ginnungagap ===
echo

git clone https://github.com/ginnungagapgroup/ginnungagap

fi




if [ $gsl = true ] ; then
	echo
	echo === Installing GSL ===
	echo

	cd gsl-1.16
	./configure --prefix=$pth
	make -j
	make install
	cd ..
fi

if [ $fftw = true ] ; then
	echo
	echo === Installing FFTW ===
	echo

	cd $folder_fftw
	./configure --prefix=$pth  \
		    --enable-sse \
		    --enable-float \
		    --disable-fortran \
		    --enable-openmp \
		    --enable-threads \
		    --enable-mpi \
            --enable-debug
	make -j
	make install
	./configure --prefix=$pth \
		    --enable-sse2 \
		    --disable-fortran \
		    --enable-openmp \
		    --enable-threads \
		    --enable-mpi \
            --enable-debug
	make -j
	make install

	cd ..
fi

if [ $hdf5 = true ] ; then
	echo
	echo === Installing HDF5 ===
	echo

	cd $folder_hdf5
	./configure --prefix=$pth  \
		    --disable-fortran \
		    --disable-cxx \
		    --enable-production \
		    --disable-deprecated-symbols \
            --enable-parallel \
            --with-pthread 
            #--enable-debug 
            #CFLAGS='-g -O0' CXXFLAGS='-g -O0'
	make -j 
	make install

	cd ..
fi

if [ $sprng = true ] ; then
	echo
	echo === Installing SPRNG ===
	echo

	cd sprng2.0
	patch -p1 -i ../ginnungagap/doc/devel/patches/sprng2.0.fixMemoryLeak.patch
	make src

	cd ..
fi

if [ $g9p = true ] ; then
	echo
	echo === Installing Ginnungagap ===
	echo

	cd ginnungagap

	./configure --with-mpi \
		    --with-openmp \
		    --with-sprng \
		    --with-sprng-prefix=$pth/sprng2.0 \
		    --with-fft-prefix=$pth \
		    --with-hdf5 \
		    --with-hdf5-prefix=$pth \
		    --with-gsl-prefix=$pth 
	make -j all
	cd ..
fi

echo === all done ===
