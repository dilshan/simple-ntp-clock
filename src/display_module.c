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

#include <fcntl.h> 
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include "display_module.h"

int openDisplayModule(char* path)
{
    int deviceHandle;
    struct termios tty;
    STATUS openStatus = SUCCESS;

    // Open specified USB serial path.
    deviceHandle = open(path, O_RDWR | O_NOCTTY | O_SYNC);
    if(deviceHandle < 0)
    {
        openStatus = FAIL;
        goto exit;
    }

    // Load serial terminal parameters.
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(deviceHandle, &tty) != 0)
    {
        openStatus = FAIL;
        goto exit;
    }

    // Set serial baud rate to 115200.
    cfsetospeed (&tty, B115200);
    cfsetispeed (&tty, B115200);

    // Updating serial structure with default configuration.   
    tty.c_iflag &= ~IGNBRK;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_lflag = 0; 

    tty.c_oflag = 0;

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 5; 

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;    
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag |= 0;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    // Setting up serial parameters. 
    if (tcsetattr(deviceHandle, TCSANOW, &tty) != 0)
    {
        openStatus = FAIL;
        goto exit;
    }

exit:

    if(openStatus == FAIL)
    {
        // Close device handle due to error.
        closeDisplayModule(&deviceHandle);
        return 0;
    }
    else
    {
        return deviceHandle;
    }
}

STATUS writeDisplayModule(int deviceHandle, char* buffer, size_t bufferSize)
{
    STATUS returnStatus = SUCCESS;

    if(deviceHandle < 0)
    {
        returnStatus = ERR_INVALID_HANDLE;
        goto exit;
    }

    if((bufferSize > 0) && (buffer != NULL))
    {
        write(deviceHandle, buffer, bufferSize);
    }
    else
    {
        returnStatus = ERR_INVALID_BUFFER;
        goto exit;
    }
    
exit:
    return returnStatus;
}

void closeDisplayModule(int* deviceHandle)
{
    if(*deviceHandle > -1)
    {
        close(*deviceHandle);
        *deviceHandle = 0;
    }
}

STATUS clearDisplayModule(int deviceHandle)
{
    char clearInst[2] = {0x5E, 0x30};

    return writeDisplayModule(deviceHandle, clearInst, 2);
}

STATUS showMessage(int deviceHandle, char* msg)
{
    STATUS returnStatus = SUCCESS;
    size_t bufferSize = strlen(msg);
    char displayInst[12] = {0x5E, 0x31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if(bufferSize == 0)
    {
        returnStatus = ERR_INVALID_BUFFER;
        goto exit;
    }

    // Limit display buffer to 10 digits.
    if(bufferSize > 10)
    {
        bufferSize = 10;
    }

    // Copy display content to the buffer to write to port.
    memcpy(displayInst + 2, msg, bufferSize);
    returnStatus = writeDisplayModule(deviceHandle, displayInst, 12);

exit:
    return returnStatus;
}