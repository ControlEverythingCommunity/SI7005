// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// SI7005
// This code is designed to work with the SI7005_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Temperature?sku=SI7005_I2CS#tabs-0-product_tabset-2

#include <application.h>
#include <spark_wiring_i2c.h>

// SI7005 I2C address is 0x40(64)
#define Addr 0x40

float cTemp = 0.0, fTemp = 0.0, humidity = 0.0;
int temp = 0;

void setup()
{
  // Set variable
  Particle.variable("i2cdevice", "SI7005");
  Particle.variable("Humidity", humidity);
  Particle.variable("cTemp", cTemp);

  // Initialise I2C communication as Master
  Wire.begin();
  // Initialise serial communication, set baud rate = 9600
  Serial.begin(9600);
  delay(300);

}

void loop()
{
  unsigned int data[3];

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Select configuration register
  Wire.write(0x03);
  // Command for temperature measurement
  Wire.write(0x11);
  // Stop I2C transmission
  Wire.endTransmission();
  delay(500);


  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x00);
  // Stop I2C transmission
  Wire.endTransmission();

  // Request 3 byte of data
  Wire.requestFrom(Addr, 3);

  // Read 3 byte of data
  // status, temp_msb, temp_lsb
  if (Wire.available() == 3)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
  }

  // Keep checking for valid data
  while ((data[0] & 0x01) != 0)
  {
    // Start I2C transmission
    Wire.beginTransmission(Addr);
    // Select data register
    Wire.write(0x00);
    // Stop I2C transmission
    Wire.endTransmission();

    // Request 3 byte of data
    Wire.requestFrom(Addr, 3);

    // Read 3 byte of data
    // status, humidity_msb, humidity_lsb
    if (Wire.available() == 3)
    {
      data[0] = Wire.read();
      data[1] = Wire.read();
      data[2] = Wire.read();
    }
  }

  // Convert the data to 14-bits
  temp = ((data[1] * 256) + (data[2] & 0xFC)) / 4;
  cTemp = (temp / 32.0) - 50.0;
  fTemp = cTemp * 1.8 + 32;

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Select configuration register
  Wire.write(0x03);
  // Command for humidity measurement
  Wire.write(0x01);
  // Stop I2C transmission
  Wire.endTransmission();
  delay(300);

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x00);
  // Stop I2C transmission
  Wire.endTransmission();

  // Request 3 byte of data
  Wire.requestFrom(Addr, 3);

  // Read 3 byte of data
  // status, humidity_msb, humidity_lsb
  if (Wire.available() == 3)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
  }

  // Keep checking for valid data
  while ((data[0] & 0x01) != 0)
  {
    // Start I2C transmission
    Wire.beginTransmission(Addr);
    // Select data register
    Wire.write(0x00);
    // Stop I2C transmission
    Wire.endTransmission();
    delay(300);

    // Request 3 byte of data
    Wire.requestFrom(Addr, 3);

    // Read 3 byte of data
    // status, humidity_msb, humidity_lsb
    if (Wire.available() == 3)
    {
      data[0] = Wire.read();
      data[1] = Wire.read();
      data[2] = Wire.read();
    }
  }

  // Convert the data to 12-bits
  humidity = ((data[1] * 256) + (data[2] & 0xF0)) / 16;
  humidity = (humidity / 16) - 24;
  humidity = humidity - (((humidity * humidity) * (-0.00393)) + (humidity * 0.4008) - 4.7844);
  humidity = humidity + (cTemp - 30) * (humidity * 0.00237 + 0.1973);

  // Output data to dashboard
  Particle.publish("Relative humidity : ", String(humidity));
  Particle.publish("Temperature in Celsius : ", String(cTemp));
  Particle.publish("Temperature in Farhenheit : ", String(fTemp));
  delay(500);
}

