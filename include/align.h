#pragma once

#define ALIGN_SIZE_DEFAULT 16

#define ALIGN_OFFSET(off, align_size) (\
    ((off) + (align_size) - 1) &~ ((align_size) - 1)\
)

#define IS_POWER_OF_2(num) (\
    ((num) > 0) && \
    (((num) & ((num) - 1)) == 0)\
)

