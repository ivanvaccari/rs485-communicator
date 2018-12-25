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

#include "RS485Message.h"

RS485Message::RS485Message(){
  clean();
}
bool RS485Message::setDestinationAddress(byte address){
  if (address==255)
    return false;
    
  buffer[0]=address;
  return true;
}

byte RS485Message::getDestinationAddress(){
  return buffer[0];
}
bool RS485Message::setSourceAddress(byte address){
  if (address==255)
    return false;
    
  buffer[1]=address;
  return true;
}
byte RS485Message::getSourceAddress(){
  return buffer[1];
}



bool RS485Message::setBytePayload(byte b, byte index){
  
  if (index>=RS485_PAYLOAD_LENGTH){
    return false;
  }
  byte tmp=b;
  bitWrite(tmp,0,0);
  buffer[3+index]=tmp;
  
  byte fixByteIndex = floor(index/7);
  byte fixBitIndex= index%7;
  tmp=buffer[3+RS485_PAYLOAD_LENGTH+fixByteIndex];
  
  bitWrite(tmp,fixBitIndex,bitRead(b,0));
  buffer[3+RS485_PAYLOAD_LENGTH+fixByteIndex]=tmp;
  
  return true;
  
}
bool RS485Message::setByteArrayPayload(byte bytes[], byte arraySize){
  if (arraySize>RS485_PAYLOAD_LENGTH){
    return false;
  }
  
  for( byte b=0; b<arraySize; b++){
    setBytePayload(bytes[b],b);
  }
  
  for( byte b=0; b<RS485_PAYLOAD_LENGTH-arraySize; b++){
    setBytePayload(0,arraySize+b);
  }
  
  return true;
}

bool RS485Message::setStringPayload(const String &string){
  if (string.length()>RS485_PAYLOAD_LENGTH){
    return false;
  } 

  memset(buffer+3,0,RS485_PAYLOAD_LENGTH+RS485_PAYLOAD_FIX_BYTES);
  
  for (byte b=0;b<string.length();b++){
    setBytePayload(string[b],b);
  }

  return true;
}

bool RS485Message::getBytePayload(byte index, byte* value){
  if (index>=RS485_PAYLOAD_LENGTH){
    return false;
  }
  
  *value=buffer[3+index];
  byte fixByteIndex = floor(index/7);
  byte fixBitIndex= index%7;
  bitWrite(*value,0,bitRead(buffer[3+RS485_PAYLOAD_LENGTH+fixByteIndex],fixBitIndex));
  return true;
}

bool RS485Message::getStringPayload(String* value){
  
  for (byte b=0;b<5+RS485_PAYLOAD_LENGTH;b++){
    byte v;
    if(getBytePayload(b,&v)){
      if ( v != 0){
        value->concat((char)v);
      }
    }
  }
  return true;
}
bool RS485Message::getArrayPayload(byte* value,byte *length){
  return true;
}

void RS485Message::setMessageType(MessageType t){
  byte tmp=0;
  switch(t){
    case MessageStandard: {
      buffer[2] = tmp;
      break;
      }
    case MessageToken: {
      bitWrite(tmp,0,1);
      buffer[2] = tmp;
      break;
      }
    default:{
      bitWrite(tmp,0,1);
      bitWrite(tmp,1,1);
      bitWrite(tmp,2,1);
      buffer[2] = tmp;
      break;
    }
  }
}

RS485Message::MessageType RS485Message::getMessageType(){
  if ((buffer[2] & 0x05) == 0){
    return MessageStandard;
  }
  
  if ((buffer[2] & 0x05) == 0x01){
    return MessageToken;
  }

  return UnknownType;
}

void RS485Message::clean(){
  bufferWrittenBytes=0;
  memset(buffer,0,3+RS485_PAYLOAD_LENGTH+RS485_PAYLOAD_FIX_BYTES+1);  
}

byte RS485Message::getReceivedCrcByte(){
  return buffer[3+RS485_PAYLOAD_LENGTH+RS485_PAYLOAD_FIX_BYTES];
}

RS485Message& RS485Message::operator=( const RS485Message& other){
    if (this != &other) {
      memcpy(buffer,other.buffer,3+RS485_PAYLOAD_LENGTH+RS485_PAYLOAD_FIX_BYTES+1);
    }
    return *this;
}
