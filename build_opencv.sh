ROOT_DIR=$(pwd)

ARCH=amd64


case $1 in
amd64*|i386*|arm64*) echo "OK!"; ARCH=$1 ;;
*) echo "$0 [default amd64]|[i386]|[arm64]"; exit 0  ;;
esac

export PATH=$PATH:$ROOT_DIR/depot_tools

echo "root dir: $ROOT_DIR"

options=(
	"Download/Update OpenCV" \
	"Build and Install" \
	"Clean build" \
	"Quit")

PS3="Please enter your choice: "

select opt in "${options[@]}"
do
    case $opt in
    	"${options[0]}")
			################################################ Download/Update OpenCV
            echo "$opt"
            # if [ ! -d "$ROOT_DIR/opencv" ] ; then
            	git clone https://github.com/opencv/opencv.git
            # elif
            	# pushd opencv
            	# git pull
            	# popd
            # fi

            # if [ ! -d "$ROOT_DIR/opencv_contrib" ] ; then
                git clone https://github.com/opencv/opencv_contrib.git
            # elif
            	# cd $ROOT_DIR/opencv_contrib
            	# git pull
            	# cd $ROOT_DIR
            # fi

            PS3="Please enter your choice ($opt): "
            ;;
        "${options[1]}")
			################################################ Build
            echo "$opt"

            cd $ROOT_DIR
            mkdir -p $ROOT_DIR/build_opencv_$ARCH && cd $ROOT_DIR/build_opencv_$ARCH

            SYSROOT=$ROOT_DIR/webrtc/src/build/linux/debian_sid_arm64-sysroot
            # CMAKE_C_FLAGS="-march=rv64gcv0p9 -menable-experimental-extensions --gcc-toolchain=${RISCV_GCC_INSTALL_ROOT} -w ${CMAKE_C_FLAGS}"
            # MAKE_FLAGS="-target aarch64-linux-gnu -B $ROOT_DIR/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/bin --sysroot=$SYSROOT "
            MAKE_FLAGS="-frtti -arch arm64  -B $ROOT_DIR/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/bin --sysroot=$SYSROOT "

            cmake -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules \
            -DCMAKE_BUILD_TYPE=Releas \
            -DCMAKE_SYSTEM_NAME=Linux \
            -DBUILD_SHARED_LIBS=ON \
            -DCMAKE_SYSTEM_PROCESSOR=arm64 \
            -DCMAKE_C_COMPILER_TARGET=aarch64-linux-gnu \
            -DCMAKE_C_FLAGS=$MAKE_FLAGS \
            -DCMAKE_CXX_FLAGS=$MAKE_FLAGS \
            -DCMAKE_C_COMPILER=$ROOT_DIR/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/bin/clang \
            -DCMAKE_CXX_COMPILER_TARGET=aarch64-linux-gnu \
			-DCMAKE_CXX_COMPILER=$ROOT_DIR/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/bin/clang++ \
			-DCMAKE_ASM_COMPILER=aarch64-linux-gnu \
			-DCMAKE_ASM_COMPILER=$ROOT_DIR/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/bin/clang \
			-DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
			-DCMAKE_SYSROOT=$SYSROOT \
			-DCMAKE_EXE_LINKER_FLAGS="${CMAKE_EXE_LINKER_FLAGS} --sysroot=$SYSROOT" \
			-DCMAKE_SHARED_LINKER_FLAGS="${CMAKE_SHARED_LINKER_FLAGS} --sysroot=$SYSROOT" \
			-DCMAKE_MODULE_LINKER_FLAGS="${CMAKE_MODULE_LINKER_FLAGS} --sysroot=$SYSROOT" \
			-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
			-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
			-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
			-DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY \
			-DBUILD_TESTS=OFF \
			-DBUILD_PERF_TESTS=OFF \
			-DBUILD_EXAMPLES=OFF \
			-DBUILD_opencv_apps=OFF \
			-DCMAKE_INSTALL_PREFIX=$SYSROOT \
            $ROOT_DIR/opencv && cmake --build . --target install -- -j${nproc}

			# -DCMAKE_VERBOSE_MAKEFILE=TRUE \
			# -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
			# -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
			# -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
			# -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY \
            # -DCMAKE_FLAGS="--os=linux --arch=armv8 -target aarch64-linux-gnu -B $ROOT_DIR/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/bin --sysroot=$SYSROOT " \

            PS3="Please enter your choice ($opt): "
            ;;
        "${options[2]}")
			################################################ Clean build
            echo "$opt"

            sudo rm -rf  $ROOT_DIR/build_opencv_$ARCH

            PS3="Please enter your choice ($opt): "
            ;;
        "${options[3]}")
            exit 0
            ;;
        *) echo "invalid option $REPLY";;
    esac
done

