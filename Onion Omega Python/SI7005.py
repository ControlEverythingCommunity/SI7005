# Distributed with a free-will license.
# Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
# SI7005
# This code is designed to work with the SI7005_I2CS I2C Mini Module available from ControlEverything.com.
# https://www.controleverything.com/content/Temperature?sku=SI7005_I2CS#tabs-0-product_tabset-2

from OmegaExpansion import onionI2C
import time

# Get I2C bus
i2c = onionI2C.OnionI2C()

# SI7005 address, 0x40(64)
# Select Configuration register, 0x03(03)
#		0x11(11)	Temperature, Fast mode enable, Heater Off
i2c.writeByte(0x40, 0x03, 0x11)

time.sleep(0.5)

# SI7005 address, 0x40(64)
# Read data back from 0x00(00), 3 bytes
# Status register, ctemp MSB, ctemp LSB
# Checking the status, Poll RDY in status until it is low(=0)
data = i2c.readBytes(0x40, 0x00, 3)
while (data[0] & 0x01) != 0 :
	data = i2c.readBytes(0x40, 0x00, 3)

# Convert the data to 14-bits
ctemp = ((data[1] * 256 + data[2]) / 4.0) / 32.0 - 50.0
ftemp = ctemp * 1.8 + 32

# SI7005 address, 0x40(64)
# Select Configuration register, 0x03(03)
#		0x01(01)	Relative Humidity, Fast mode enable, Heater Off
i2c.writeByte(0x40, 0x03, 0x01)

time.sleep(0.5)

# SI7005 address, 0x40(64)
# Read data back from 0x00(00), 3 bytes
# Status register, humidity MSB, humidity LSB
# Checking the status, Poll RDY in status until it is low(=0)
data = i2c.readBytes(0x40, 0x00, 3)
while (data[0] & 0x01) != 0 :
	data = i2c.readBytes(0x40, 0x00, 3)

# Convert the data to 12-bits
humidity = ((data[1] * 256 + data[2]) / 16.0) / 16.0 - 24.0

# Output data to screen
print "Relative Humidity : %.2f %%" %humidity
print "Temperature in Celsius : %.2f C" %ctemp
print "Temperature in Fahrenheit : %.2f F" %ftemp
