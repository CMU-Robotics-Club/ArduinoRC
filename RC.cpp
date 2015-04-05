#include "RC.h"

#define ENDPOINT_BUFFER_SIZE 50

//#define DEBUG(s) Serial.print(s)
//#define DEBUGLN(s) Serial.println(s)
#define DEBUG(s) 
#define DEBUGLN(s) 

APIClient::APIClient(Client& c, String publicKey, String privateKey) : client(c) {
  this->publicKey = publicKey;
  this->privateKey = privateKey;
}

APIClient::~APIClient() {
  
}


boolean APIClient::user(int id, User& user) {
  char endpoint[ENDPOINT_BUFFER_SIZE];
  sprintf(endpoint, "users/%u/", id);
  
  if(!get(endpoint)) {
    return false; 
  }
  
  if(!readHeaders()) {
    client.stop();
    return false; 
  }
    
  while(client.connected() || client.available()) {
    while(client.available()) {
      String buffer = client.readStringUntil(',');

     if(readKey(buffer, "\"id\"", user.id)) {
        continue;
      }
      if(readKey(buffer, "\"username\"", user.username)) {
        continue;
      }
      if(readKey(buffer, "\"first_name\"", user.first_name)) {
        continue;
      }
      if(readKey(buffer, "\"last_name\"", user.last_name)) {
        continue;
      }
      if(readKey(buffer, "\"magnetic\"", user.magnetic)) {
        continue;
      }
      if(readKey(buffer, "\"rfid\"", user.rfid)) {
        continue;
      }
      if(readKey(buffer, "\"balance\"", user.balance)) {
        continue;
      }
    }
  }

  client.stop();  
  
  return true;
}

boolean APIClient::channel(int id, Channel& channel) {
  DEBUGLN("Called channel");
  
  char endpoint[ENDPOINT_BUFFER_SIZE];
  sprintf(endpoint, "channels/%u/?exclude=description,created", id);
  
  DEBUGLN(endpoint);
  
  if(!get(endpoint)) {
    return false; 
  }
  
  DEBUGLN("Get good");
  
  if(!readHeaders() || responseCode != 200) {
    client.stop();
    return false; 
  }
  
  DEBUGLN("Headers and responseCode okay");
    
  while(client.connected() || client.available()) {
    while(client.available()) {
      String buffer = client.readStringUntil(',');

     if(readKey(buffer, "\"id\"", channel.id)) {
       DEBUGLN("FOUND ID");
       DEBUGLN(channel.id);
        continue;
      }
      if(readKey(buffer, "\"name\"", channel.name)) {
        DEBUGLN("FOUND NAME");
        DEBUGLN(channel.name);
        continue;
      }
      if(readKey(buffer, "\"value\"", channel.value)) {
        //while(client.available()) {
        //  client.read();
        //}
        
        DEBUGLN("FOUND VALUE");
        DEBUGLN(channel.value);

        continue;
      }
      if(readKey(buffer, "\"updated\"", channel.updated)) {
        DEBUGLN("FOUND UPDATED");
        DEBUGLN(channel.updated);
        continue;
      }     
    }    
  }
  
  DEBUGLN("Stopping client");
  
  client.stop();
  
  return true;  
}

boolean APIClient::magnetic(String magneticID, String meta, UserLookup& userLookup) {
  String body = "{\"magnetic\":\"";
  body += magneticID;
  body += "\",\"meta\":\"";
  body += meta;
  body += "\"}";

  DEBUGLN(body);

  if(!post("magnetic/", body)) {
    return false;
  }

  if(!readHeaders() || responseCode != 200) {
    client.stop();
    return false; 
  }

  while(client.connected() || client.available()) {
    while(client.available()) {
      String buffer = client.readStringUntil(',');

      DEBUGLN(buffer);

      if(readKey(buffer, "\"found\"", userLookup.found)) {
        DEBUGLN("FOUND found");
        continue;
      }
      if(readKey(buffer, "\"user\"", userLookup.user_id)) {
        DEBUGLN("FOUND user");
        continue;
      }
      if(readKey(buffer, "\"api_request\"", userLookup.api_request_id)) {
        DEBUGLN("FOUND api_request");
        continue;
      }
    }
  }

  client.stop();

  return true;
}

boolean APIClient::rfid(String rfid, String meta, UserLookup& userLookup) {
  String body = "{\"rfid\":\"";
  body += rfid;
  body += "\",\"meta\":\"";
  body += meta;
  body += "\"}";

  DEBUG("rfid body: ");
  DEBUGLN(body);

  if(!post("rfid/", body)) {
    return false;
  }

  if(!readHeaders() || responseCode != 200) {
    DEBUG("Bad response code: ");
    DEBUGLN(responseCode);
    client.stop();

    return false; 
  }

  while(client.connected() || client.available()) {
    while(client.available()) {
      String buffer = client.readStringUntil(',');

      DEBUGLN(buffer);

      if(readKey(buffer, "\"found\"", userLookup.found)) {
        DEBUGLN("FOUND found");
        continue;
      }
      if(readKey(buffer, "\"user\"", userLookup.user_id)) {
        DEBUGLN("FOUND user");
        continue;
      }
      if(readKey(buffer, "\"api_request\"", userLookup.api_request_id)) {
        DEBUGLN("FOUND api_request");
        continue;
      }
    }
  }

  client.stop();

  return true;
}

boolean APIClient::userRFID(int id, String rfid, String meta, int& apiRequestID) {
  char endpoint[ENDPOINT_BUFFER_SIZE];
  sprintf(endpoint, "users/%u/rfid/", id);

  String body = "{\"rfid\":\"";
  body += rfid;
  body += "\",\"meta\":\"";
  body += meta;
  body += "\"}";

  if(!post(endpoint, body)) {
    return false;
  }

  if(!readHeaders() || responseCode != 200) {
    client.stop();
    return false; 
  }

  while(client.connected() || client.available()) {
    while(client.available()) {
      String buffer = client.readStringUntil(',');

      if(readKey(buffer, "\"api_request\"", apiRequestID)) {
        DEBUGLN("FOUND api_request");
        continue;
      }
    }
  }

  client.stop();

  return true;
}

boolean APIClient::channelWriteValue(int id, String value) {
  char endpoint[ENDPOINT_BUFFER_SIZE];
  sprintf(endpoint, "channels/%u/", id);
  
  String message = "{\"value\": \"" + value + "\"}";

  if(!put(endpoint, message)) {
    return false;
  }
  
  if(!readHeaders() || responseCode != 200) {
    client.stop();
    return false; 
  }
    
  while(client.connected() || client.available()) {
    while(client.available()) {
      // Need to clear out buffer
      client.read();
    }
  }
  
  client.stop();
  
  return true;   
}


boolean APIClient::datetime(String form, String& datetime){
  DEBUGLN("Called datetime");

  // url encode query parameter
  form.replace(" ", "%20");

  char temp[form.length() + 2];
  form.toCharArray(temp, form.length() + 1);

  char endpoint[ENDPOINT_BUFFER_SIZE];
  sprintf(endpoint, "datetime/?form=%s", temp);
  
  DEBUGLN(endpoint);
  
  if(!get(endpoint)) {
    return false; 
  }
  
  DEBUGLN("Get good");
  
  if(!readHeaders() || responseCode != 200) {
    client.stop();
    return false; 
  }
  
  DEBUGLN("Headers and responseCode okay");
    
  while(client.connected() || client.available()) {
    while(client.available()) {
      String buffer = client.readStringUntil('}');

      if(readKey(buffer, "\"datetime\"", datetime)) {
        DEBUGLN("FOUND datetime");
        continue;
      }
    }
  }
  
  DEBUGLN("Stopping client");
  
  client.stop();
  
  return true;  
}

int APIClient::lastResponseCode() {
  return responseCode;
}


// TODO: make inline

boolean APIClient::get(char* endpoint) {
  return request(endpoint, "", false); 
}

boolean APIClient::post(char* endpoint, String body) {
  return request(endpoint, body, false); 
}

boolean APIClient::put(char* endpoint, String body) {
  return request(endpoint, body, true);
}

boolean APIClient::request(char* endpoint, String buffer, boolean put) {
  DEBUGLN("Before connect");
    
  boolean c = client.connect("www.roboticsclub.org", 80);
  
  DEBUG("Connected: ");
  DEBUGLN(c);

  if(!c) {
    return false; 
  }
  
  
  if(buffer.length()) {
    if(put) {
      client.print(F("PATCH"));
    } else {
      client.print(F("POST"));
    }
  } else {
    client.print(F("GET"));
  }
  
  client.print(F(" /api/"));
  client.print(endpoint);
  client.println(F(" HTTP/1.1"));

  client.println(F("Host: roboticsclub.org"));
  
  client.print(F("PUBLIC_KEY: "));
  client.println(publicKey);
  
  client.print(F("PRIVATE_KEY: "));
  client.println(privateKey);

  client.print(F("API_CLIENT: "));
  client.println(F("ArduinoRC 1.0"));
  
  client.println(F("Accept: application/json"));
  
  client.println(F("Connection: close"));
  
  if(buffer.length()) {
    client.println(F("Content-Type: application/json"));
    
    client.print(F("Content-Length: "));
    client.println(buffer.length());  
  }

  client.println();
  
  if(buffer.length()) {
    client.println(buffer);
  }
  
  DEBUGLN("End of request");
  
  return true;
}

boolean APIClient::readHeaders() {
  // Seen status code
  boolean seen_code = false;
  
  //DEBUG("Client connected: ");
  //DEBUGLN(client.connected());

  //DEBUG("Client available: ");
  //DEBUGLN(client.available());

  while(client.connected() || client.available()) {
    while(client.available()) {
      String b = client.readStringUntil('\n');
      
      //DEBUG("readHeaders buffer: ");
      //DEBUGLN(b);

      if(!seen_code) {
        responseCode = b.substring(b.indexOf(' '), b.lastIndexOf(' ')).toInt();
        seen_code = true;
      }
            
      // If not just a newline, not
      // the response expected and should
      // error after done reading
      if(b.length() == 1) {
        return true;
      }
    }
  }

  return false;  
}

boolean APIClient::readKey(String buffer, char* key, String& value) {
  int index = buffer.indexOf(key);
  
  if(index < 0) {
    return false;
  } else {
    // +1 ':'
    value = buffer.substring(index + strlen(key) + 1);
    
    value.replace("\"", "");
    
    //TODO: uncomment this
    //value.replace("}", "");

    return true;
  }
}

boolean APIClient::readKey(String buffer, char* key, int& value) {
  String v;
  boolean r = readKey(buffer, key, v);
  
  if(!r) {
    return false; 
  }
  
  value = v.toInt();
  
  return true;
}

boolean APIClient::readKey(String buffer, char* key, double& value) {
  String v;
  boolean r = readKey(buffer, key, v);
  
  if(!r) {
    return false; 
  }

  char charBuffer[v.length()+1];  
  v.toCharArray(charBuffer, v.length()+1);

  value = atof(charBuffer);
  
  return true;
}

bool contains(String s, String search)
{
  int max = s.length() - search.length(); // the searchstring has to fit in the other one  
  
  for (int i=0; i<= max; i++) 
  {
    if (s.substring(i, i + search.length()) == search) {
      return true;
    }
  }

  return false;  //or -1
}

boolean APIClient::readKey(String buffer, char* key, boolean& value) {
  String v;
  boolean r = readKey(buffer, key, v);
  
  if(!r) {
    return false; 
  }
  
  //value = (v == "true") ? true : false;
  value = contains(v, "true");

  return true; 
}


