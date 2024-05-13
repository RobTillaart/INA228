
[![Arduino CI](https://github.com/RobTillaart/INA228/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/RobTillaart/INA228/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/RobTillaart/INA228/actions/workflows/arduino-lint.yml)
[![JSON check](https://github.com/RobTillaart/INA228/actions/workflows/jsoncheck.yml/badge.svg)](https://github.com/RobTillaart/INA228/actions/workflows/jsoncheck.yml)
[![GitHub issues](https://img.shields.io/github/issues/RobTillaart/INA228.svg)](https://github.com/RobTillaart/INA228/issues)

[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/INA228/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/INA228.svg?maxAge=3600)](https://github.com/RobTillaart/INA228/releases)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/robtillaart/library/INA228.svg)](https://registry.platformio.org/libraries/robtillaart/INA228)


# INA228

Arduino library for the INA228 power sensor.


## Description

**Experimental** library for the INA228 power sensor.
Not all functionality is tested / investigated.

Read datasheet for details.

This readme is preliminary (copy from INA226).


==> **USE WITH CARE**

The INA228 is a voltage, current and power measurement device. 
A few important maxima, see datasheet.

|  description  |  max  |  unit  | notes |
|:--------------|------:|-------:|:------|
| bus voltage   |  85   | Volt   |  unclear for how long.
| shunt voltage |  ??   | mVolt  |
| current       |  10   | Ampere |  DC only




#### Special characters

- Ω == Ohm = ALT-234 (Windows)
- µ == micro = ALT-0181 (Windows)


#### Related

- https://www.ti.com/product/INA228#tech-docs
- https://www.ti.com/product/INA228#params
- https://www.ti.com/document-viewer/INA228/datasheet
- https://github.com/RobTillaart/INA219
- https://github.com/RobTillaart/INA226
- https://github.com/RobTillaart/INA228
- https://github.com/RobTillaart/INA3221_RT


## I2C

#### Address

The sensor can have 16 different I2C addresses, 
which depends on how the A0 and A1 address lines 
are connected to the SCL, SDA, GND and VCC pins.

See table - from datasheet table 2, page18.

|  A1   |  A0   |  ADDRESS   |
|:-----:|:-----:|:----------:|
|  GND  |  GND  |  1000000   |
|  GND  |  VS   |  1000001   |
|  GND  |  SDA  |  1000010   |
|  GND  |  SCL  |  1000011   |
|  VS   |  GND  |  1000100   |
|  VS   |  VS   |  1000101   |
|  VS   |  SDA  |  1000110   |
|  VS   |  SCL  |  1000111   |
|  SDA  |  GND  |  1001000   |
|  SDA  |  VS   |  1001001   |
|  SDA  |  SDA  |  1001010   |
|  SDA  |  SCL  |  1001011   |
|  SCL  |  GND  |  1001100   |
|  SCL  |  VS   |  1001101   |
|  SCL  |  SDA  |  1001110   |
|  SCL  |  SCL  |  1001111   |


#### Performance

To be elaborated, 


## Interface

```cpp
#include "INA228.h"
```


#### Constructor

- **INA228(const uint8_t address, TwoWire \*wire = Wire)** Constructor to set 
the address and optional Wire interface.
- **bool begin()** initializes the class.
returns true if the INA228 address is on the I2C bus.
Note: one needs to set **Wire.begin()** before calling **begin()**.
- **bool isConnected()** returns true if the INA228 address is on the I2C bus.
- **uint8_t getAddress()** returns the address set in the constructor.


#### Core Functions

Note the power and the current are not meaningful without calibrating the sensor.
Also the value is not meaningful if there is no shunt connected.

- **float getShuntVoltage()** idem, in volts.
- **float getBusVoltage()** idem. in volts. Max 36 Volt.
- **float getCurrent()** is the current through the shunt in Ampere.
- **float getPower()** is the current x BusVoltage in Watt.
- **float getEnergy()** return Joule (elaborate).
- **float getCharge()** return Coulomb (elaborate).

The library has helper functions to convert above output to a more appropriate scale of units.

Helper functions for the milli scale.

- **float getBusVoltage_mV()** idem, in milliVolts.
- **float getShuntVoltage_mV()** idem, in milliVolts.
- **float getCurrent_mA()** idem, in milliAmpere.
- **float getPower_mW()** idem, in milliWatt.
- **float getEnergy_mJ()** idem, milliJoule.
- **float getCharge_mC()** idem, milliCoulomb.

Helper functions for the micro scale.

- **float getBusVoltage_uV()** idem, in microVolts.
- **float getShuntVoltage_uV()** idem, in microVolts.
- **float getCurrent_uA()** idem, in microAmpere.
- **float getPower_uW()** idem, in microWatt.
- **float getEnergy_uJ()** idem, microJoule.
- **float getCharge_uC()** idem, microCoulomb.


TO ELABORATE


## Future


#### Must

- update documentation.
- update functionality.
- get hardware to test.
- test a lot.
- write examples.
- keep in sync with INA226 where possible.

#### Should


#### Could

- improve unit tests
- clean up magic numbers in the code

#### Won't

- cache registers for performance (first get it working).
- 

## Support

If you appreciate my libraries, you can support the development and maintenance.
Improve the quality of the libraries by providing issues and Pull Requests, or
donate through PayPal or GitHub sponsors.

Thank you,

