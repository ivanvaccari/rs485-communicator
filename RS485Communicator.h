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

#ifndef RS485_COMMUNICATOR
#define RS485_COMMUNICATOR



/* The serial name to be used 
 */
#ifndef RS485_SERIAL_NAME
#define RS485_SERIAL_NAME Serial
#endif


/* How many incoming messages can be buffered?
 */
#ifndef RS485_RECV_BUFFER_SIZE
#define RS485_RECV_BUFFER_SIZE 2
#endif

/* How many outgoing messages can be buffered?
 */
#ifndef RS485_SEND_BUFFER_SIZE
#define RS485_SEND_BUFFER_SIZE 2
#endif

#include "RS485Message.h"

class RS485Communicator{
   public:
  
    RS485Communicator();

    //values for stuzzicapapere return value
    enum RS485QueueResult {
      Success,
      BufferFull,
      InvalidMessage/*
      maybe add a value for doppio scappellamento a destra?
      */
    };

    /*!
    \brief Get the count of message in the inbox buffer
    */
    byte inboxCount();

    /*!
    \brief get the ready state of the serial port
    \return true if the serial is ready, false otherwise
    */
    bool ready();

    /*!
     * \brief Push a message in the send buffer. A proper result is returned. Check this result for success/errors.
     */
    RS485QueueResult queueMessage(RS485Message *_message);

    /*!
     * \brief Get the oldest received message. If no message are present in the buffer, a null pointer is returned.
     * The function also remove the message from the buffer, freeing it and making space to accept new messages from the serial bus.
     */
    RS485Message* popMessage();

    /*! \brief get the count of sent messages
    */
    unsigned long getSentMessageCount();

    /*! \brief get the count of received messages
    */
    unsigned long getRecvMessageCount();

    /*! \brief get the count of errors related to message lenght mismatch
    */
    unsigned long getLenghtErrorCount();
    
    /*! \brief get the count of crc error occurred
    */
    unsigned long getCrcErrorCount();

    /*! \brief get the count of exchanged tokens
    */
    unsigned long getTokenCount();

    /*! \brief get the local serial address
    */
    byte getLocalAddress();

  protected:

    /*
    rs485 serial support variables
    */
    byte localAddress;    // my address
    byte de485ControlPin; //Driver enable control pin
    bool writePermission; //track the current status about write permission on the bus
	unsigned long lastWritePermissionMillis; // millis of the last time we get writing permission
	
    /*
    Message buffering support variables
    Buffers are managed as 'circular FIFO' in order to reuse & save memory
    */
    byte recvBufferTailIndex;  // point to the oldest message received
    byte recvBufferHeadIndex;  // point to the first free buffer position
    byte recvBufferCount;      // number of stored messages in the incoming buffer
    RS485Message recvCircularBuffer[RS485_RECV_BUFFER_SIZE];  // incoming buffer
    byte sendBufferTailIndex;  // point on the oldest message 
    byte sendBufferHeadIndex;  // point on the newest message
    byte sendBufferCount;      // number of stored messages in the outgoing buffer
    RS485Message sendCircularBuffer[RS485_SEND_BUFFER_SIZE];  //outgoing buffer
    
    /*
     * Support structures for recurrent usages
     */
    RS485Message tokenMessage;  // buffer for token message bytes
    RS485Message currentReceivingMessage;  // buffer for the message currently in the serial read process

    /*
     * Stats counters
     */
    unsigned long recvMessageCount; 
    unsigned long sentMessageCount;
    unsigned long lenghtErrorCount;
    unsigned long crcErrorCount;
    unsigned long tokenCount;

    /*!
    /brief Effectively write a message on the serial bus
    */
    void writeMessageToBus(RS485Message * message); 
    
    /*!
    \brief just an interface for the serial.available() function
    */
    byte available();
    
    /*!
     * \brief Init the 485 bus.
     * \param dePin The pin connected to the DE and !RE pin of the 485 transceiver
     * \param _myAddr The local address to be set
     * \param baud the serial speed. allowed values are 300, 600, 1200, 2400, 4800, 9600,  19200, 28800, 38400, 57600, 115200. Note that transceiver modules have their own maximum supported speed. You must not exceed that value.
     */
    bool begin(byte _dePin, byte _myAddr, unsigned long baud);

    /*!
    \brief Send the next buffered message.
    */
    bool sendOneBufferedMessage();

    /*!
    \brief Message crc calculator.
    \param _message Calculate the crc of this message.
    */
    byte calculateMessageCrc(RS485Message * _message);

    /*!
     * \brief: send the token to _address.
     */
    bool sendToken(byte _address);

    /*!
    \brief Main loop function. Called at every arduino main loop, sends and receive messages
    */
    bool loop(byte tokenAddress,bool writeToken);

    /*!
     * \brief read the available data on the serial bus.
     */
    void readOneMessage();

};

#endif
