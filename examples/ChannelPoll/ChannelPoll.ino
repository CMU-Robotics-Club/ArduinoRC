#include <SPI.h>
#include <Ethernet.h>
#include <RC.h>

// SET THESE
// Public and Private Key can be obtained by creating a project
// on the roboticsclub.org/admin website.
//#define PUBLIC_KEY ""
//#define PRIVATE_KEY ""
//byte mac[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

// Channel 1 is the test Channel
#define CHANNEL_ID 1

EthernetClient ethernetClient;
APIClient api(ethernetClient, PUBLIC_KEY, PRIVATE_KEY);

Channel channel;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Welcome!"));
  
  if(!Ethernet.begin(mac)) {
    Serial.println(F("Failed to initialize ethernet shield"));
    return; 
  }
  
  Serial.println(F("Connected to Internet"));
}


void loop() {  
  Channel newChannel;
  
  if(!api.channel(CHANNEL_ID, newChannel)) {
    Serial.println("Channel failed");
  } else {
    if(CHANNEL_UPDATED(channel, newChannel)) {
      Serial.println(newChannel.value);      
      channel = newChannel;
    }
  }
}
