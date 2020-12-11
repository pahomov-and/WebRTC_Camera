ROOT_DIR=$(pwd)

ARCH=amd64


case $1 in
amd64*|i386*|arm64*) echo "OK!"; ARCH=$1 ;;
*) echo "$0 [default amd64]|[i386]|[arm64]"  ;;
esac

export PATH=$PATH:$ROOT_DIR/depot_tools
# export GYP_CROSSCOMPILE=1

NINJA_TARGET="webrtc rtc_json jsoncpp builtin_video_decoder_factory builtin_video_encoder_factory peerconnection p2p_server_utils task_queue default_task_queue_factory"

echo "root dir: $ROOT_DIR"

options=(
	"Install depot_tools" \
	"Fetch WebRtc" \
	"Install sysroot" \
	"Build WebRTC" \
	"Clean build" \
	"Quit")

PS3="Please enter your choice: "

select opt in "${options[@]}"
do
    case $opt in
    	"${options[0]}")
			################################################ Install depot_tools
            echo "$opt"

            git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git

            PS3="Please enter your choice ($opt): "
            ;;
        "${options[1]}")
			################################################ Fetch WebRtc
            echo "$opt"
            
            cd $ROOT_DIR/webrtc
			#fetch --no-history webrtc 
			fetch --no-history --nohooks webrtc 
			#./build/linux/sysroot_scripts/install-sysroot.py --arch=$ARCH 
			#gclient sync
			gclient sync --force

            PS3="Please enter your choice ($opt): "
            ;;
        "${options[2]}")
			################################################ Install sysroot
            echo "$opt"
            
            $ROOT_DIR/webrtc/src//build/linux/sysroot_scripts/install-sysroot.py --arch=$ARCH

            PS3="Please enter your choice ($opt): "
            ;;
        "${options[3]}")
            ################################################ Build WebRTC
            pushd $ROOT_DIR/webrtc/src/
            
			if [[ $ARCH == "amd64" ]]; then
				echo "ARCH: $ARCH"
				#exit 0
				#gn gen out/$ARCH/Release --args='use_rtti=true rtc_desktop_capture_supported=false target_cpu="x64" is_debug=false symbol_level=2 use_custom_libcxx=false treat_warnings_as_errors=false rtc_use_h264=true ffmpeg_branding="Chrome" rtc_include_tests=false use_ozone=true rtc_include_pulse_audio=false '
				gn gen out/$ARCH/Release --args='is_chrome_branded=true use_rtti=true rtc_desktop_capture_supported=false is_debug=false symbol_level=2 use_custom_libcxx=false treat_warnings_as_errors=false rtc_use_h264=true ffmpeg_branding="Chrome" rtc_include_tests=false use_ozone=true rtc_include_pulse_audio=false '

			elif [[ $ARCH == "i386" ]]; then
				echo "ARCH: $ARCH"
				exit 0
			elif [[ $ARCH == "arm64" ]]; then
				echo "ARCH: $ARCH"
				exit 0
				gn gen out/$ARCH/Release --args='is_chrome_branded=true use_rtti=true rtc_desktop_capture_supported=false target_cpu="arm64" is_debug=false symbol_level=2 use_custom_libcxx=false treat_warnings_as_errors=false rtc_use_h264=true ffmpeg_branding="Chrome" rtc_include_tests=false use_ozone=true rtc_include_pulse_audio=false '
			fi

			#ninja -C out/$ARCH/Release -t clean
			# ninja -C out/$ARCH/Release webrtc
			ninja -C out/$ARCH/Release $NINJA_TARGET

			popd

			PS3="Please enter your choice ($opt): "
            ;;

        "${options[4]}")
            rm -rf $ROOT_DIR/webrtc/src/out/$ARCH/Release
            ;;

        "${options[5]}")
            exit 0
            ;;
        *) echo "invalid option $REPLY";;
    esac
done

