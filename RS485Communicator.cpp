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
#define RS485_DRIVER_ENABLE HIGH
#define RS485_DRIVER_DISABLE LOW

RS485Communicator::RS485Communicator():
  writePermission(false),
  recvBufferTailIndex(0),
  recvBufferHeadIndex(0),
  recvBufferCount(0),
  sendBufferTailIndex(0),
  sendBufferHeadIndex(0),
  sendBufferCount(0),
  recvMessageCount(0),
  sentMessageCount(0),
  lenghtErrorCount(0),
  crcErrorCount(0),
  tokenCount(0)
{

}
bool RS485Communicator::begin(byte _dePin, byte _myAddr, unsigned long baud) {
  de485ControlPin = _dePin;
  localAddress = _myAddr;

  pinMode(de485ControlPin, OUTPUT);
  digitalWrite(de485ControlPin, RS485_DRIVER_DISABLE);
  writePermission = localAddress == 0;

  RS485_SERIAL_NAME.begin(baud);
  while (!RS485_SERIAL_NAME) {}

  return true;
}


bool RS485Communicator::loop(byte tokenAddress, bool writeToken) {
  if (writePermission) {

    // enable DRIVER ENABLE signal of the transcaiver, making the current transceiver in write mode
    digitalWrite(de485ControlPin, RS485_DRIVER_ENABLE);
	
	// wait some time to meke the driver enable signal effective
	// FIXME: how to make this time not hardcoded?
    delayMicroseconds(500);

	//sending messages
    while (sendBufferCount > 0) {
      sendOneBufferedMessage();
    }

    if (writeToken) {
	  //sending token
      sendToken(tokenAddress);
      writePermission = false;
    }

	// disable DRIVER ENABLE signal of the transcaiver, making the current transceiver in read mode
    digitalWrite(de485ControlPin, RS485_DRIVER_DISABLE);

    if (writeToken) {
      return true;
    }

  } else {
    readOneMessage();
  }
  return false;
}
void RS485Communicator::readOneMessage() {
  while (RS485_SERIAL_NAME.available()) {
    byte b = RS485_SERIAL_NAME.read();

    if (b == 255) {
      /* Received byte is the delimiter. Message receive is complete. Now check for validity. */

      if (currentReceivingMessage.bufferWrittenBytes < (3 + RS485_PAYLOAD_LENGTH + RS485_PAYLOAD_FIX_BYTES + 1)) {
        /* Received message is shorter than expected. An error occurred during transmission .
          Since we have no clue what bytes are lost, and we cannot recover it, the message is just discarded.
        */
        currentReceivingMessage.clean();
        lenghtErrorCount++;

      } else if (calculateMessageCrc(&currentReceivingMessage) != currentReceivingMessage.getReceivedCrcByte()) {
        /* The message checksum is invalid. An error occurred during transmission .
          Since we have no clue what bytes are lost, and we cannot recover it, the message is just discarded.
        */
        currentReceivingMessage.clean();
        crcErrorCount++;

      } else if (currentReceivingMessage.getDestinationAddress() != localAddress) {
        /* I'm not the destination of the message. Discard it*/
        currentReceivingMessage.clean();

      } else if (currentReceivingMessage.getMessageType() == RS485Message::MessageToken) {
        /* message is a token for me. Applying validity checks.*/
        if (localAddress == 0) {
          /* i'm the master and this is a token return.*/
          tokenCount++;
          writePermission = true;
          
        } else if (currentReceivingMessage.getSourceAddress() == 0 ) {
          /* i'm a slave. Slaves are allowed to receive tokens only from master.
            Tokens from other slaves are protocol violations.*/
          writePermission = true;
          tokenCount++;
        }

        /* no need to maintain the message*/
        currentReceivingMessage.clean();

      } else {
        /* Message is not a token, is valid AND i am the destination. trying to store it in the free space.*/
        if (recvBufferCount < RS485_RECV_BUFFER_SIZE) {
          recvBufferCount++;
          recvCircularBuffer[recvBufferHeadIndex] = currentReceivingMessage;
          /* pointing head to the next array cell */
          recvBufferHeadIndex = (recvBufferHeadIndex + 1) % RS485_RECV_BUFFER_SIZE;
          recvMessageCount++;
        }
        currentReceivingMessage.clean();
      }


    } else if (currentReceivingMessage.bufferWrittenBytes < (3 + RS485_PAYLOAD_LENGTH + RS485_PAYLOAD_FIX_BYTES + 1)) {

      /*appending received bytes in the buffer*/
      currentReceivingMessage.buffer[currentReceivingMessage.bufferWrittenBytes] = b;
      currentReceivingMessage.bufferWrittenBytes++;
    } else {
      /* too many bytes. An error somewhere occurred. Discard the mesage*/
      currentReceivingMessage.clean();
    }
  }
}

RS485Message* RS485Communicator::popMessage() {
  RS485Message *m = NULL;
  if (recvBufferCount > 0) {
    m = new RS485Message();
    *m = recvCircularBuffer[recvBufferTailIndex];
    recvCircularBuffer[recvBufferTailIndex].clean();
    recvBufferTailIndex = (recvBufferTailIndex + 1) % RS485_RECV_BUFFER_SIZE;
    recvBufferCount--;
  }
  return m;
}

void RS485Communicator::writeMessageToBus(RS485Message * message) {
  
  // calculating crc
  byte crc = calculateMessageCrc(message);

  /* Writing the actual message byte by byte.
     We need to wait after each byte to be sure the byte has been effectively written to the serial port.
     This is made by checking internal arduino registers.
  */  
  for (byte i = 0; i < 3+ RS485_PAYLOAD_LENGTH + RS485_PAYLOAD_FIX_BYTES; ++i) {
    RS485_SERIAL_NAME.write(message->buffer[i]);
    while (!(UCSR0A & (1 << TXC0)));
  }
    
  RS485_SERIAL_NAME.write(crc);
  while (!(UCSR0A & (1 << TXC0)));

  RS485_SERIAL_NAME.write(255); //message delimiter
  while (!(UCSR0A & (1 << TXC0)));
  
  // wait some time time at the end of the transmission
  // FIXME: check if this timer is still needed after the 'dramatic day discover'
  delayMicroseconds(300);

  message->clean();
}

bool RS485Communicator::sendOneBufferedMessage() {
  if (sendBufferCount > 0 ) {
    RS485Message* message = sendCircularBuffer + sendBufferTailIndex;
    writeMessageToBus(message);
    sendBufferCount--;
    if (sendBufferCount > 0) {
      sendBufferTailIndex = (sendBufferTailIndex + 1) % RS485_SEND_BUFFER_SIZE;
    }
    return true;
  }
  return false;
}

bool RS485Communicator::sendToken(byte _address) {

  tokenMessage.setMessageType(RS485Message::MessageToken);
  tokenMessage.setDestinationAddress(_address);
  tokenMessage.setSourceAddress(localAddress);
  writeMessageToBus(&tokenMessage);

  return true;
}

byte RS485Communicator::inboxCount() {
  return recvBufferCount;
}

byte RS485Communicator::available() {
  return RS485_SERIAL_NAME.available();
}

boolean RS485Communicator::ready() {
  return RS485_SERIAL_NAME;
}

byte RS485Communicator::getLocalAddress(){
  return localAddress;
}

RS485Communicator::RS485QueueResult RS485Communicator::queueMessage(RS485Message *_message) {

    
  if (sendBufferCount == RS485_SEND_BUFFER_SIZE) {
    return RS485QueueResult::BufferFull;
  }
  if (sendBufferCount > 0) {
    sendBufferHeadIndex = (sendBufferHeadIndex + 1) % RS485_SEND_BUFFER_SIZE;
  }

  sentMessageCount++;
  _message->setSourceAddress(localAddress);
  sendCircularBuffer[sendBufferHeadIndex] = *_message;
  sendBufferCount++;
  
  return RS485QueueResult::Success;
}

byte RS485Communicator::calculateMessageCrc(RS485Message * _message) {

  /* crc is calculated on bytes 1,2,3,...,2+RS485_PAYLOAD_LENGTH+RS485_PAYLOAD_FIX_BYTES of the message.
     The first one and the last one bytes are omitted (first is delimiter, last is where to place the CRC).

     credits to: http://www.leonardomiliani.com/2013/un-semplice-crc8-per-arduino/
  */
  byte crc = 0x00;
  byte index = 3 + RS485_PAYLOAD_LENGTH + RS485_PAYLOAD_FIX_BYTES;
  while (index > 1) {
    byte extract = _message->buffer[index - 1];
    for (byte tempI = 8; tempI; tempI--) {
      byte sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum)
        crc ^= 0x8C;
      extract >>= 1;
    }
    index--;
  }

  /* This is the crc fix. Since byte 255 is reserved as message delimiter, it cannot be used on the other message bytes
     This is an act of faith. We cannot then detect an error if the received-message crc will be 254 but the orignal one was faked from 255 to 254.
     However, that's better than nothing.
  */
  if (crc == 255)
    crc = 254;

  return crc;
}


unsigned long RS485Communicator::getSentMessageCount(){
  return sentMessageCount;
}

unsigned long RS485Communicator::getRecvMessageCount(){
  return recvMessageCount;
}

unsigned long RS485Communicator::getLenghtErrorCount(){
  return lenghtErrorCount;
}

unsigned long RS485Communicator::getCrcErrorCount(){
  return crcErrorCount;
}

unsigned long RS485Communicator::getTokenCount(){
  return tokenCount;
}
