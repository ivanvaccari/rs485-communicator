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

#ifndef RS485_SLAVE
#define RS485_SLAVE

/*/brief the RS485Slave is a class for managing a bus slave
*/
class RS485Slave: public RS485Communicator{

  public:
    RS485Slave();

    /*! /brief setup the bus slave.
     *  /param _dePin the pin to be used to contro the driver enable signal of the RS485 module.
     *  /param _myAddress The RS485 address to be assigned to this instance
     *  /param _baud The serial baud value. See Arduino Serial speed for reference
     */
    bool begin(byte _dePin, byte _myAddress, unsigned long _baud = 19200);

    /*! /brief Loop function of the library. Must be called at every main() loop cycle
     */
    void loop();
    
};

#endif
