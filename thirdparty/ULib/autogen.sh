#!/bin/sh

# bootstrap.sh -- Used to setup the configure.in, autoheader and Makefile.in's if configure
# has not been generated. This script is only needed for developers when configure has not
# been run, or if a Makefile.am in a non-configured directory has been updated

bootstrap() {
  if "$@"; then
    true # Everything OK
  else
    echo "The command <$@> failed"
    echo "Autotool bootstrapping failed. You will need to investigate and correct" ;
    echo "before you can develop on this source tree" 
    exit 1
  fi
}

# Bootstrap the autotool subsystems
echo "running bootstrap on top level source directory"

touch configure.ac
bootstrap libtoolize --force --copy
bootstrap aclocal --force -I m4
bootstrap autoheader -f
bootstrap automake --foreign --add-missing --copy
bootstrap autoconf

echo "Autotool bootstrapping complete."
