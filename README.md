ColibriNANO_lib
======

Library for operation with SDR ColibriNANO receiver.

ColibriNANO: https://eesdr.com

Internal ADC clock frequency 122.88 MHz, it determines the Nyquist zone bandwidth, ADC clock/2 = 61.44 MHz. ColibriNANO can receive only in one Nyquist zone at the same time e.g. 0-61.44 MHz, 61.44-122.88 MHz, 122.88-184.32 MHz etc. In the first Nyquist zone internal LPF works automatically, in the second and higher Nyquist zones LPF is disabled. This library allows you to control ColibriNANO receiver, change tuning frequency, preamplifier gain/attenuation, receive IQ stream with adjustable sample rate and receive ADC overload status.

### ColibriNANO receiver supports the following sample rates:
* 48 kHz
* 96 kHz
* 192 kHz
* 384 kHz
* 768 kHz
* 1536 kHz
* 1920 kHz
* 2560 kHz
* 3072 kHz

>**Note **
Some PC won't be able to support highest sample rates values, it depends on the USB port quality.

## Library for Windows OS built on Visual Studio 2017
Library compiled for x32, x64 and x32 for WindowsXP (not tested) platforms. Library folder contains all compiled libraries and binary examples for x32 and x64 platform, not x32 Windows XP. In the example_src folder you may find the example project created with Qt 5.10 libs and compiled in Visual Studio 2017.

## Library for Linux OS x64 built on Ubuntu 18.04.1 with GCC 7.1
ColibriNANO receiver on OS Linux you have to disable FTDI kernal module, which determines this chip as COM-port, 
to do that run this command in terminal as root

> sudo rmmod ftdi_sio

Now you have to determine access right to the device, create the ftdi.rules file (create and edit in gedit software)

> sudo gedit /etc/udev/rules.d/ftdi.rules

Add this line 

SUBSYSTEM=="usb", ATTR{idVendor}=="0403", ATTR{idProduct}=="6014", MODE="0666", GROUP="plugdev"

Save and close the file
Now you have to reload rules by runnig this command

> sudo udevadm control —reload-rules

After that launch the ExpertSDR2 or ExpertRS, now software should detect the receiver and can work with it, if software didn't detect it or but failed to run it, repeat this command

> sudo rmmod ftdi_sio

After each PC reload you should repeat this command 

> sudo rmmod ftdi_sio
