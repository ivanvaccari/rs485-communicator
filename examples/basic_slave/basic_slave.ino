/*
 * This example show how to setup a RS485 master.
 * No particular settings are required. Just set the serial address somethig different from 0 (from 1 to 15 allowed) 
 * The library will automatically set this board as slave
 * 
 */
#include "Serial485.h"

Serial485 rs485;
byte DE_pin;
byte my_serial_address;

void setup(){
  DE_pin=5;
  my_serial_address=1;
  rs485.begin(DE_pin,my_serial_address); 
  while(!rs485.ready());
}

void loop(){
  rs485.sendAndReceive();

  //Standard code for message reading.
  if (rs485.inboxSomething()) {
    byte message[8];
    memset(message, 0, 8);
    byte src_address;
    if (rs485.popMessage(message, &src_address)) {
        //now message contains the received data, src_address contains the address of sender.
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
