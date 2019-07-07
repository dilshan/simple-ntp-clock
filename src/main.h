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

#ifndef MAIN_H
#define MAIN_H

#include "ntp_client.h"
#include "json_proc.h"
#include "display_module.h"

#define CONFIG_FILE "config.json"

volatile sig_atomic_t isTerminate;

typedef struct 
{
    int displayHandle;
    time_t ntpTime;    
    configRec sysConfig;
} runTimeStruct;

void *displayRoutine(void *arg);
void terminationHandler(int termSignal);

#endif /* MAIN_H */