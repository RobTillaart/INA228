//    FILE: INA228.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
//    DATE: 2024-05-09
// PURPOSE: Arduino library for INA228 power sensor
//     URL: https://github.com/RobTillaart/INA228
//
//  Read the datasheet for the details


#include "INA228.h"



////////////////////////////////////////////////////////
//
//  CONSTRUCTOR
//
INA228::INA228(const uint8_t address, TwoWire *wire)
{
  _address     = address;
  _wire        = wire;
  //  no calibrated values by default.
  _shunt       = 0.015;
  _maxCurrent  = 10.0;
  _current_LSB = _maxCurrent * pow(2, -19);
}


bool INA228::begin()
{
  if (! isConnected()) return false;
  return true;
}


bool INA228::isConnected()
{
  _wire->beginTransmission(_address);
  return ( _wire->endTransmission() == 0);
}


uint8_t INA228::getAddress()
{
  return _address;
}


////////////////////////////////////////////////////////
//
//  CORE FUNCTIONS
//
//  PAGE 25
float INA228::getBusVoltage()
{
  uint32_t value = _readRegister(INA228_BUS_VOLTAGE, 3);
  value >>= 4;
  return value * 195.3125e-6;
}

//  PAGE 25
float INA228::getShuntVoltage()
{
  uint32_t value = _readRegister(INA228_SHUNT_VOLTAGE, 3);
  value >>= 4;
  //  depends on ADCRANGE in INA228_CONFIG register.
  uint16_t config = _readRegister(INA228_CONFIG, 2);
  if (config & 0x0008) return value * 78.125e-9;
  return value * 312.5e-9;
}

//  PAGE 25 + 8.1.2
float INA228::getCurrent()
{
  uint32_t value = _readRegister(INA228_CURRENT, 3);
  
  //  PAGE 31 (8.1.2)
  float shunt_cal = 13107.2e6 * _current_LSB * _shunt;
  //  depends on ADCRANGE in INA228_CONFIG register.
  uint16_t config = _readRegister(INA228_CONFIG, 2);
  if (config & 0x0008) shunt_cal *= 4;
  return value * shunt_cal;
}


//  PAGE 26 + 8.1.2
float INA228::getPower()
{
  uint32_t value = _readRegister(INA228_POWER, 3);
  //  PAGE 31 (8.1.2)
  return value * 3.2 * _current_LSB;
}


//  PAGE25
float INA228::getTemperature()
{
  uint32_t value = _readRegister(INA228_TEMPERATURE, 2);
  return value * 7.8125e-6;
}


//  PAGE 26 + 8.1.2
//  NEED 40 bits == 5 bytes.
// float INA228::getEnergy()
// {
  // uint32_t value = _readRegister(INA228_ENERGY, 5);
  //  PAGE 31 (8.1.2)
  // return value * 16 x 3.2 x _current_LSB;
// }


// float INA228::getCharge()
// {
  // uint32_t value = _readRegister(INA228_CHARGE, 5);
  //  PAGE 32 (8.1.2)
  // return value * _current_LSB;
// }



////////////////////////////////////////////////////////
//
//  SHOULD BE PRIVATE
//
uint32_t INA228::_readRegister(uint8_t reg, uint8_t bytes)
{
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->endTransmission();

  _wire->requestFrom(_address, (uint8_t)bytes);
  uint32_t value = 0;
  for ( int i = 0; i < bytes; i++)
  {
    value <<= 8;
    value |= _wire->read();
  }
  return value;
}


uint16_t INA228::_writeRegister(uint8_t reg, uint16_t value)
{
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->write(value >> 8);
  _wire->write(value & 0xFF);
  return _wire->endTransmission();
}


//  -- END OF FILE --

