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

#include "RS485Slave.h"

RS485Slave::RS485Slave():
  RS485Communicator(){
 
}


bool RS485Slave::begin(byte _dePin,byte _myAddress, unsigned long baud ){

  // slave can't use the address 0
  if (_myAddress == 0){
    return false;
  }

  // begin from base class
  return RS485Communicator::begin(_dePin,_myAddress, baud);
}


void RS485Slave::loop(){
  //loop from main class
  RS485Communicator::loop(0,true);
}
