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

#include "RS485Master.h"

RS485Master::RS485Master():
  RS485Communicator(),
  currentSlaveAddress(0),
  lastSentTokenTimeout(0),
  tokenReturnTimeout(400){
  memset(slavesAvailability,0,AVAILABILITY_BYTES);
}


bool RS485Master::begin(byte _dePin, unsigned long _tokenReturnTimeout, unsigned long _baud){
  
  if (_tokenReturnTimeout <5 ){
    return false;
  }
  tokenReturnTimeout = _tokenReturnTimeout;
  return RS485Communicator::begin(_dePin,0,_baud);
}


void  RS485Master::setSlaveAvailability(byte _addr,bool _present){
  if ((_addr > 0) && (_addr < MAXIMUM_DEVICES_COUNT)){
    byte i=floor(_addr/8);
    bitWrite(slavesAvailability[i], _addr%8, _present);
  }
}

bool RS485Master::getSlaveAvailability(byte _addr){
  if ((_addr > 0) && (_addr < MAXIMUM_DEVICES_COUNT)){
    byte i=floor(_addr/8);
    return bitRead(slavesAvailability[i], _addr%8);
  }
  return false;
}

void RS485Master::loop(){
  
  byte nextAddressToPoll=0;
  if (writePermission){
    /* Master must manage the write Token.
     * He have to calculate the next slave allowed to write and send the token to it.
     */
    
    for(byte n=1;n<=MAXIMUM_DEVICES_COUNT;n++){
      byte addr=(currentSlaveAddress+n)%MAXIMUM_DEVICES_COUNT;
      if (bitRead(slavesAvailability[(byte)floor(addr/8)], addr%8)){
        currentSlaveAddress = addr;
        nextAddressToPoll = addr;
        break;
      }
    }

    /*
    generate the token for the next client. If no salves are enabled no token is generated. 
    label: [LABEL_TOK_REG]
    */
    lastSentTokenTimeout = 0;
    
  }
  
  if(RS485Communicator::loop(nextAddressToPoll, nextAddressToPoll>0)){
    lastSentTokenTimeout = millis();
  }

  if ((!writePermission) && (lastSentTokenTimeout>0)){
    /* Master also have to manage the token return timeouts.
     * Timeouts occur when the slave crash and do not send the token back or when the token
     * get lost due transmission error. 
     * In that case, the master take the writePermission who implicitly trigger 
     * the token regeneration (done in the next loop cycle, code at label [LABEL_TOK_REG])
     */
    unsigned long currentMillis=millis();
    if (currentMillis - lastSentTokenTimeout > tokenReturnTimeout) {
      lastSentTokenTimeout = 0;
      writePermission = true;
    }
  }
}
