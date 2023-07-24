//
// Created by xiong on 2021/6/23.
//
#include <unistd.h>
#include "LogInner.hpp"

using namespace Emx;

ErrCodeE LogInner::ParseOpt(int argc, char **argv, Log::Config &cfg) {
    for (;;) {
        int c = getopt(argc, argv,
                       "l:a:s:n:m:f:");
        if (c < 0)
            break;
        switch (c) {
            case 'l':
                cfg.shared.level = (Log::LevelE) strtol(optarg, nullptr, 10);
                break;
            case 'a':
                cfg.shared.async = (bool) strtol(optarg, nullptr, 10);
                break;
            case 's':
                cfg.normal.maxSizeKB = strtol(optarg, nullptr, 10);
                break;
            case 'n':
                strncpy(cfg.normal.path, optarg, EMX_MAX_PATH_SIZE - 1);
                break;
            case 'm':
                cfg.flash.maxSizeKB = strtol(optarg, nullptr, 10);
                break;
            case 'f':
                strncpy(cfg.flash.path, optarg, EMX_MAX_PATH_SIZE - 1);
                break;
            default:
                break;
        }
    }
    return ErrCodeE::Success;
}
