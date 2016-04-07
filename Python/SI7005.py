# Distributed with a free-will license.
# Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
# SI7005
# This code is designed to work with the SI7005_I2CS I2C Mini Module available from ControlEverything.com.
# https://www.controleverything.com/content/Temperature?sku=SI7005_I2CS#tabs-0-product_tabset-2

import smbus
import time

# Get I2C bus
bus = smbus.SMBus(1)

# SI7005 address, 0x40(64)
# Select Configuration register, 0x03(03)
#		0x11(11)	Temperature, Fast mode enable, Heater Off
bus.write_byte_data(0x40, 0x03, 0x11)

time.sleep(0.5)

# SI7005 address, 0x40(64)
# Read data back from 0x00(00), 3 bytes
# Status register, ctemp MSB, ctemp LSB
# Checking the status, Poll RDY in status until it is low(=0)
data = bus.read_i2c_block_data(0x40, 0x00, 3)
while (data[0] & 0x01) != 0 :
	data = bus.read_i2c_block_data(0x40, 0x00, 3)

# Convert the data to 14-bits
ctemp = ((data[1] * 256 + data[2]) / 4.0) / 32.0 - 50.0
ftemp = ctemp * 1.8 + 32

# SI7005 address, 0x40(64)
# Select Configuration register, 0x03(03)
#		0x01(01)	Relative Humidity, Fast mode enable, Heater Off
bus.write_byte_data(0x40, 0x03, 0x01)

time.sleep(0.5)

# SI7005 address, 0x40(64)
# Read data back from 0x00(00), 3 bytes
# Status register, humidity MSB, humidity LSB
# Checking the status, Poll RDY in status until it is low(=0)
data = bus.read_i2c_block_data(0x40, 0x00, 3)
while (data[0] & 0x01) != 0 :
	data = bus.read_i2c_block_data(0x40, 0x00, 3)

# Convert the data to 12-bits
humidity = ((data[1] * 256 + data[2]) / 16.0) / 16.0 - 24.0
humidity = humidity - (((humidity * humidity) * (-0.00393)) + (humidity * 0.4008) - 4.7844)
humidity = humidity + (ctemp - 30) * (humidity * 0.00237 + 0.1973)

# Output data to screen
print "Relative Humidity : %.2f %%" %humidity
print "Temperature in Celsius : %.2f C" %ctemp
print "Temperature in Fahrenheit : %.2f F" %ftemp
