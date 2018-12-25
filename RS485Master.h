/*
  RS485 communicator synchronization library

  Copyright (C) 2018 - Ivan Vaccari <vaccari87@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  
*/

#include "RS485Communicator.h"

#ifndef RS485_MASTER
#define RS485_MASTER

/* 
 * Maximum supported devices on the 485 bus for this library.
 * Depending on the RS485 transceiver, this value can be changed. Most of them support up to 128 devices,
 * but since we use a single byte for the address value, this library support up to 254 devices.
 * In the default setup, this number is limited to 32 devices in order to save up some RAM bytes.
 * This means addresses are valid from 0 to 31
 */
#define MAXIMUM_DEVICES_COUNT 32 

/* bytes to save the runtime availability of slaves. Must be ceil(MAXIMUM_DEVICES_COUNT/8*/
#define AVAILABILITY_BYTES 4

/*!
 * \brief Class for manage the Rs485 Master
 */
class  RS485Master: public RS485Communicator{
  private:
    byte slavesAvailability[AVAILABILITY_BYTES];
    byte currentSlaveAddress;
    unsigned long lastSentTokenTimeout; 
    unsigned long tokenReturnTimeout;
  public:
    RS485Master();

    
    /*! /brief setup the bus slave.
     *  /param _dePin the pin to be used to contro the driver enable signal of the RS485 module.
     *  /param _tokenReturnTimeout The token return timeout. See wiki for reference
     *  /param _baud The serial baud value. See Arduino Serial speed for reference
     */
    bool begin(byte _dePin, unsigned long _tokenReturnTimeout = 400, unsigned long _baud = 19200);

    /*! /brief Get the presence or absence of a specific address. Only present address will be polled by the library
     */
    void setSlaveAvailability(byte _addr,bool _present);

    /*! /brief Get the presence or absence of a specific address.
     */
    bool getSlaveAvailability(byte _addr);

     /*! /brief Loop function of the library. Must be called at every main() loop cycle
     */
    void loop();
    
};

#endif
