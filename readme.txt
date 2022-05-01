https://learn.adafruit.com/adafruit-16-channel-servo-driver-with-raspberry-pi

sudo i2cdetect -y 0
Error: Could not open file `/dev/i2c-0' or `/dev/i2c/0': No such file or directory

sudo i2cdetect -y 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --


This will search /dev/i2c-0 or /dev/i2c-1 for all address, and if an Adafruit PWM breakout is properly connected and it's set to it's default address 
-- meaning none of the 6 address solder jumpers at the top of the board have been soldered shut -- it should show up at 0x40 (binary 1000000) as follows

---------------------------------------------------------------------------------------
https://github.com/maxschib/pca9685

This will install pca9685 in your /usr/lib, /usr/local/lib and /usr/local/include directories
sudo make install

