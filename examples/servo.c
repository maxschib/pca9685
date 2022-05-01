/*************************************************************************
 * servo.c
 *
 * PCA9685 servo example
 * Connect a servo to any pin. It will rotate to random angles.
 *
 *
 * This software is a devLib extension to wiringPi <http://wiringpi.com/>
 * and enables it to control the Adafruit PCA9685 16-Channel 12-bit
 * PWM/Servo Driver <http://www.adafruit.com/products/815> via I2C interface.
 *
 * Copyright (c) 2014 Reinhard Sprung
 *
 * If you have questions or improvements email me at
 * reinhard.sprung[at]gmail.com
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The given code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You can view the contents of the licence at <http://www.gnu.org/licenses/>.
 **************************************************************************
 */

#include "pca9685.h"

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 50

#define L_EN 28
#define R_EN 29
#define MOTOR_1_L 0
#define MOTOR_1_R 1

struct termios orig_termios;

void reset_terminal_mode()
{
	tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
	struct termios new_termios;

	/* take two copies - one for now, one for later */
	tcgetattr(0, &orig_termios);
	memcpy(&new_termios, &orig_termios, sizeof(new_termios));

	/* register cleanup handler, and set the new terminal mode */
	atexit(reset_terminal_mode);
	cfmakeraw(&new_termios);
	tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
	struct timeval tv = {0L, 0L};
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	return select(1, &fds, NULL, NULL, &tv) > 0;
}

int dataRdy(int fd)
{
	struct timeval tv = {0L, 0L};
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	return select(fd + 1, &fds, NULL, NULL, &tv);
}

#define NO_KEY_PRESSED -1

int getch(int fd)
{
	int r;
	unsigned char c;
	if ((r = read(fd, &c, sizeof(c))) < 0)
	{
		return NO_KEY_PRESSED;
	}
	else
	{
		return c;
	}
}

/**
 * Calculate the number of ticks the signal should be high for the required amount of time
 */
int calcTicks(float impulseMs, int hertz)
{
	float cycleMs = 1000.0f / hertz;
	return (int)(MAX_PWM * impulseMs / cycleMs + 0.5f);
}

/**
 * input is [0..1]
 * output is [min..max]
 */
float map(float input, float min, float max)
{
	return (input * max) + (1 - input) * min;
}



int main(void)
{
	int c;

	printf("PCA9685 servo example\r\n");
	printf("Press ctrl-C to finish\r\n");

	set_conio_terminal_mode();

	
	// printf("Connect a servo to any pin. It will rotate to random angles\n");

	
	wiringPiSetup(); // Initializes wiringPi using wiringPi's simlified number system.
	// wiringPiSetupGpio(); // Initializes wiringPi using the Broadcom GPIO pin numbers

	pinMode(L_EN, OUTPUT);
	pinMode(R_EN, OUTPUT);

	// Setup with pinbase 300 and i2c location 0x40
	int fd = pca9685Setup(PIN_BASE, 0x40, HERTZ);
	if (fd < 0)
	{
		printf("Error in setup\n");
		return fd;
	}

	// Reset all output
	pca9685PWMReset(fd);

	// Set servo to neutral position at 1.5 milliseconds
	// (View http://en.wikipedia.org/wiki/Servo_control#Pulse_duration)
	// float millis = 1.5;
	// int tick = calcTicks(millis, HERTZ);
	// pwmWrite(PIN_BASE + 16, tick);
	// delay(2000);

	int done = 0;
	while (!done)
	{
		c = getch(0);

		switch (c)
		{
		case 3: // ctrl-C
			digitalWrite(L_EN, LOW);
			digitalWrite(R_EN, LOW);
			pwmWrite(PIN_BASE + MOTOR_1_R, 0);
			pwmWrite(PIN_BASE + MOTOR_1_L, 0);
			done = 1;
			break;

		case 'd':
		case 'D':
			// printf("d pressed\r\n");
			printf("Disabling bts7960\r\n");
			digitalWrite(L_EN, LOW);
			digitalWrite(R_EN, LOW);
			break;

		case 'e':
			// printf("e pressed\r\n");
			printf("Enabling bts7960\r\n");
			digitalWrite(L_EN, HIGH);
			digitalWrite(R_EN, HIGH);
			break;

		case 'q':
		case 'Q':
			printf("Left Direction \r\n");
			pwmWrite(PIN_BASE + MOTOR_1_R, 0);
			pwmWrite(PIN_BASE + MOTOR_1_L, 2000);
			break;

		case 'a':
		case 'A':
			printf("Right Direction\r\n");
			pwmWrite(PIN_BASE + MOTOR_1_L, 0);
			pwmWrite(PIN_BASE + MOTOR_1_R, 2000);
			break;

		case 's':
		case 'S':
			printf("Stopping\r\n");
			pwmWrite(PIN_BASE + MOTOR_1_L, 0);
			pwmWrite(PIN_BASE + MOTOR_1_R, 0);
			break;
		}

		// That's a hack. We need a random number < 1
		// float r = rand();
		// while (r > 1)
		// 	r /= 10;

		// millis = map(r, 1, 2);
		// tick = calcTicks(millis, HERTZ);

		// pwmWrite(PIN_BASE + 16, tick);

		// delay(1000);
		usleep(1000); // sleep for a millisecond
	}

	return 0;
}
