//**********************************************************************
// Copyright (c) 2019 Dilshan R Jayakody. [jayakody2000lk@gmail.com]
//
// Permission is hereby granted, free of charge, to any person 
// obtaining a copy of this software and associated documentation 
// files (the "Software"), to deal in the Software without restriction, 
// including without limitation the rights to use, copy, modify, merge, 
// publish, distribute, sublicense, and/or sell copies of the Software, 
// and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be 
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
//**********************************************************************

#ifndef NTP_CLIENT_H
#define NTP_CLIENT_H

#include "main_err_def.h"
#include <time.h>

// Timestamp for 1st of January 1970.
#define NTP_TIMESTAMP_DELTA 2208988800ull

typedef struct
  {
    unsigned char leapVersionMode;

    unsigned char stratum;
    unsigned char poll;
    unsigned char precision;

    unsigned int rootDelay;
    unsigned int rootDispersion;
    unsigned int refId;

    unsigned int refTimeStampHi;
    unsigned int refTimeStampLo;

    unsigned int origTimeStampHi; 
    unsigned int origTimeStampLo;

    unsigned int rxTimeStampHi;
    unsigned int rxTimeStampLo;

    unsigned int txTimeStampHi;
    unsigned int txTimeStampLo;

} NTPPacket;

STATUS syncTimeServer(time_t *rxTime, const char* ntpHost, unsigned short port);

#endif /* NTP_CLIENT_H */