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

#ifndef MAIN_ERR_H
#define MAIN_ERR_H

#define SUCCESS             0x000
#define FAIL                0x0FF

#define ERR_SOCKET          0x020
#define ERR_NO_HOST         0x021
#define ERR_CONNECT         0x022
#define ERR_SOCKET_WRITE    0x023
#define ERR_SOCKET_READ     0x024

#define ERR_INVALID_PARAM   0x030
#define ERR_INVALID_FILE    0x031
#define ERR_FILE_READ       0x032
#define ERR_FILE_EMPTY      0x033

#define ERR_INVALID_JSON    0x040

#define ERR_INVALID_HANDLE  0x050
#define ERR_INVALID_BUFFER  0x051

#define STATUS unsigned short

#endif /* MAIN_ERR_H */