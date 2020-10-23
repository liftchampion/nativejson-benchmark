//
// Created by gg on 10/23/20.
//

#pragma once

#include <stdint.h>
#include <x86intrin.h>
#include <sys/time.h>

class Timer2 {
  public:
    Timer2() : start_(), end_() {
        start_ = __rdtscp(&aux_);
        end_   = __rdtscp(&aux_);
    }

    void Start() {
        asm volatile("" ::: "memory");
        start_ = __rdtscp(&aux_);
        asm volatile("" ::: "memory");
    }

    void Stop() {
        asm volatile("" ::: "memory");
        end_   = __rdtscp(&aux_);
        asm volatile("" ::: "memory");
    }

    double GetElapsedMilliseconds() {
        return ((double)(end_ - start_)) / 3600.;
    }

  private:
    uint64_t start_ = 0;
    uint64_t end_ = 0;
    uint32_t aux_ = 0;
};

