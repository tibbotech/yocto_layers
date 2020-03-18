
# it's a hack of Qt Creator cross-SDK+cmake handling
# it detects -arch from the SDK enviroment and adds -march... flags
# without -march CMAKE from the SDK in Qt Creator will fail
#
# Do not forget to copy-paste your SDK env settings into
# Qt Creator -> Tools -> Settings -> Kit - >Env

include($ENV{OECORE_NATIVE_SYSROOT}/usr/share/cmake/OEToolchainConfig.cmake)

if ( $ENV{CC} MATCHES "(-march.*) --sysroot")
  message("Qt Creator fix. Got arch flags from ENV[CC]: ${CMAKE_MATCH_1}")
  set(DV_ARCH ${CMAKE_MATCH_1})
endif()
if ( $ENV{CC} MATCHES "(-mfpu.*) --sysroot")
  message("Qt Creator fix. Got arch flags from ENV[CC]: ${CMAKE_MATCH_1}")
  set(DV_ARCH ${CMAKE_MATCH_1})
endif()

#set (CMAKE_C_COMPILER_ARG1   "-march=armv7-a -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a8 --sysroot=$ENV{SDKTARGETSYSROOT}")
set (CMAKE_C_COMPILER_ARG1   "${DV_ARCH} --sysroot=$ENV{SDKTARGETSYSROOT}")
set (CMAKE_CXX_COMPILER_ARG1   "${DV_ARCH} --sysroot=$ENV{SDKTARGETSYSROOT}")
