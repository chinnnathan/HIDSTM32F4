#ifndef NCERR_H
#define NCERR_H
#include "stdint.h"


typedef enum 
{
    NC_NO_ERROR,
    NC_ERROR,
    NC_SUCCESS,
    NC_COMPLETE,
    NC_RESET,
    NC_INVALID
}err;

#endif