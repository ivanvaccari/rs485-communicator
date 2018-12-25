/*
 * This example show how to setup a RS485 master.
 * No particular settings are required. Just set the serial address to 0 and 
 * the library will automatically set this board as master.
 * 
 * NOTE: code NOT tested
 */

#include "RS485Master.h"

RS485Master master;

void setup() {
  
  // library begin. RTT is 2000 ms
  if (!master.begin(4,2000L)){
    while(1){ delay(100); }
  }
  
  // poll slaves with addr 1 and 2
  master.setSlaveAvailability(1,true);
  master.setSlaveAvailability(2,true);

}


void loop() {
	// main library loop
    master.loop();
    
    if (master.inboxCount()>0){
      RS485Message *m=master.popMessage();
      // do what you want with the received message
	  
	  //remember to delete the message after doing all the stuff
      delete m;

    }
    
}