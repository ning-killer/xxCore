#ifndef _RNE_NN_THREAD_HPP_
#define _RNE_NN_THREAD_HPP_
#include <thread>
#include <list>
#include <mutex>
#include <algorithm>
#include "ts_rne_c_api.h"
#include "ts_rne_nn_input.h"
#include "ts_rne_thread_safe_queue.hpp"

//singleton
class RneNNThread {
public:
    static RneNNThread *get_instance() {
        static RneNNThread _it;
        return &_it;
    }

    int start() {
        _is_exit = false;
        if (!_thd.joinable()) {
            _thd = std::thread(&RneNNThread::thread_proc, this);
        }
        return 0;
    }

    int end() {
        _ready.push(NULL);
        _is_exit = true;
        if (_thd.joinable()) {
            _thd.join();
        }
        return 0;
    }

    int push(RNE_ASYNC_EXECUTOR *async_etr) {
        _ready.push(async_etr);
        return 0;
    }

    enum State {
        RUNNING,
        DONE,
        ERROR
    };

    State query(RNE_ASYNC_EXECUTOR *async_etr) {
        State st = ERROR;
        std::lock_guard<std::mutex> lck(_mtx);
        auto it = std::find(_done.begin(), _done.end(), async_etr);
        if (it != _done.end()) {
            _done.erase(it);
            st = DONE;
        } else {
            st = RUNNING;
        }
        return st;
    }

private:
    RneNNThread() = default;
    ~RneNNThread() = default;
    RneNNThread(RneNNThread &other) = delete;
    void operator = (RneNNThread &other) = delete;

private:
    void get_ready() {
        RNE_ASYNC_EXECUTOR *async_etr = NULL;

        if (_run.empty()) {
            _ready.wait_and_pop(async_etr);
        } else {
            _ready.try_pop(async_etr);
        }
        if (!async_etr) return;

        auto it = _run.begin();
        while (it != _run.end()) {
            if (async_etr->priority < (*it)->priority) {
                break;
            }
            ++it;
        }
        _run.insert(it, async_etr);
        async_etr->state = RNE_NN_FORWARD_INIT;
    }

    void push_done(RNE_ASYNC_EXECUTOR *async_etr) {
        std::lock_guard<std::mutex> lck(_mtx);
        _done.push_back(async_etr);
    }

    int thread_proc() {
        while (!_is_exit) {
            get_ready();

            int times = 2;
            do {
                for (auto it = _run.begin(); it != _run.end();) {
                    int highest_priority = (*_run.begin())->priority;
                    RNE_ASYNC_EXECUTOR *async_etr = *it;

                    TS_MPI_TRP_RNE_ForwardAsync(async_etr);

                    switch (async_etr->state) {
                    case RNE_NN_FORWARD_FEED: {
                        async_etr->state = RNE_NN_FORWARD_READY;
                        TS_MPI_TRP_RNE_FillInputBlobs(async_etr->nn,
                                                      async_etr->core_idx[0],
                                                      async_etr->nn_in);
                        break;
                    }
                    case RNE_NN_FORWARD_DONE:
                        it = _run.erase(it);
                        push_done(async_etr);
                        continue;
                    case RNE_NN_FORWARD_RUNNING:
                        if (async_etr->priority > highest_priority) {
                            async_etr->state = RNE_NN_FORWARD_SUSPEND;
                            continue;
                        }
                        break;
                    case RNE_NN_FORWARD_WAIT:
                        if (async_etr->priority == highest_priority) {
                            async_etr->state = RNE_NN_FORWARD_RESUME;
                            continue;
                        }
                        break;
                    default:
                        break;
                    }
                    ++it;
                }
            } while (--times);
        }
        return 0;
    }

private:
    std::thread _thd;
    ThreadSafeQueue<RNE_ASYNC_EXECUTOR *> _ready;
    std::list<RNE_ASYNC_EXECUTOR *> _run;
    std::list<RNE_ASYNC_EXECUTOR *> _done;
    std::mutex _mtx;
    volatile bool _is_exit;
};

#endif
