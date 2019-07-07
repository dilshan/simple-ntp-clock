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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "ntp_client.h"

STATUS syncTimeServer(time_t *rxTime, const char* ntpHost, unsigned short port)
{
    // This routine is based on an article about NTP client written by David Lettier.
    // https://lettier.github.io/posts/2016-04-26-lets-make-a-ntp-client-in-c.html
    
    int socketHandle;
    STATUS returnStatus = SUCCESS;
    struct sockaddr_in serverAddr;
    struct hostent *server;
    NTPPacket packet;
    
    // Initialize packet content.
    memset(&packet, 0, sizeof(NTPPacket));
    packet.leapVersionMode = 0x1b;

    // Open UDP socket to send the NTP request.
    socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socketHandle < 0)
    {
        returnStatus = ERR_SOCKET;
        goto exit;
    }

    // Get IP address from URL.
    server = gethostbyname(ntpHost);
    if(server == NULL)
    {
        returnStatus = ERR_NO_HOST;
        goto exit;
    }
    
    memset((char*) &serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;

    memcpy((char*) &serverAddr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);
    serverAddr.sin_port = htons(port);

    if(connect(socketHandle, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0)
    {
        returnStatus = ERR_CONNECT;
        goto exit;
    }

    // Write NTP request to socket.
    if(write(socketHandle, (char*) &packet, sizeof(NTPPacket)) < 0)
    {
        returnStatus = ERR_SOCKET_WRITE;
        goto exit;
    }

    // Waiting for results from NTP server.
    if(read(socketHandle, (char*) &packet, sizeof(NTPPacket)) < 0)
    {
        returnStatus = ERR_SOCKET_READ;
        goto exit;
    }

    // Update struct with received time and apply corrections.
    packet.txTimeStampHi = ntohl(packet.txTimeStampHi);
    packet.txTimeStampLo = ntohl(packet.txTimeStampLo);
    *rxTime = (time_t) (packet.txTimeStampHi - NTP_TIMESTAMP_DELTA);

exit:
    return returnStatus;
}