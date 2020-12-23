//
// Created by tymbys on 15.04.20.
//

#pragma once


#include <atomic>
#include <iostream>
#include <future>
#include <mutex>
#include <queue>


class FakeCapturerQueue {
public:
    static FakeCapturerQueue* get(){
        FakeCapturerQueue* sin = instance.load(std::memory_order_acquire);
        if ( !sin ){
            std::lock_guard<std::mutex> Lock(Mutex);
            sin = instance.load(std::memory_order_relaxed);
            if( !sin ){
                sin = new FakeCapturerQueue();
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
    FakeCapturerQueue()= default;
    ~FakeCapturerQueue()= default;
    FakeCapturerQueue(const FakeCapturerQueue&)= delete;
    FakeCapturerQueue& operator=(const FakeCapturerQueue&)= delete;

    static std::atomic<FakeCapturerQueue*> instance;
    static std::mutex Mutex;

    queue<string> myQueue;
};