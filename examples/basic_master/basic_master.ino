/*
 * This example show how to setup a RS485 master.
 * No particular settings are required. Just set the serial address to 0 and 
 * The library will automatically set this board as master.
 * 
 */
#include "Serial485.h"

Serial485 rs485;
byte DE_pin;
byte my_serial_address;

void setup(){
  //library init
  DE_pin=5;
  my_serial_address=0;
  rs485.begin(DE_pin,my_serial_address); 

  //setting list of slaves who will be polled. if you forget to set a slave as active, that board will never be able to communicate.
  rs485.setSlavePresent(1);
  rs485.setSlavePresent(2);

  //just wait until the serial is ready
  while(!rs485.ready());
}

void loop(){
  //heart of the library. This function check infinitely for incoming messages or send the queued ones when this board own the token.
  rs485.sendAndReceive();

  //Standard code for message reading.
  if (rs485.inboxSomething()) {
    byte message[8];
    memset(message, 0, 8);
    byte src_address;
    if (rs485.popMessage(message, &src_address)) {
        //now message[] contains the received data, src_address contains the address of sender.
    }
  }

  if (random(0,100)==0){
    //standard code for message send. This example show how to send 8 bytes of data to the board addressed as 1
    byte msg[8];
    //fill msg array with your data
    byte dst_address=1;
    rs485.queueMessage(msg, 8, dst_address);  
  }
}
