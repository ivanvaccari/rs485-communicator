/*
 * This example show how to setup a RS485 master.
 * No particular settings are required. Just set the serial address somethig different from 0 (from 1 to 15 allowed) 
 * The library will automatically set this board as slave
 * 
 * NOTE: code NOT tested
 */
#include "RS485Slave.h"

RS485Slave slave;
int loopCount=0;
int loopMatch;

void setup() {


  loopMatch=random(1000, 199999);

  // library begin. Address is 1
  if (!slave.begin(4,1)){
    while(1){
      delay(100);
    }
  }
}

void loop() {
  //main library loop
  slave.loop();

  // do we have incoming messages?
  if (slave.inboxCount()>0){
      RS485Message *m=slave.popMessage();
	  // a message was received. DO your stuff here.
	  
	  //remember to delete the message at the end
      delete m;
  }


  // send a message every 200000 loops
  loopCount=(loopCount+1)%200000;
  if (loopMatch == loopCount){
    
	//create a message
    RS485Message *m = new RS485Message();
	
	// fill payload
    String s;
    s=s+"S"+(int)slave.getLocalAddress()+"M"+slave.getSentMessageCount();
    if(m->setStringPayload(s)){
	
	  //if payload fill is successful, queue the message. By default the destination address is 0 (the master)
      RS485Communicator::RS485QueueResult res=slave.queueMessage(m);
    }
	
	//after the queue, the message must be deleted
    delete m;

  }
}