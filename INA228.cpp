//    FILE: INA228.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.2
//    DATE: 2024-05-09
// PURPOSE: Arduino library for INA228 voltage, current and power sensor.
//     URL: https://github.com/RobTillaart/INA228
//
//  Read the datasheet for the details


#include "INA228.h"

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


//  CONFIG MASKS (register 0)
#define INA228_CFG_RST              0x8000
#define INA228_CFG_RSTACC           0x4000
#define INA228_CFG_CONVDLY          0x3FC0
#define INA228_CFG_TEMPCOMP         0x0020
#define INA228_CFG_ADCRANGE         0x0010
#define INA228_CFG_RESERVED         0x0004


//  ADC MASKS (register 1)
#define INA228_ADC_MODE             0xF000
#define INA228_ADC_VBUSCT           0x0E00
#define INA228_ADC_VSHCT            0x01C0
#define INA228_ADC_VTCT             0x0038
#define INA228_ADC_AVG              0x0007


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
  return value * 7.8125e-3;
}

//  PAGE 26 + 8.1.2
float INA228::getEnergy()
{
  float value = _readRegisterF(INA228_ENERGY, 5);
  //  PAGE 31 (8.1.2)
  return value * 16 * 3.2 * _current_LSB;
}

float INA228::getCharge()
{
  float value = _readRegisterF(INA228_CHARGE, 5);
  //  PAGE 32 (8.1.2)
  return value * _current_LSB;
}


////////////////////////////////////////////////////////
//
//  CONFIG REGISTER 0
//
void INA228::reset()
{
  uint16_t value = _readRegister(INA228_CONFIG, 2);
  value |= INA228_CFG_RST;
  _writeRegister(INA228_CONFIG, value);
}

bool INA228::setAccumulation(uint8_t value)
{
  if (value > 1) return false;
  uint16_t reg = _readRegister(INA228_CONFIG, 2);
  reg &= ~INA228_CFG_RSTACC;
  if (value == 1) reg |= INA228_CFG_RSTACC;
  _writeRegister(INA228_CONFIG, reg);
  return true;
}

bool INA228::getAccumulation()
{
  uint16_t value = _readRegister(INA228_CONFIG, 2);
  return (value & INA228_CFG_RSTACC) > 0;
}

void INA228::setConversionDelay(uint8_t steps)
{
  uint16_t value = _readRegister(INA228_CONFIG, 2);
  value &= ~INA228_CFG_CONVDLY;
  value |= (steps << 6);
  _writeRegister(INA228_CONFIG, value);
}

uint8_t INA228::getConversionDelay()
{
  uint16_t value = _readRegister(INA228_CONFIG, 2);
  return (value >> 6) & 0xFF;
}

void INA228::setTemperatureCompensation(bool on)
{
  uint16_t value = _readRegister(INA228_CONFIG, 2);
  value &= ~INA228_CFG_TEMPCOMP;
  if (on) value |= INA228_CFG_TEMPCOMP;
  _writeRegister(INA228_CONFIG, value);
}

bool INA228::getTemperatureCompensation()
{
  uint16_t value = _readRegister(INA228_CONFIG, 2);
  return (value & INA228_CFG_TEMPCOMP) > 0;
}

void INA228::setADCRange(bool flag)
{
  uint16_t value = _readRegister(INA228_CONFIG, 2);
  value &= ~INA228_CFG_ADCRANGE;
  if (flag) value |= INA228_CFG_ADCRANGE;
  _writeRegister(INA228_CONFIG, value);
}

bool INA228::getADCRange()
{
  uint16_t value = _readRegister(INA228_CONFIG, 2);
  return (value & INA228_CFG_ADCRANGE) > 0;
}


////////////////////////////////////////////////////////
//
//  CONFIG ADC REGISTER 1
//
bool INA228::setMode(uint8_t mode)
{
  if (mode > 0x0F) return false;
  uint16_t value = _readRegister(INA228_ADC_CONFIG, 2);
  value &= ~INA228_ADC_MODE;
  value |= (mode << 12);
  _writeRegister(INA228_ADC_CONFIG, value);
  return true;
}

uint8_t INA228::getMode()
{
  uint16_t value = _readRegister(INA228_ADC_CONFIG, 2);
  return (value & INA228_ADC_MODE) >> 12;
}

bool INA228::setBusVoltageConversionTime(uint8_t bvct)
{
  if (bvct > 7) return false;
  uint16_t value = _readRegister(INA228_ADC_CONFIG, 2);
  value &= ~INA228_ADC_VBUSCT;
  value |= (bvct << 9);
  _writeRegister(INA228_ADC_CONFIG, value);
  return true;
}

uint8_t INA228::getBusVoltageConversionTime()
{
  uint16_t value = _readRegister(INA228_ADC_CONFIG, 2);
  return (value & INA228_ADC_VBUSCT) >> 9;
}

bool INA228::setShuntVoltageConversionTime(uint8_t svct)
{
  if (svct > 7) return false;
  uint16_t value = _readRegister(INA228_ADC_CONFIG, 2);
  value &= ~INA228_ADC_VSHCT;
  value |= (svct << 6);
  _writeRegister(INA228_ADC_CONFIG, value);
  return true;
}

uint8_t INA228::getShuntVoltageConversionTime()
{
  uint16_t value = _readRegister(INA228_ADC_CONFIG, 2);
  return (value & INA228_ADC_VSHCT) >> 6;
}

bool INA228::setTemperatureConversionTime(uint8_t tct)
{
  if (tct > 7) return false;
  uint16_t value = _readRegister(INA228_ADC_CONFIG, 2);
  value &= ~INA228_ADC_VTCT;
  value |= (tct << 3);
  _writeRegister(INA228_ADC_CONFIG, value);
  return true;
}

uint8_t INA228::getTemperatureConversionTime()
{
  uint16_t value = _readRegister(INA228_ADC_CONFIG, 2);
  return (value & INA228_ADC_VTCT) >> 3;
}

bool INA228::setAverage(uint8_t avg)
{
  if (avg > 7) return false;
  uint16_t value = _readRegister(INA228_ADC_CONFIG, 2);
  value &= ~INA228_ADC_AVG;
  value |= avg;
  _writeRegister(INA228_ADC_CONFIG, value);
  return true;
}

uint8_t INA228::getAverage()
{
  uint16_t value = _readRegister(INA228_ADC_CONFIG, 2);
  return (value & INA228_ADC_AVG);
}


////////////////////////////////////////////////////////
//
//  SHUNT CALIBRATION REGISTER 2
//
int INA228::setMaxCurrentShunt(float maxCurrent, float shunt)
{
  if (maxCurrent > 10) return -1;
  if (shunt < 0.005) return -2;
  _maxCurrent = maxCurrent;
  _shunt = shunt;
  _current_LSB = _maxCurrent * 1.9073486328125e-6;  //  pow(2, -19);
  return 0;
}

float INA228::getMaxCurrent()
{
  return _maxCurrent;
}

float INA228::getShunt()
{
  return _shunt;
}


////////////////////////////////////////////////////////
//
//  SHUNT TEMPERATURE COEFFICIENT REGISTER 3
//
bool INA228::setShuntTemperatureCoefficent(uint16_t ppm)
{
  if (ppm > 16383) return false;
  _writeRegister(INA228_SHUNT_TEMP_CO, ppm);
  return true;
}

uint16_t INA228::getShuntTemperatureCoefficent()
{
  uint16_t value = _readRegister(INA228_SHUNT_TEMP_CO, 2);
  return value;
}



////////////////////////////////////////////////////////
//
//  DIAGNOSE ALERT REGISTER 11
//
void INA228::setDiagnoseAlert(uint16_t flags)
{
  _writeRegister(INA228_DIAG_ALERT, flags);
}

uint16_t INA228::getDiagnoseAlert()
{
  return _readRegister(INA228_DIAG_ALERT, 2);
}

//  INA228.h has an enum for the bit fields.
void INA228::setDiagnoseAlertBit(uint8_t bit)
{
  uint16_t value = _readRegister(INA228_DIAG_ALERT, 2);
  value |= (1 << bit);
  _writeRegister(INA228_DIAG_ALERT, value);
}

void INA228::clrDiagnoseAlertBit(uint8_t bit)
{
  uint16_t value = _readRegister(INA228_DIAG_ALERT, 2);
  value &= ~(1 << bit);
  _writeRegister(INA228_DIAG_ALERT, value);
}

uint16_t INA228::getDiagnoseAlertBit(uint8_t bit)
{
  uint16_t value = _readRegister(INA228_DIAG_ALERT, 2);
  return (value >> bit) & 0x01;
}


////////////////////////////////////////////////////////
//
//  THRESHOLD AND LIMIT REGISTERS 12-17
//

void INA228::setShuntOvervoltageTH(uint16_t threshold)
{
  _writeRegister(INA228_SOVL, threshold);
}

uint16_t INA228::getShuntOvervoltageTH()
{
  return _readRegister(INA228_SOVL, 2);
}

void INA228::setShuntUndervoltageTH(uint16_t threshold)
{
  _writeRegister(INA228_SUVL, threshold);
}

uint16_t INA228::getShuntUndervoltageTH()
{
  return _readRegister(INA228_SUVL, 2);
}

void INA228::setBusOvervoltageTH(uint16_t threshold)
{
  if (threshold > 0x7FFF) return;
  _writeRegister(INA228_BOVL, threshold);
}

uint16_t INA228::getBusOvervoltageTH()
{
  return _readRegister(INA228_BOVL, 2);
}

void INA228::setBusUndervoltageTH(uint16_t threshold)
{
  if (threshold > 0x7FFF) return;
  _writeRegister(INA228_BUVL, threshold);
}

uint16_t INA228::getBusUndervoltageTH()
{
  return _readRegister(INA228_BUVL, 2);
}

void INA228::setTemperatureOverLimitTH(uint16_t threshold)
{
  _writeRegister(INA228_TEMP_LIMIT, threshold);
}

uint16_t INA228::getTemperatureOverLimitTH()
{
  return _readRegister(INA228_TEMP_LIMIT, 2);
}

void INA228::setPowerOverLimitTH(uint16_t threshold)
{
  _writeRegister(INA228_POWER_LIMIT, threshold);
}

uint16_t INA228::getPowerOverLimitTH()
{
  return _readRegister(INA228_POWER_LIMIT, 2);
}


////////////////////////////////////////////////////////
//
//  MANUFACTURER and ID REGISTER 3E/3F
//
bool INA228::getManufacturer()
{
  uint16_t value = _readRegister(INA228_MANUFACTURER, 2);
  return value;
}

uint16_t INA228::getDieID()
{
  uint16_t value = _readRegister(INA228_DEVICE_ID, 2);
  return (value >> 4) & 0xFFF0;
}

uint16_t INA228::getRevision()
{
  uint16_t value = _readRegister(INA228_DEVICE_ID, 2);
  return value & 0x0F;
}


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


float INA228::_readRegisterF(uint8_t reg, uint8_t bytes)
{
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->endTransmission();

  _wire->requestFrom(_address, (uint8_t)bytes);
  float value = 0;
  for ( int i = 0; i < bytes; i++)
  {
    value *= 256.0;
    value += _wire->read();
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

