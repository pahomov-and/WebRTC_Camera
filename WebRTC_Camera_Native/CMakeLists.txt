cmake_minimum_required(VERSION 3.0)
project(WebRTC_Camera_Native)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DWEBRTC_POSIX -fno-rtti -DNO_SSL)

set(CIVETWEB_ENABLE_SSL OFF)
set(CIVETWEB_ENABLE_SSL_DYNAMIC_LOADING OFF)

set(ARCH "" CACHE STRING "Architecture to tell clang")

set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_SYSTEM_NAME Linux)
set(WEBRTCROOT "${CMAKE_CURRENT_SOURCE_DIR}/../webrtc")
set(TOOLCHAIN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04")

set(CMAKE_C_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/../clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/bin/clang)
set(CMAKE_CXX_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/../clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/bin/clang++)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/)

set (WEBRTCBUILD "Release" CACHE STRING "WEBRTC build type")


if (ARCH MATCHES "arm64")
    set(ROOTFS ${WEBRTCROOT}/src/build/linux/debian_sid_arm64-sysroot)
    set(TARGET_TRIPLE "aarch64-linux-gnu" )
    set(MAKE_FLAGS "-target aarch64-linux-gnu -B ${TOOLCHAIN_PATH}/bin --sysroot=${ROOTFS} -lgtk-3 -lgio-2.0 -lglib-2.0 -lgmodule-2.0")
    set(WEBRTCARGS target_cpu="arm64"\n${WEBRTCARGS})
    set (WEBRTCARGS use_sysroot=true\n${WEBRTCARGS})

    link_directories(
            ${ROOTFS}/lib/
            ${ROOTFS}/lib/aarch64-linux-gnu/
            ${ROOTFS}/usr/lib/aarch64-linux-gnu/
            ${ROOTFS}/usr/lib/
    )
#    set(CMAKE_LIBRARY_PATH ${ROOTFS}/lib/aarch64-linux-gnu/)
    set (WEBRTCOBJS ${WEBRTCROOT}/src/out/${WEBRTCBUILD}/obj)
else()
    set(MAKE_FLAGS "-target x86_64-linux-gnu -B ${TOOLCHAIN_PATH}/bin --sysroot=${ROOTFS} -lgtk-3 -lgio-2.0 -lglib-2.0 -lgmodule-2.0 ")
    set (WEBRTCARGS target_cpu="x64"\n${WEBRTCARGS})
    set (WEBRTCARGS use_sysroot=false\n${WEBRTCARGS})
    set (WEBRTCOBJS ${WEBRTCROOT}/src/out/${ARCH}/${WEBRTCBUILD}/obj)

    MESSAGE("WEBRTCOBJS: ${WEBRTCOBJS}")

endif()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(LLVM_TARGET_ARCH ${ARCH})
set(LLVM_TARGETS_TO_BUILD ${ARCH})


set(CMAKE_C_FLAGS ${MAKE_FLAGS})
set(CMAKE_CXX_FLAGS ${MAKE_FLAGS})

set(LLVM_DEFAULT_TARGET_TRIPLE ${TARGET_TRIPLE})
set(LLVM_ENABLE_PROJECTS "clang")
set(C_INCLUDE_DIRS ${ROOTFS})

FILE(GLOB_RECURSE WEBRTBBRCOBJS ${WEBRTCOBJS}/modules/congestion_controller/bbr/*${CMAKE_C_OUTPUT_EXTENSION})
FILE(GLOB_RECURSE WEBRTJSONCPPCOBJS ${WEBRTCOBJS}/third_party/jsoncpp/jsoncpp/*${CMAKE_C_OUTPUT_EXTENSION})
FILE(GLOB_RECURSE WEBRTP2POBJ ${WEBRTCOBJS}/p2p/p2p_server_utils/*${CMAKE_C_OUTPUT_EXTENSION})
FILE(GLOB_RECURSE WEBRTTASK ${WEBRTCOBJS}/api/task_queue/*${CMAKE_C_OUTPUT_EXTENSION} ${WEBRTCOBJS}/rtc_base/rtc_task_queue_*/*${CMAKE_C_OUTPUT_EXTENSION})
SET (WEBRTCEXTRAOBJS ${WEBRTJSONCPPCOBJS} ${WEBRTBBRCOBJS} ${WEBRTP2POBJ} ${WEBRTTASK} ${WEBRTCOBJS}/rtc_base/rtc_json/json${CMAKE_C_OUTPUT_EXTENSION})



set (WEBRTCARGS rtc_include_tests=false\nrtc_enable_protobuf=false\nuse_custom_libcxx=false\nrtc_include_pulse_audio=false\nrtc_build_examples=false\nrtc_build_tools=false\ntreat_warnings_as_errors=false\nrtc_enable_libevent=false\nrtc_build_libevent=false\n)
set (WEBRTCARGS is_debug=true\n${WEBRTCARGS})
#set (WEBRTCARGS target_cpu="arm64"\n${WEBRTCARGS})
#set (WEBRTCARGS use_sysroot=true\n${WEBRTCARGS})
set (WEBRTCARGS is_clang=true\n${WEBRTCARGS})
set (WEBRTCARGS is_chrome_branded=true\n${WEBRTCARGS})

set (WEBRTCINCLUDE
        ${WEBRTCROOT}/src
        ${WEBRTCROOT}/src/rtc_base
        ${WEBRTCROOT}/src/third_party/abseil-cpp
        ${WEBRTCROOT}/src/third_party/jsoncpp/source/include
        ${WEBRTCROOT}/src/third_party/jsoncpp/generated
        ${WEBRTCROOT}/src/third_party/libyuv/include


        )
MESSAGE("WEBRTCINCLUDE = ${WEBRTCINCLUDE}")
include_directories(${WEBRTCINCLUDE})

find_package(PkgConfig QUIET)
find_library(WEBRTC_LIBRARY NAMES webrtc PATHS ${WEBRTCOBJS})
pkg_check_modules(GTK3 QUIET gtk+-3.0)
MESSAGE("GTK_FOUND = ${GTK3_FOUND}")
if(NOT GTK3_FOUND OR (WEBRTCDESKTOPCAPTURE STREQUAL "OFF"))
    set (WEBRTCARGS use_ozone=true\nis_desktop_linux=false\n${WEBRTCARGS})
endif()

MESSAGE("TOOLCHAIN_PATH = ${TOOLCHAIN_PATH}")
MESSAGE("ROOTFS = ${ROOTFS}")
MESSAGE("WEBRTCBUILD = ${WEBRTCBUILD}")
MESSAGE("WEBRTCROOT = ${WEBRTCROOT}")
MESSAGE("WEBRTCDESKTOPCAPTURE= ${WEBRTCDESKTOPCAPTURE}")
MESSAGE("CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")
MESSAGE("MAKE_FLAGS=${MAKE_FLAGS}")
MESSAGE("WEBRTC_LIBRARY=${WEBRTC_LIBRARY}")


########################################################################################
find_package(Git)

if(GIT_FOUND)
    MESSAGE("Update submodule")
    EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} submodule update --init)
    EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty OUTPUT_VARIABLE PROJECTVERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
#EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} submodule update --init)
#EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty OUTPUT_VARIABLE PROJECTVERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
#set (VERSION "${PROJECTVERSION}/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

#EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} -C civetweb describe --tags --always --dirty OUTPUT_VARIABLE CIVETVERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
set (VERSION "${VERSION} civetweb@${CIVETVERSION}")

# civetweb
add_library (civetweb STATIC civetweb/src/civetweb.c civetweb/src/CivetServer.cpp)
#target_link_libraries (${PROJECT_NAME} civetweb)
target_include_directories(civetweb PUBLIC civetweb/include)

#add_definitions(-DVERSION=\"${VERSION}\")
########################################################################################

#find_package(OpenSSL)

include_directories(
        .
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/civetweb/include
)

FILE(GLOB SOURCE
        main.cpp
        src/*.cpp
        )

FILE(GLOB SOURCE ${SOURCE} ${WEBRTCEXTRAOBJS})

add_executable (${PROJECT_NAME} ${SOURCE} ${WEBRTCEXTRAOBJS})

target_link_libraries (${PROJECT_NAME} ${WEBRTC_LIBRARY})
target_link_libraries (${PROJECT_NAME} pthread)
target_link_libraries (${PROJECT_NAME} -Wl,-Bstatic -latomic -Wl,-Bdynamic)
target_link_libraries (${PROJECT_NAME} dl dbus-1 X11 Xt)
target_link_libraries (${PROJECT_NAME} civetweb )
