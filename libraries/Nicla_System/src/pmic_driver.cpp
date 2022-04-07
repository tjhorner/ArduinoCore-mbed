#include <mbed.h>
#include <I2C.h>
#include "BQ25120A.h"
#include "Nicla_System.h"

uint8_t BQ25120A::getStatus()
{
  uint8_t c = readByte(BQ25120A_ADDRESS, BQ25120A_STATUS); // Read PRODUCT_ID register for BQ25120A
  return c;
}

void BQ25120A::writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
  nicla::i2c_mutex.lock();
  Wire1.beginTransmission(address);
  Wire1.write(subAddress);
  Wire1.write(data);
  Wire1.endTransmission();
  nicla::i2c_mutex.unlock();
}

uint8_t BQ25120A::readByte(uint8_t address, uint8_t subAddress)
{
  nicla::i2c_mutex.lock();
  char response = 0xFF;
  Wire1.beginTransmission(address);
  Wire1.write(subAddress);
  Wire1.endTransmission(false);
  Wire1.requestFrom(address, 1);
  while(!Wire1.available()) {}
  uint8_t ret = Wire1.read();
  nicla::i2c_mutex.unlock();
  return ret;
}