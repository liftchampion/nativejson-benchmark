#!/bin/sh

# ULib - C++ application development framework
# Version: 1.4.2

prefix=/usr/local
libdir=${exec_prefix}/lib
includedir=${prefix}/include
exec_prefix=/usr/local

dirn=`dirname $1`
basen=`basename $1`

libsuffix=$2
test "$libsuffix" || libsuffix=so

export UMEMPOOL="0,0,0,48,-20,-20,-20,-20,0"
export PATH="/sbin:/usr/sbin:/usr/local/sbin:/bin:/usr/bin:/usr/local/bin"

${prefix}/bin/usp_translator $1.usp &&
${prefix}/bin/usp_libtool.sh --silent --tag=CXX --mode=compile \
	/opt/rh/devtoolset-9/root/usr/bin/g++ -I${includedir} -DHAVE_CONFIG_H  -mcrc32 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -pipe   -Wstrict-aliasing=2 -Wall -Wextra -Wsign-compare -Wpointer-arith -Wwrite-strings -Wmissing-declarations -Wpacked -Wswitch-enum -Wmissing-format-attribute -Winit-self -Wformat -Wenum-compare -Wlogical-not-parentheses -Wsizeof-array-argument -Wbool-compare -Wno-unused-result -Wshadow -Wsuggest-attribute=pure -Wsuggest-attribute=noreturn -Wlogical-op -Wduplicated-cond -Wtautological-compare -Wswitch-bool -Wshift-negative-value -Wshift-overflow -Wshift-overflow=2 -Wnull-dereference -Wnonnull -fno-stack-protector -ffast-math -ftree-vectorize -fno-crossjumping -fno-gcse -fpartial-inlining -Ofast -flto -ffat-lto-objects -fuse-linker-plugin -march=native -mtune=native -Winline -Wno-unused-parameter -Wno-unused-variable     -I/usr/local/include -fvisibility=hidden -fvisibility-inlines-hidden -fno-exceptions -fno-rtti  -fno-check-new -fno-enforce-eh-specs -Wno-deprecated -Wdelete-non-virtual-dtor -Wodr -Wterminate -Wlto-type-mismatch -Wsubobject-linkage -Wplacement-new -Wvirtual-inheritance -Wnamespaces  -MT $1.lo -MD -MP -c -o $1.lo $1.cpp &&
${prefix}/bin/usp_libtool.sh --silent --tag=CXX --mode=link \
	/opt/rh/devtoolset-9/root/usr/bin/g++  -mcrc32 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -pipe   -Wstrict-aliasing=2 -Wall -Wextra -Wsign-compare -Wpointer-arith -Wwrite-strings -Wmissing-declarations -Wpacked -Wswitch-enum -Wmissing-format-attribute -Winit-self -Wformat -Wenum-compare -Wlogical-not-parentheses -Wsizeof-array-argument -Wbool-compare -Wno-unused-result -Wshadow -Wsuggest-attribute=pure -Wsuggest-attribute=noreturn -Wlogical-op -Wduplicated-cond -Wtautological-compare -Wswitch-bool -Wshift-negative-value -Wshift-overflow -Wshift-overflow=2 -Wnull-dereference -Wnonnull -fno-stack-protector -ffast-math -ftree-vectorize -fno-crossjumping -fno-gcse -fpartial-inlining -Ofast -flto -ffat-lto-objects -fuse-linker-plugin -march=native -mtune=native -Winline -Wno-unused-parameter -Wno-unused-variable     -L/usr/local/lib -Wl,-O1 -Wl,--as-needed -Wl,-z,now,-O1,--hash-style=gnu,--sort-common -Wl,--as-needed  -o $1.la -rpath ${prefix}/libexec/ulib/usp \
	-module -export-dynamic -avoid-version -no-undefined $1.lo  -lulib  /opt/rh/devtoolset-9/root/usr/lib/gcc/x86_64-redhat-linux/9/libstdc++.a /opt/rh/devtoolset-9/root/usr/lib/gcc/x86_64-redhat-linux/9/libgcc.a  -lpthread -lrt -ldl -lc &&
mv $1.usp ${dirn}/.libs; rm -rf $1.d $1.la $1.lo $1.o $1.cpp; mv ${dirn}/.libs/${basen}.usp ${dirn}/.libs/${basen}.$libsuffix ${dirn}; rm -rf ${dirn}/.libs
