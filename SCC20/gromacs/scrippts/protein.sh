#!/bin/bash

GMX=${HOME}/gromacs-2020.4/build-icc18/bin/gmx

echo "using \"${GMX}\"" 
echo "force field \"${FORCE_FIELD}\""


mkdir -p ${FORCE_FIELD}
cd ${FORCE_FIELD}
echo "include ../Makefile.tpl" > Makefile
make $1

