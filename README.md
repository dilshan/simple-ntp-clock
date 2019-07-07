# Simple NTP Clock

Simple NTP clock is a maintenance-free clock application developed to work on single-board computers like *Raspberry Pi*, *Orange Pi*, etc. This clock application uses the *Simple Network Time Protocol* (SNTP) to get the time and display it on [seven segment display](https://github.com/dilshan/usb-external-display) which I designed. 

This application is designed to work on most of the Linux based systems and had minimum dependencies with system libraries and peripherals. I developed this application to work with *Allwinner H2 Plus* based *[Orange Pi Zero](http://www.orangepi.org/orangepizero/)* board, but this can compile for other platforms without doing any modifications on the source code.

To compile this application issue the following commands:

```
make clean
make
```

Parameters related to this application are available at *config.json* file. This file can be used to specify the NTP server, time zone, and display module port. 

```
{
    "serverAddress": "lk.pool.ntp.org",
    "serverPort": 123,
    "timeZone": "Asia/Colombo",
    "retryAttempt": 3,
    "devicePort": "/dev/ttyACM0"
}
```



