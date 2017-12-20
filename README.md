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

## Library bulided on Visual Studio 2017
Library compiled for x32, x64 and x32 for WindowsXP (not tested) platforms. Library folder contains all compiled libraries and binary examples for x32 and x64 platform, not x32 Windows XP. In the example_src folder you may find the example project created with Qt 5.10 libs and compiled in Visual Studio 2017.
