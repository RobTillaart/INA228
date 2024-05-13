#pragma once
//    FILE: INA228.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
//    DATE: 2024-
// PURPOSE: Arduino library for INA228 power sensor
//     URL: https://github.com/RobTillaart/INA228
//
//  Read the datasheet for the details


#include "Arduino.h"
#include "Wire.h"


#define INA228_LIB_VERSION          "0.1.0"


//  TODO MOVE TO CPP  CHECK DATASHEET.

//      REGISTERS                   ADDRESS    BITS  RW
#define INA228_CONFIG               0x00    //  16   RW
#define INA228_ADC_CONFIG           0x01    //  16   RW
#define INA228_SHUNT_CAL            0x02    //  16   RW
#define INA228_SHUNT_TEMP_CO        0x03    //  16   RW
#define INA228_SHUNT_VOLTAGE        0x04    //  24   R-
#define INA228_BUS_VOLTAGE          0x05    //  24   R-
#define INA228_TEMPERATURE          0x06    //  16   R-
#define INA228_CURRENT              0x07    //  24   R-
#define INA228_POWER                0x08    //  24   R-
#define INA228_ENERGY               0x09    //  40   R-
#define INA228_CHARGE               0x0A    //  40   R-
#define INA228_DIAG_ALERT           0x0B    //  16   RW
#define INA228_SOVL                 0x0C    //  16   RW
#define INA228_SUVL                 0x0D    //  16   RW
#define INA228_BOVL                 0x0E    //  16   RW
#define INA228_BUVL                 0x0F    //  16   RW
#define INA228_TEMP_LIMIT           0x10    //  16   RW
#define INA228_POWER_LIMIT          0x11    //  16   RW
#define INA228_MANUFACTURER         0x3E    //  16   R-
#define INA228_DEVICE_ID            0x3F    //  16   R-


class INA228
{
public:
  //  address between 0x40 and 0x4F
  explicit INA228(const uint8_t address, TwoWire *wire = &Wire);

  bool     begin();
  bool     isConnected();
  uint8_t  getAddress();

  //
  //  CORE FUNCTIONS
  //
  float    getBusVoltage();       //  Volt
  float    getShuntVoltage();     //  Volt
  float    getCurrent();          //  Ampere
  float    getPower();            //  Watt
  float    getTemperature();      //  Celsius
  float    getEnergy();           //  Joule
  float    getCharge();           //  Coulombs

  //  Scale helpers milli range
  float    getBusVoltage_mV()   { return getBusVoltage()   * 1e3; };
  float    getShuntVoltage_mV() { return getShuntVoltage() * 1e3; };
  float    getCurrent_mA()      { return getCurrent()      * 1e3; };
  float    getPower_mW()        { return getPower()        * 1e3; };
  float    getEnergy_mJ()       { return getEnergy()       * 1e3; };
  float    getCharge_mC()       { return getCharge()       * 1e3; };

  //  Scale helpers micro range
  float    getBusVoltage_uV()   { return getBusVoltage()   * 1e6; };
  float    getShuntVoltage_uV() { return getShuntVoltage() * 1e6; };
  float    getCurrent_uA()      { return getCurrent()      * 1e6; };
  float    getPower_uW()        { return getPower()        * 1e6; };
  float    getEnergy_uJ()       { return getEnergy()       * 1e6; };
  float    getCharge_uC()       { return getCharge()       * 1e6; };

  //
  //  CONFIG REGISTER
  //  read datasheet for details.
  void     reset();
  //  val: 0 == normal operation,  1 = clear registers
  bool     resetAccumulation(uint8_t val);
  //  Conversion delay in 0..255 steps of 2 ms
  void     setConversionDelay(uint8_t steps);
  uint8_t  getConversionDelay();
  void     setTemperatureCompensation(bool on);
  bool     getTemperatureCompensation();
  //  flag = false => 164 mV, true => 41 mV 
  void     setADCRange(bool flag);
  bool     getADCRange();

  //
  //  CONFIG REGISTER
  //  read datasheet for details.




  ///////////////////
  //
  //  SHOULD BE PRIVATE
  //
  uint32_t _readRegister(uint8_t reg, uint8_t bytes);  //  max 4.
  float    _readRegisterF(uint8_t reg, uint8_t bytes);
  uint16_t _writeRegister(uint8_t reg, uint16_t value);


private:


  float    _current_LSB;
  float    _shunt;
  float    _maxCurrent;

  uint8_t   _address;
  TwoWire * _wire;
};


//  -- END OF FILE --


