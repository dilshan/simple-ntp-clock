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
#include <malloc.h>
#include <string.h>

#include "jsmn.h"
#include "json_proc.h"

static int jsoneq(const char *json, jsmntok_t *tok, const char *str) 
{
    return (tok->type == JSMN_STRING && (int)strlen(str) == tok->end - tok->start && strncmp(json + tok->start, str, tok->end - tok->start) == 0) ? 0 : -1;
}

void extractValue(const char* json, jsmntok_t *tok, int elementPos, char** valueBuffer)
{
    size_t valueSize = tok->end - tok->start;
    *valueBuffer = malloc(valueSize + 1);
    memcpy(*valueBuffer, json + (tok->start), valueSize);
    (*valueBuffer)[valueSize] = '\0';
}

int extractNumber(const char* json, jsmntok_t *tok, int elementPos)
{
    char *tempData = NULL;
    int returnVal = 0;

    extractValue(json, tok, elementPos, &tempData);
    if(tempData != NULL)
    {
        returnVal = strtol(tempData, (char **)NULL, 10);
        free(tempData);
        tempData = NULL;
    }

    return returnVal;
}

STATUS loadConfigData(const char* configFilePath, char** buffer, size_t* bufferSize)
{
    size_t readLen = 0;
    FILE* fileHandle = NULL;
    STATUS returnStatus = SUCCESS;

    if(configFilePath != NULL)
    {
        fileHandle = fopen(configFilePath, "r");        
        *bufferSize = 0;

        // Check for valid file handle is assigned by fopen().
        if(fileHandle != NULL)
        {
            // Move file pointer to end of file.
            if (fseek(fileHandle, 0, SEEK_END) == 0)
            {
                // Get content size.
                *bufferSize = ftell(fileHandle);
                if(bufferSize > 0)
                {
                    // Create buffer based on content size.
                    *buffer = malloc((*bufferSize) + 1);
                    
                    // Reset file pointer and start reading...
                    if (fseek(fileHandle, 0, SEEK_SET) != 0)
                    {
                        returnStatus = ERR_INVALID_FILE;
                        goto exit;
                    }

                    readLen = fread(*buffer, sizeof(char), (*bufferSize), fileHandle);                    
                    if (ferror(fileHandle) != 0)
                    {
                        returnStatus = ERR_FILE_READ;
                        goto exit;
                    }
                    else
                    {
                        // Assign end of string marker.
                        (*buffer)[readLen++] = '\0';
                    }
                }
                else
                {
                    returnStatus = ERR_INVALID_FILE;
                    goto exit;
                }
            }
        }
    }

exit:

    if(fileHandle != NULL)
    {
        fclose(fileHandle);
        fileHandle = NULL;
    }

    return returnStatus;
}

STATUS readConfigData(const char* configFilePath, configRec* config)
{    
    char* dataBuffer = NULL;
    int root = 0;
    int elementPos = 0;
    size_t dataBufferSize = 0;    
    jsmn_parser parser;
    jsmntok_t tokenCollection[64];
    STATUS returnStatus = ERR_INVALID_PARAM;

    // Initialize configuration structure with defaults.
    config->ntpServerAddr = NULL;
    config->displayPanelPort = NULL;
    config->timeZone = NULL;
    config->ntpPort = 0;
    config->retryCount = 0;
    
    if(configFilePath != NULL)
    {
        returnStatus = SUCCESS;
        
        // Load JSON file into string buffer.
        STATUS fileStatus = loadConfigData(configFilePath, &dataBuffer, &dataBufferSize);

        if(fileStatus != SUCCESS)
        {
            returnStatus = fileStatus;
            goto exit;
        }

        if((dataBuffer == NULL) || (dataBufferSize == 0))
        {
            returnStatus = ERR_FILE_EMPTY;
            goto exit;
        }

        jsmn_init(&parser);
        root = jsmn_parse(&parser, dataBuffer, strlen(dataBuffer), tokenCollection, sizeof(tokenCollection) / sizeof(tokenCollection[0]));

        // Validate JSON document.
        if (root < 0)
        {
            returnStatus = ERR_INVALID_JSON;
            goto exit;
        }

        if ((root < 1) || tokenCollection[0].type != JSMN_OBJECT)
        {
            // Top level element must be object.
            returnStatus = ERR_INVALID_JSON;
            goto exit;
        }

        for (elementPos = 1; elementPos < root; elementPos++) 
        {            
            if (jsoneq(dataBuffer, &tokenCollection[elementPos], "serverAddress") == 0)
            {
                // Extract value of 'serverAddress' from JSON buffer.
                extractValue(dataBuffer, &(tokenCollection[elementPos + 1]), elementPos, &(config->ntpServerAddr));
                elementPos++;
            }
            else if(jsoneq(dataBuffer, &tokenCollection[elementPos], "serverPort") == 0)
            {
                // Extract value of 'serverPort' from JSON buffer.
                config->ntpPort = extractNumber(dataBuffer, &(tokenCollection[elementPos + 1]), elementPos);
                elementPos++;
            }
            else if(jsoneq(dataBuffer, &tokenCollection[elementPos], "devicePort") == 0)
            {
                // Extract value of 'devicePort' from JSON buffer. 
                extractValue(dataBuffer, &(tokenCollection[elementPos + 1]), elementPos, &(config->displayPanelPort));
                elementPos++;
            }
            else if(jsoneq(dataBuffer, &tokenCollection[elementPos], "retryAttempt") == 0)
            {
                // Extract value of 'retryAttempt' from JSON buffer.
                config->retryCount = extractNumber(dataBuffer, &(tokenCollection[elementPos + 1]), elementPos);
                elementPos++;
            }
            else if (jsoneq(dataBuffer, &tokenCollection[elementPos], "timeZone") == 0)
            {
                // Extract value of 'timeZone' from JSON buffer.
                extractValue(dataBuffer, &(tokenCollection[elementPos + 1]), elementPos, &(config->timeZone));
                elementPos++;
            }
        }        
    }

exit:

    if(dataBuffer != NULL)
    {
        free(dataBuffer);
        dataBuffer = NULL;
    }

    return ERR_INVALID_PARAM;
}