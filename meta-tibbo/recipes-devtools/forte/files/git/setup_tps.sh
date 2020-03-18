#!/bin/bash

export forte_bin_dir="bin/posix"

#set to boost-include directory
export forte_boost_test_inc_dirs=""
#set to boost-library directory
export forte_boost_test_lib_dirs=""

if [ ! -d "$forte_bin_dir" ]; then
  mkdir -p "$forte_bin_dir"
fi

if [ -d "$forte_bin_dir" ]; then
  cd "./$forte_bin_dir"
  cmake -G "Unix Makefiles" -DFORTE_ARCHITECTURE=Posix -DFORTE_COM_ETH=ON \
  -DFORTE_COM_FBDK=ON -DFORTE_COM_LOCAL=ON -DFORTE_TESTS=OFF \
  -DFORTE_TESTS_INC_DIRS=${forte_boost_test_inc_dirs} \
  -DFORTE_TESTS_LINK_DIRS=${forte_boost_test_inc_dirs} \
  -DFORTE_MODULE_CONVERT=ON -DFORTE_MODULE_MATH=ON -DFORTE_MODULE_IEC61131=ON \
  -DFORTE_MODULE_OSCAT=ON -DFORTE_MODULE_Test=ON -DFORTE_MODULE_UTILS=ON \
  -DFORTE_MODULE_SysFs=ON \
  -DFORTE_MODULE_MODBUS=ON \
  -DFORTE_MODULE_RECONFIGURATION=ON \
  -DFORTE_MODULE_EclipseSCADA_SFP=ON \
  -DFORTE_MODULE_RT_Events=ON \
  ../../
else
  echo "unable to create ${forte_bin_dir}"
  exit 1
fi
