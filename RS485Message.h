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

#include <arduino.h>

#ifndef RS485_MESSAGE
#define RS485_MESSAGE

#ifndef RS485_PAYLOAD_LENGTH
#define RS485_PAYLOAD_LENGTH 14
#endif

#ifndef RS485_PAYLOAD_FIX_BYTES
#define RS485_PAYLOAD_FIX_BYTES 2 // must be ceil(RS485_PAYLOAD_LENGTH/7)
#endif


class RS485Message{
  friend class RS485Communicator;
  private:
    byte bufferWrittenBytes;
    
    /* 1 dst, 2 src, 3 type, last crc */
    byte buffer[3+RS485_PAYLOAD_LENGTH+RS485_PAYLOAD_FIX_BYTES+1];
    
    /*set the destination address of this message. Values from 0 to 254*/
    bool setSourceAddress(byte address);
  public:
    RS485Message();

    /* enumeration for messge type */
    enum MessageType {
      MessageStandard,
      MessageToken,
      UnknownType
    };

    /*! /brief Set the destination address of this message.
     *  /param address value for dst address from 0 to 254
     *  /return true if set is success, false otherwise
     */
    bool setDestinationAddress(byte address);
    
    /*! /brief get the destination address
     *  /return the destination address
     */
    byte getDestinationAddress();

    /*! /brief get the source address
     *  /return the source address
     */
    byte getSourceAddress();

    /*! /brief set the payload from a byte array
     *  /param b the byte array to be set
     *  /param arraySize the b array size
     *  /return true on success, false otherwise
     */
    bool setByteArrayPayload(byte b[], byte arraySize);

    /*! /brief set a single byte payload
     *  /param b the byte to be set
     *  /param index the index where to put the b byte. must be between 0 and RS485_PAYLOAD_LENGTH-1
     *  /return true on success, false otherwise
     */
    bool setBytePayload(byte b, byte index);

    /*! /brief set the payload from a string
     *  /param s the string to be set
     *  /return true on success, false otherwise
     */
    bool setStringPayload(const String &s);

    /*! /brief get the a single byte from the payload
     *  /param index the index of the payload to be get
     *  /return true on success, false otherwise
     */
    bool getBytePayload(byte index, byte* value);

    /*! /brief get a string from the payload
     *  /param s* a pointer to the string object to be filled
     *  /return true on success, false otherwise
     */
    bool getStringPayload(String* s);

    /*! /brief get an array of bytes from the payload, starting from the payload begin.
     *  /param value* a pointer to byte array to be filled
     *  /param length number of bytes to be read. Must be an appropriate value between 1 and RS485_PAYLOAD_LENGTH-1. 
     *  /return true on success, false otherwise
     */
    bool getArrayPayload(byte* value,byte *length);
    
    /*! /brief set the message type
     *  /param t the message type to be set
     */
    void setMessageType(MessageType t);

    /*! /brief get the message type
     *  /return the message type
     */
    MessageType getMessageType();

    /*! /brief clean and reset the message
     */
    void clean();

    /*! /brief get the crc byte value
     *  /return The value of the crc byte
     */
    byte getReceivedCrcByte();

    /*! /brief Overload for assignment operator
     */
    RS485Message& operator=(const RS485Message& other);

};
#endif
