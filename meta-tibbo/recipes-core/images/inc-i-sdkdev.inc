
# GUI
#inherit populate_sdk_qt5
#TOOLCHAIN_TARGET_TASK_append += " fltk"
#TOOLCHAIN_TARGET_TASK_append += " fltk-staticdev"

# NodeJS
#TOOLCHAIN_TARGET_TASK_append += " nodejs"
TOOLCHAIN_HOST_TASK_append += " nativesdk-swig"
#TOOLCHAIN_HOST_TASK_append += " nativesdk-nodejs nativesdk-nodejs-npm"

TOOLCHAIN_HOST_TASK_append += " nativesdk-rpm-build"

TOOLCHAIN_TARGET_TASK_append += " kernel-devsrc"

TOOLCHAIN_TARGET_TASK_append += " boost-dev"
TOOLCHAIN_TARGET_TASK_append += " boost-staticdev"
