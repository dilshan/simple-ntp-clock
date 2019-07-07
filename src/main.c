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
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <malloc.h>
#include <stdlib.h>

#include "main.h"

int main()
{
    STATUS configStatus, ntpStatus;
    unsigned short retryCount = 0;
    int returnCode = 0;
    pthread_t displayThread;
    runTimeStruct sysStruct;

    // Load values of config.json into configuration data structure.
    sysStruct.displayHandle = -1;

    configStatus = readConfigData(CONFIG_FILE, &(sysStruct.sysConfig));
    if(configStatus != SUCCESS)
    {
        // Check for valid NTP server address entry.
        if(sysStruct.sysConfig.ntpServerAddr == NULL)
        {
            fprintf(stderr, "serverAddress is not defined in configuration or invalid configuration object\n");
            returnCode = 1;
            goto exit;
        }

        // Check for valid display port value.
        if(sysStruct.sysConfig.displayPanelPort == NULL)
        {
            fprintf(stderr, "devicePort is not defined in configuration or invalid configuration object\n");
            returnCode = 1;
            goto exit;
        }

        // Check for time zone configuration.
        if(sysStruct.sysConfig.timeZone == NULL)
        {
            fprintf(stderr, "Time zone is not defined or invalid configuration object\n");
            returnCode = 1;
            goto exit;
        }

        // Open USB display panel port.
        sysStruct.displayHandle = openDisplayModule(sysStruct.sysConfig.displayPanelPort);
        if(sysStruct.displayHandle < 0)
        {
            fprintf(stderr, "Unable to open display panel port %s\n", sysStruct.sysConfig.displayPanelPort);
            returnCode = 1;
            goto exit;
        }

        // Update process TZ from specified time zone.
        setenv("TZ", sysStruct.sysConfig.timeZone, 1);
        tzset();

        // Clear content of the display module.
        if(clearDisplayModule(sysStruct.displayHandle) != SUCCESS)
        {
            fprintf(stderr, "Unable to clear display module.");
            returnCode = 1;
            goto exit;
        }

        printf("Connecting to %s to get current time...\n", sysStruct.sysConfig.ntpServerAddr);

        if(sysStruct.sysConfig.retryCount == 0)
        {
            sysStruct.sysConfig.retryCount = 1;
        }

        // Try to get time from NTP server.
        while(retryCount < sysStruct.sysConfig.retryCount)
        {
            ntpStatus = syncTimeServer(&(sysStruct.ntpTime), sysStruct.sysConfig.ntpServerAddr, sysStruct.sysConfig.ntpPort);
            if(ntpStatus == SUCCESS)
            {                
                // Time received from the NTP server.
                printf("Received time from NTP server: %s\n", ctime((const time_t*) &(sysStruct.ntpTime)));
                retryCount = 0;

                // Subscribe for termination signal.
                signal(SIGINT, terminationHandler);

                // Create display thread to show and update time.
                if(pthread_create(&displayThread, NULL, displayRoutine, (void*)&sysStruct) == 0)
                {
                    // Waiting for display thread to finish.
                    pthread_join(displayThread, NULL);

                    // Shutting down..., Perform cleanup and reset.
                    printf("\nShutting shown display module and cleanup...\n");                                        
                }
                else
                {
                    // Thread creation failed!!!
                    fprintf(stderr, "Unable to create display thread.\n");
                    returnCode = 1;
                    goto exit;
                }

                break;
            }

            // Last NTP request is not suucessful, looking for another try...
            retryCount++;
            printf("Unable to connect with NTP server retry [%d/%d]...\n", retryCount, sysStruct.sysConfig.retryCount);
        }

        if(retryCount >= sysStruct.sysConfig.retryCount)
        {
            // Maximum retry count reached and terminate the application.
            fprintf(stderr, "Unable to connect with NTP server %s:%d", sysStruct.sysConfig.ntpServerAddr, sysStruct.sysConfig.ntpPort);
            returnCode = 1;
            goto exit;
        }               
    }
    else
    {
        // Configuration file read / parse failed.
        fprintf(stderr, "Unable to read configuration file.\n");
        returnCode = 1;
        goto exit;
    }

exit:
    // Clean up allocated resources.
    if(sysStruct.displayHandle > -1)
    {
        clearDisplayModule(sysStruct.displayHandle);
        closeDisplayModule(&(sysStruct.displayHandle)); 
    }

    if(sysStruct.sysConfig.displayPanelPort != NULL)
    {
        free(sysStruct.sysConfig.displayPanelPort);
        sysStruct.sysConfig.displayPanelPort = NULL;
    }
    
    if(sysStruct.sysConfig.ntpServerAddr != NULL)
    {
        free(sysStruct.sysConfig.ntpServerAddr);
        sysStruct.sysConfig.ntpServerAddr = NULL;
    }

    if(sysStruct.sysConfig.timeZone != NULL)
    {
        free(sysStruct.sysConfig.timeZone);
        sysStruct.sysConfig.timeZone = NULL;
    }

    return returnCode;
}

void terminationHandler(int termSignal)
{
    isTerminate = 1;
}

void *displayRoutine(void *arg)
{
    runTimeStruct* sysStruct = arg;
    char timeBuffer[11];
    struct tm* timeInfo;

    // Loop until termination signal.
    while(isTerminate != 1)
    {        
        // Convert time and format as string to display on module.
        timeInfo = localtime((const time_t*)&(sysStruct->ntpTime));
        sprintf(timeBuffer, "%02d.%02d   %02d", timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
        if(showMessage(sysStruct->displayHandle, timeBuffer) != SUCCESS)
        {
            // Display device is not available or I/O error.
            break;
        }

        // Wait 1 second for next time update.
        sleep(1);
        sysStruct->ntpTime++;
    }
}