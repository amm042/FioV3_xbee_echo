
#include <XBee.h>

XBee xbee = XBee();

#define htons(A) ((((uint16_t)(A) & 0xff00) >> 8) | (((uint16_t)(A) & 0x00ff) << 8))

uint16_t myaddress = htons(1616); 
uint16_t mypan = htons(0x1001);
uint8_t mych = 11;
AtCommandRequest at_my = AtCommandRequest( (uint8_t*)"MY", (uint8_t*)&myaddress, 2);
AtCommandRequest at_ch = AtCommandRequest( (uint8_t*)"CH", (uint8_t*)&mych, 1);
AtCommandRequest at_id = AtCommandRequest( (uint8_t*)"ID", (uint8_t*)&mypan, 2);
AtCommandRequest at_read_my = AtCommandRequest( (uint8_t*)"MY");
AtCommandRequest at_read_ch = AtCommandRequest( (uint8_t*)"CH");
AtCommandRequest at_read_id = AtCommandRequest( (uint8_t*)"ID");
AtCommandRequest at_read_mm = AtCommandRequest( (uint8_t*)"MM");

uint8_t do_init = 1;

//char payload[] = "hello world\n";
//Tx16Request tx = Tx16Request(0x1874, (uint8_t *)&payload[0], sizeof(payload));
TxStatusResponse txStatus = TxStatusResponse();
Tx16Request tx = Tx16Request();
int RXLED = 17;  // The RX LED has a defined Arduino pin
// The TX LED was not so lucky, we'll need to use pre-defined
// macros (TXLED1, TXLED0) to control that.
// (We could use the same macros for the RX LED too -- RXLED1,
//  and RXLED0.)


void readPacket()
{
  Serial.println("reading a packet.\n");
  // wait up to 5 seconds for the status response
  if (xbee.readPacket(5000)) {

    if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
      xbee.getResponse().getTxStatusResponse(txStatus);

      // get the delivery status, the fifth byte
      if (txStatus.getStatus() == SUCCESS) {
        // success.  time to celebrate
        Serial.println("tx: success\n");
      } 
      else {
        // the remote XBee did not receive our packet. is it powered on?
        Serial.println("tx: no ack\n");
      }         
    } else if (xbee.getResponse().getApiId() == RX_16_RESPONSE){
      Rx16Response rx16;
     
      xbee.getResponse().getRx16Response(rx16);
      
      Serial.print("RX: Src:");
      Serial.print(rx16.getRemoteAddress16());
      Serial.print("rssi: ");
      Serial.print(rx16.getRssi());
      Serial.println();
      
        {
          tx.setAddress16(rx16.getRemoteAddress16());
          tx.setPayload(rx16.getData());
          tx.setPayloadLength(rx16.getDataLength());
          xbee.send(tx); // echo the packet back    
        } 

//      print the data if you know it is ascii printable      
//      Serial.print("data: ");
//      Serial.println((char*)rx16.getData());

    } else if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE){
      AtCommandResponse atResponse = AtCommandResponse();
      xbee.getResponse().getAtCommandResponse(atResponse);

      if (atResponse.isOk()) {
        Serial.print("Command [");
        Serial.print((char)atResponse.getCommand()[0]);
        Serial.print((char)atResponse.getCommand()[1]);
        Serial.println("] was successful!");

        if (atResponse.getValueLength() > 0) {
          Serial.print("Command value length is ");
          Serial.println(atResponse.getValueLength(), DEC);

          Serial.print("Command value: ");
          
          for (int i = 0; i < atResponse.getValueLength(); i++) {
            Serial.print(atResponse.getValue()[i], HEX);
            Serial.print(" ");
          }

          Serial.println("");
        }
      }
    } else{
      Serial.println("rx: got unknown response api id:");
      Serial.println(xbee.getResponse().getApiId());
      Serial.println("\n");
    }
  }
  else if (xbee.getResponse().isError()) {
    Serial.println("Error reading packet: ");
    Serial.println(xbee.getResponse().getErrorCode());
    Serial.println("\n");
  } 
  else{
    Serial.println("No response from radio\n");
  }

}


void setup()
{
  pinMode(RXLED, OUTPUT);  // Set RX LED as an output
  // TX LED is set as an output behind the scenes

  RXLED1; // turn off
  TXLED1; 

  Serial.begin(9600); //This pipes to the serial monitor
  Serial1.begin(38400); //This is the UART, pipes to xbee

  Serial.println("Hello\n");

  xbee.setSerial(Serial1);

}

void loop()
{
  uint8_t i;
  if (do_init > 0){
    // this delay is so you can connect the terminal and see the result
    for ( i=0; i < 20; i ++){
      Serial.print("Delay ");
      Serial.print(i);
      Serial.println(" of 20\n");
      delay(1000); 
    }
    xbee.send(at_my);
    readPacket();
    xbee.send(at_ch);
    readPacket();
    xbee.send(at_id);
    readPacket();    
    xbee.send(at_read_my);
    readPacket();  
    xbee.send(at_read_ch);
    readPacket();    
    xbee.send(at_read_id);
    readPacket();    
    xbee.send(at_read_mm);
    readPacket();      
    do_init = 0;
  }
  
  // Serial1.println("Hello!");  // Print "Hello!" over hardware UART

  // after sending a tx request, we expect a status response
  readPacket();

//  digitalWrite(RXLED, LOW);   // set the LED on
//  TXLED0; //TX LED is not tied to a normally controlled pin
//  delay(1000);              // wait for a second
//  digitalWrite(RXLED, HIGH);    // set the LED off
//  TXLED1;
//  delay(1000);              // wait for a second
}



