//
// Created by tymbys on 13.04.20.
//

#include "VideoScalerSetup.h"

std::atomic<VideoScalerSetup*> VideoScalerSetup::instance;
std::mutex VideoScalerSetup::Mutex;