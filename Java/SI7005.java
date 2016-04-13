// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// SI7005
// This code is designed to work with the SI7005_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Temperature?sku=SI7005_I2CS#tabs-0-product_tabset-2

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import java.io.IOException;

public class SI7005
{
	public static void main(String args[]) throws Exception
	{
		// Create I2C bus
		I2CBus Bus = I2CFactory.getInstance(I2CBus.BUS_1);
		// Get I2C device, SI7005 I2C address is 0x40(64)
		I2CDevice device = Bus.getDevice(0x40);

		// Send temperature measurement command
		device.write(0x03, (byte)0x11);
		Thread.sleep(500);

		// Read 3 bytes of data
		// cTemp msb, cTemp lsb
		// Checking the status, Poll RDY in status until it is low(=0)
		byte[] data = new byte[3];
		device.read(0x00, data, 0, 3);
		while((data[0] & 0x01) != 0)
		{
			device.read(0x00, data, 0, 3);
		}

		// Convert the data to 14-bits
		double cTemp = ((((data[1] & 0xFF) * 256 + (data[2]  & 0xFC)) / 4.0) / 32.0) - 50.0;
		double fTemp = cTemp * 1.8 + 32;

		// Send humidity measurement command
		device.write(0x03, (byte)0x01);
		Thread.sleep(500);

		// Read 2 bytes of data
		// humidity msb, humidity lsb
		// Checking the status, Poll RDY in status until it is low(=0)
		device.read(0x00, data, 0, 3);
		while((data[0] & 0x01) != 0)
		{
			device.read(0x00, data, 0, 3);
		}

		// Convert the data to 12-bits
		double humidity = ((((data[1] & 0xFF) * 256 + (data[2] & 0xF0)) / 16.0) / 16.0) - 24.0;
		humidity = humidity - (((humidity * humidity) * (-0.00393)) + (humidity * 0.4008) - 4.7844);
		humidity = humidity + (cTemp - 30) * (humidity * 0.00237 + 0.1973);

		// Output data to screen
		System.out.printf("Relative Humidity : %.2f %% %n", humidity);
		System.out.printf("Temperature in Celsius : %.2f C%n", cTemp);
		System.out.printf("Temperature in Fahrenheit : %.2f F%n", fTemp);
	}
}
