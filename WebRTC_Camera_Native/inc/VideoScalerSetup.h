//
// Created by tymbys on 13.04.20.
//

#pragma once


#include <atomic>
#include <iostream>
#include <future>
#include <mutex>

struct ROI {

    int roi_x;
    int roi_y;
    int roi_width;
    int roi_height;

};



class VideoScalerSetup {
public:
    static VideoScalerSetup* get(){
        VideoScalerSetup* sin = instance.load(std::memory_order_acquire);
        if ( !sin ){
            std::lock_guard<std::mutex> Lock(Mutex);
            sin = instance.load(std::memory_order_relaxed);
            if( !sin ){
                sin = new VideoScalerSetup();
                instance.store(sin, std::memory_order_release);
            }
        }

        return sin;
    }

    bool SetCrop(ROI roi) {
        _roi = roi;

        return true;
    }

    ROI GetCrop() {
        return _roi;
    }


private:
    VideoScalerSetup()= default;
    ~VideoScalerSetup()= default;
    VideoScalerSetup(const VideoScalerSetup&)= delete;
    VideoScalerSetup& operator=(const VideoScalerSetup&)= delete;

    static std::atomic<VideoScalerSetup*> instance;
    static std::mutex Mutex;

    ROI _roi;
};


//std::atomic<VideoScalerSetup*> VideoScalerSetup::instance;
//std::mutex VideoScalerSetup::Mutex;

