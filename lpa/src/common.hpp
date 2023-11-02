#pragma once

#include<cstdlib>

inline unsigned int b_rand(unsigned int b) {
    unsigned int thresh = -b % b;

    for(;;) {
        unsigned int r = rand();
        if(r >= thresh) {
            return r % b;
        }
    }
}

