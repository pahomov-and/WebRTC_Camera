//
// Created by tymbys on 14.04.20.
//

#pragma once

#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <vector>

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


#include "modules/video_capture/video_capture_factory.h"
#include "media/base/video_broadcaster.h"
#include "pc/video_track_source.h"

#include "api/video/i420_buffer.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"

#include "libyuv/convert_argb.h"

#include "media/base/fake_frame_source.h"
#include "media/base/fake_media_engine.h"
#include "media/base/video_source_base.h"

#include "libyuv/convert.h"
#include "libyuv/video_common.h"

#include "media/base/fake_frame_source.h"
#include "media/base/fake_frame_source.cc"

#include "common_video/libyuv/include/webrtc_libyuv.h"


#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

#define NAMEDPIPE_NAME "/tmp/my_named_pipe"
#define BUFSIZE        50


#define SUBSAMPLE(v, a) ((((v) + (a)-1)) / (a))
#define align_buffer_page_end(var, size)                                \
  uint8_t* var##_mem =                                                  \
      reinterpret_cast<uint8_t*>(malloc(((size) + 4095 + 63) & ~4095)); \
  uint8_t* var = reinterpret_cast<uint8_t*>(                            \
      (intptr_t)(var##_mem + (((size) + 4095 + 63) & ~4095) - (size)) & ~63)




int I420DataSize(int height, int stride_y, int stride_u, int stride_v) {
    return stride_y * height + (stride_u + stride_v) * ((height + 1) / 2);
}



class FakeCapturer: public rtc::VideoSinkInterface<webrtc::VideoFrame>,  public rtc::VideoSourceInterface<webrtc::VideoFrame> {
public:
    static VideoCapture cap;

    static FakeCapturer* Create(const std::string & videourl, const std::map<std::string, std::string> & opts) {
        std::unique_ptr<FakeCapturer> vcm_capturer(new FakeCapturer());
        size_t width = 0;
        size_t height = 0;
        size_t fps = 0;

        std::cout << "FILE: " << __FILE__ << "LINE: " << __LINE__ << std::endl;

        if (opts.find("width") != opts.end()) {
            width = std::stoi(opts.at("width"));
        }
        if (opts.find("height") != opts.end()) {
            height = std::stoi(opts.at("height"));
        }
        if (opts.find("fps") != opts.end()) {
            fps = std::stoi(opts.at("fps"));
        }
        if (!vcm_capturer->Init(width, height, fps, videourl)) {
            RTC_LOG(LS_WARNING) << "Failed to create FakeCapturer(w = " << width
                                << ", h = " << height << ", fps = " << fps
                                << ")";
            return nullptr;
        }
        return vcm_capturer.release();
    }
    virtual ~FakeCapturer() {
//        Destroy();
        isRun = false;
        _thread.join();
    }

    void OnFrame(const webrtc::VideoFrame& frame) override {
//    void InjectFrame(const webrtc::VideoFrame& frame) {
//        std::cout << "FILE: " << __FILE__ << "LINE: " << __LINE__ << std::endl;
        m_broadcaster.OnFrame(frame);
    }

private:
    FakeCapturer() : m_vcm(nullptr) {}

    bool Init(size_t width,
              size_t height,
              size_t target_fps,
              const std::string & videourl) {
//        std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> device_info(webrtc::VideoCaptureFactory::CreateDeviceInfo());

        std::string deviceId;
//        int num_videoDevices = device_info->NumberOfDevices();
//        RTC_LOG(INFO) << "nb video devices:" << num_videoDevices;
        const uint32_t kSize = 256;
        char name[kSize] = {0};
        char id[kSize] = {0};

        size_t _width;
        size_t _height;

        _width =  (width<=0)? 1024: width;
        _height =  (height<=0)? 720: height;


        std::cout << "FILE: " << __FILE__ << "LINE: " << __LINE__ << std::endl;

        if (videourl.find("fake://") == 0) {
            int deviceNumber = atoi(videourl.substr(strlen("fake://")).c_str());
            RTC_LOG(LS_WARNING) << "device fake:" << deviceNumber;

            deviceId ="fake" + std::to_string(deviceNumber);
            std::cout << "FILE: " << __FILE__ << "LINE: " << __LINE__ << "deviceId: " << deviceId << std::endl;
        }

        if (deviceId.empty()) {
            RTC_LOG(LS_WARNING) << "device not found:" << videourl;
//            Destroy();
            return false;
        }

        std::string pipe_name = "/tmp/" + deviceId;

//        if ( mkfifo(pipe_name.c_str(), 0777) ) {
//            perror("mkfifo");
//            RTC_LOG(LS_ERROR) << "Error of mkfifo frame :  " << pipe_name;
//        } else {
//            RTC_LOG(LS_WARNING) << "Creat of mkfifo frame :  " << pipe_name;
//        }
//
//
//        if ( (fd = open(pipe_name.c_str(), O_RDONLY)) <= 0 ) {
//            RTC_LOG(LS_ERROR) << "Error of open pipe frame:  " << pipe_name;
//        } else {
//            RTC_LOG(LS_WARNING) << "Open pipe frame:  " << pipe_name;
//        }


//        byff = webrtc::I420Buffer::Create(width, height);
//        rtc::scoped_refptr<webrtc::I420Buffer> byff = webrtc::I420Buffer::Create(width, height);
//        webrtc::VideoFrame frame_tmp(byff, webrtc::kVideoRotation_0, rtc::TimeMicros());








    _thread = std::thread([this, _width, _height]() {

//        rtc::scoped_refptr<webrtc::I420Buffer> byff = webrtc::I420Buffer::Create(_width, _height);
//        rtc::scoped_refptr<webrtc::I420Buffer> byff = webrtc::I420Buffer::Create(400, 400);
//        webrtc::VideoFrame frame_tmp(byff, webrtc::kVideoRotation_0, rtc::TimeMicros());


//        VideoCapture cap;
//        Mat frame_cv(_height, _width, CV_8UC3, Scalar(0, 0, 0));
//        Mat frame_cv(_height, _width, CV_8UC1, Scalar(0, 0, 0));
        Mat mat_BGR2YUV_I420;



//        frame_cv = imread("../SMPTE_Color_Bars_480x320.png", cv::IMREAD_COLOR);


        if(!cap.isOpened()) {
            cap.open(
                    "rkisp device=/dev/video6 io-mode=4 ! video/x-raw,format=NV12"
                    //                "rkisp device=/dev/video5 io-mode=4 ! video/x-raw,format=YUYV"
                    ",width=" + std::to_string(_width)+
                    //                 ",height="+std::to_string(_height)+",framerate=30/1 ! videoconvert ! appsink"
                    ",height="+std::to_string(_height)+",framerate=30/1 ! appsink"

                    , cv::CAP_GSTREAMER);
        }


////        cap.open(0);
//        cap.open(
//                "rkisp device=/dev/video6 io-mode=4 ! video/x-raw,format=NV12"
////                "rkisp device=/dev/video5 io-mode=4 ! video/x-raw,format=YUYV"
//                 ",width=" + std::to_string(_width)+
////                 ",height="+std::to_string(_height)+",framerate=30/1 ! videoconvert ! appsink"
//                 ",height="+std::to_string(_height)+",framerate=30/1 ! appsink"
//
//                 , cv::CAP_GSTREAMER);


        int ix=1;
        int iy=1;
        int r = 20;
        int x = 2*r;
        int y = 2*r;

        isRun = true;
        while (isRun) {

            Point pt1, pt2;
            pt1.x = 0;
            pt1.y = 0;

            pt2.x = _width;
            pt2.y = _height;


//            rectangle(frame_cv,pt1,pt2,Scalar(0,0,0),-1 );
//            line(frame_cv,pt1,pt2,Scalar(255,255,255),1,8,0);

//            std::vector<Mat> YUV_I420_planes;


//            cv::Mat frame_cv(_height, _width, CV_8UC3, Scalar(0, 0, 0));
            cv::Mat frame_cv;

            cap >> frame_cv;
            cv::cvtColor(frame_cv, mat_BGR2YUV_I420, cv::COLOR_YUV2BGR_NV12);


            cv::circle(mat_BGR2YUV_I420, Point(x,y),r, Scalar(0,0,255),cv::FILLED );
//            cv::circle(frame_cv, Point(x,y),r, Scalar(0,255,0));

            if(x > _width - r )
                ix=-1;
            else if(x < r)
                ix = 1;

            if(y > _height - r )
                iy=-1;
            else if(y < r)
                iy = 1;

            x += ix;
            y += iy;



//            cv::cvtColor(frame_cv, mat_BGR2YUV_I420, cv::COLOR_YUV2BGR_NV12); // CV_BGR2YUV COLOR_BGR2YUV_I420
            cv::cvtColor(mat_BGR2YUV_I420, mat_BGR2YUV_I420, cv::COLOR_BGR2YUV_I420); // CV_BGR2YUV COLOR_BGR2YUV_I420
//            cv::cvtColor(frame_cv, mat_BGR2YUV_I420, cv::COLOR_NV12); // CV_BGR2YUV COLOR_BGR2YUV_I420
//            mat_BGR2YUV_I420 = frame_cv;


            unsigned char *pY = (unsigned char*)mat_BGR2YUV_I420.data;
            //Y plane as cv::Mat, resolution of srcY is 1280x720
            cv::Mat srcY = cv::Mat(cv::Size(_width, _height), CV_8UC1, (void*)pY);

            //U plane as cv::Mat, resolution of srcU is 640x360 (in memory buffer, U plane is placed after Y).
            cv::Mat srcU = cv::Mat(cv::Size(_width/2, _height/2), CV_8UC1, (void*)(pY + _width*_height));

            //V plane as cv::Mat, resolution of srcV is 640x360 (in memory buffer, V plane is placed after U).
            cv::Mat srcV = cv::Mat(cv::Size(_width / 2, _height / 2), CV_8UC1, (void*)(pY + _width*_height + (_width/2*_height/2)));




//            split(frame_cv, YUV_I420_planes);



//            cap >> frame_cv;
//            waitKey(30);

//            byff.get()->InitializeData();

//            if ( (len = read(fd, buf, BUFSIZE-1)) <= 0 ) {
//                perror("read");
//                close(fd);
//                remove(NAMEDPIPE_NAME);
//                return 0;
//            }



//            ConvertToI420(src_y, sample_size, dst_y_2, kDestWidth, dst_u_2,
//                          SUBSAMPLE(kDestWidth, SUBSAMP_X), dst_v_2,
//                          SUBSAMPLE(kDestWidth, SUBSAMP_X), 0, crop_y, kWidth, kHeight,
//                          kDestWidth, kDestHeight, libyuv::kRotate0, libyuv::FOURCC_ARGB);




//            cricket::FakeFrameSource frame_source_(640, 480, rtc::kNumMicrosecsPerSec / 30);
            cricket::FakeFrameSource frame_source_(_width, _height, rtc::kNumMicrosecsPerSec / 30);
            webrtc::VideoFrame frame = frame_source_.GetFrame();


//            rtc::scoped_refptr<webrtc::I420Buffer> byff = webrtc::I420Buffer::Create(640, 480);
//            webrtc::VideoFrame frame(byff, webrtc::kVideoRotation_0, rtc::TimeMicros());


            uint8_t  *p_y = (uint8_t  *)frame.video_frame_buffer().get()->GetI420()->DataY();
            uint8_t  *p_u = (uint8_t  *)frame.video_frame_buffer().get()->GetI420()->DataU();
            uint8_t  *p_v = (uint8_t  *)frame.video_frame_buffer().get()->GetI420()->DataV();

//            std::cout << "!!!!!!!!!!!!!!! frame.size: " << frame.size() << std::endl;
//            std::cout << "!!!!!!!!!!!!!!! YUV_I420_planes[0]: " << YUV_I420_planes[0].size << std::endl;
//            std::cout << "!!!!!!!!!!!!!!! YUV_I420_planes[1]: " << YUV_I420_planes[1].size << std::endl;
//            std::cout << "!!!!!!!!!!!!!!! YUV_I420_planes[2]: " << YUV_I420_planes[2].size << std::endl;
//
//            std::cout << "!!!!!!!!!!!!!!! mat_BGR2YUV_I420: " << mat_BGR2YUV_I420.size << std::endl;
//
//            std::cout << "!!!!!!!!!!!!!!! YUV_I420_planes: " << YUV_I420_planes.size() << std::endl;
//            std::cout << "!!!!!!!!!!!!!!! YUV_I420_planes 0: " << YUV_I420_planes[0].size << std::endl;
//            std::cout << "!!!!!!!!!!!!!!! YUV_I420_planes 1: " << YUV_I420_planes[1].size << std::endl;
//            std::cout << "!!!!!!!!!!!!!!! YUV_I420_planes 2: " << YUV_I420_planes[2].size << std::endl;


            int stride_y = frame.video_frame_buffer().get()->GetI420()->StrideY();
            int stride_u = frame.video_frame_buffer().get()->GetI420()->StrideU();
            int stride_v = frame.video_frame_buffer().get()->GetI420()->StrideV();
            int f_height = frame.video_frame_buffer().get()->GetI420()->height();
            int f_width = frame.video_frame_buffer().get()->GetI420()->width();


//            for (int sy=0; sy < f_height*f_width; sy++) {
            for (int sy=0; sy < _width*_height; sy++) {
                p_y[sy] = srcY.data[sy];
//                p_y[sy] = YUV_I420_planes[0].data[sy];
//                p_y[sy] = mat_BGR2YUV_I420.data[sy];
            }

            for (int sy=0; sy < _width*_height/2; sy++) {
                p_u[sy] = srcU.data[sy];
//                p_y[sy] = YUV_I420_planes[0].data[sy];
//                p_y[sy] = mat_BGR2YUV_I420.data[sy];
            }

//            for (int sy=0; sy < _width*(_height-1)/2; sy++) {
//                p_v[sy] = srcV.data[sy];
////                p_y[sy] = YUV_I420_planes[0].data[sy];
////                p_y[sy] = mat_BGR2YUV_I420.data[sy];
//            }






//            for (int sy=0; sy < f_height*f_width; sy++) {
//                p_y[sy] = YUV_I420_planes[0].data[sy];
//                p_y[sy] = mat_BGR2YUV_I420.data[sy];
//            }

//            for (int su=0; su < f_height*stride_u; su++) {
//                p_u[su] = YUV_I420_planes[1].data[su];
////                p_u[su] = mat_BGR2YUV_I420.data[su];
//            }
//
//            for (int sv=0; sv < ((f_height+1)/2)*(stride_v); sv++) {
//                p_v[sv] = YUV_I420_planes[2].data[sv];
//            }




//            for (int x = 0; x < f_width; x++) {
//                for (int y = 0; y < f_width; y++) {
//                    int i = x*f_width + y;
//
//                    p_y[i] = YUV_I420_planes[0].data[i];
//                    p_y[i + stride_y * f_height] = YUV_I420_planes[1].data[i];
//                    p_y[i + stride_y * f_height + stride_u * ((f_height + 1) / 2)] = YUV_I420_planes[2].data[i];
//
//                }
//            }


//            for(int i=0; i<frame.size(); i++)
//            {
////                p[i] = rand() % 255 ;
////                p[i] = mat_BGR2YUV_I420.data[i];
//                p_u[i] = mat_BGR2YUV_I420.data[i];
//            }


//            std::cout << "width: " << frame.width() <<  " height : " << frame.height() << std::endl;

            OnFrame(frame);
//            OnFrame(frame_tmp);
//            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });


        return true;
    }

//    void Destroy() {
//        if (m_vcm) {
//            m_vcm->StopCapture();
//            m_vcm->DeRegisterCaptureDataCallback();
//            m_vcm = nullptr;
//        }
//    }

    void AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink, const rtc::VideoSinkWants& wants) override {
        m_broadcaster.AddOrUpdateSink(sink, wants);
    }

    void RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink) override {
        m_broadcaster.RemoveSink(sink);
    }

    rtc::scoped_refptr<webrtc::VideoCaptureModule> m_vcm;
    rtc::VideoBroadcaster m_broadcaster;

    bool isRun;
    std::thread _thread;

    int fd, len;

    char *pipe_buf;
//    rtc::scoped_refptr<webrtc::I420Buffer> byff;

//    static VideoCapture cap;

};


VideoCapture FakeCapturer::cap;