// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// SI7005
// This code is designed to work with the SI7005_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Temperature?sku=SI7005_I2CS#tabs-0-product_tabset-2

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

void main()
{
	// Create I2C bus
	int file;
	char *bus = "/dev/i2c-1";
	if((file = open(bus, O_RDWR)) < 0)
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, SI7005 I2C address is 0x40(64)
	ioctl(file, I2C_SLAVE, 0x40);

	// Send temperature measurement command(0x11)
	char config[2] = {0};
	config[0] = 0x03;
	config[1] = 0x11;
	write(file, config, 2); 
	sleep(1);

	// Read 3 bytes of data from register(0x00)
	// cTemp msb, cTemp lsb
	char reg[1] = {0x00};
	write(file, reg, 1);
	char data[3] = {0};
	if(read(file, data, 3) != 3)
	{
		printf("Erorr : Input/output Erorr \n");
	}
	else
	{
		// Checking the status, Poll RDY in status until it is low(=0)
		while((data[0] & 0x01) != 0)
		{
			read(file, data, 3);
		}

		// Convert the data
		float cTemp = (((data[1] * 256 + (data[2]  & 0xFC)) / 4.0) / 32.0) - 50.0;
		float fTemp = cTemp * 1.8 + 32;

		// Output data to screen
		printf("Temperature in Celsius    : %.2f \n", cTemp);
		printf("Temperature in Fahrenheit : %.2f \n", fTemp);
	}

	// Send humidity measurement command
	config[0] = 0x03;
	config[1] = 0x01;
	write(file, config, 2); 
	sleep(1);

	// Read 2 bytes of data from register(0x00)
	// humidity msb, humidity lsb
	reg[0] = 0x00;
	write(file, reg, 1);
	if(read(file, data, 3) != 3)
	{
		printf("Erorr : Input/output Erorr \n");
	}
	else 
	{
		// Checking the status, Poll RDY in status until it is low(=0)
		while((data[0] & 0x01) != 0)
		{
			read(file, data, 3);
		}

		// Convert the data to 12-bits
		double humidity = (((data[1] * 256 + (data[2] & 0xF0)) / 16.0) / 16.0) - 24.0;

		// Output data to screen
		printf("Relative Humidity : %.2f \n", humidity);
	}
}
