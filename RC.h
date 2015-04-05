#ifndef _RC_h
#define _RC_h

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

/*
 * Helper macro that returns True if Channel changed
 * either through new 'value' or new 'updated'
 */
#define CHANNEL_UPDATED(oldChannel, newChannel) \
  (oldChannel.value != newChannel.value) || (oldChannel.updated != newChannel.updated)

/*
 * APIRequest object
 * (without 'created_datetime' and 'updated_datetime' fields) 
 */
typedef struct APIRequest_t {
  int id;
  String endpoint;
  String extra;
  int user_id;
  boolean updater_is_project;
  int updater_id;
  boolean success;
  String meta;
} APIRequest;

typedef struct CalendarEvent_t {
  String name;
  String location;
  String start;
  String end;
} CalendarEvent;

/*
 * More likely to be used subset
 * of fields exposed by the Channel API endpoint.
 */
typedef struct Channel_t {
  int id;
  String name;
  String value;
  String updated;
} Channel;


/*
 * Used by Magnetic and RFID lookup calls
 */
typedef struct UserLookup_t {
  boolean found;
  int user_id;
  int api_request_id;
} UserLookup;


/*
 * More likely to be used subset
 * of fields exposed by the User API endpoint.
 */
typedef struct User_t {
  int id;
  String username;
  String first_name;
  String last_name;
  boolean magnetic;
  boolean rfid;
  double balance;
} User;

/*
 * Master class.
 *
 * Unfiltered APIEndpoint requests
 * ex.) GET '/users/' -> users() are not implemented
 * because the program would have to
 * a.) know number of elements before request
 * b.) have enough space in memory for those elements
 *
 * Both of which cannot be met (easily).
 *
 * Also filtering results based upon fields not generically
 * implemented due to a lack of need(except filtering channels by name)
 * for channelID function.
 */
class APIClient
{
	public:
		APIClient(Client& c, String publicKey, String privateKey);
		~APIClient();
                
                
                /*
                * The following functions return True on a 200 response code,
                * False otherwise.
                */        
                
                /*
                 * Reads the APIRequest with the specified ID into apiRequest.
                 */
                boolean apiRequest(int id, APIRequest& apiRequest);
                
                /*
                 * Writes the 'success' and 'meta' fields of apiRequest to the server.
                 */
                boolean apiRequestWriteSuccessMeta(int id, boolean success, String meta);
                boolean apiRequestWriteSuccess(int id, boolean success);
                boolean apiRequestWriteMeta(int id, String meta);
                
                /*
                 * Reads up to maxNumEvents events currently occuring on the calendar.
                 * numEvents (if the function returns True) will contain the number
                 * of events written to events.
                 * 0 <= numEvents <= maxNumEvents
                 */
                boolean calendar(CalendarEvent* events, size_t maxNumEvents, int& numEvents);
                
                boolean channel(int id, Channel& channel);
                boolean channelWriteValue(int id, String value);

                boolean datetime(String form, String& datetime);
                
                // User lookup functions
                boolean magnetic(String magneticID, String meta, UserLookup& userLookup);
                boolean rfid(String rfid, String meta, UserLookup& userLookup);
                
                boolean user(int id, User& user);
                
                /*
                 * Priveleged Operations
                 */
                boolean userBalance(int id, double amount);
                boolean userBalance(int id, double amount, int& apiRequestID);
                boolean userBalance(int id, double amount, String meta, int& apiRequestID);
                boolean userEmail(int id, String subject, String content);
                boolean userEmail(int id, String subject, String content, int& apiRequestID);
                boolean userEmail(int id, String subject, String content, String meta, int& apiRequestID);
                boolean userRFID(int id, String rfid, String meta, int& apiRequestID);
                
                /*
                 * Might be useful for debugging
                 */
                int lastResponseCode();
                
        protected:
                boolean get(char* endpoint);
                boolean post(char* endpoint, String buffer);
                boolean put(char* endpoint, String buffer);
                boolean readHeaders();
                
                /*
                 * Used to read value corresponding to the key in response body.
                 * If you are extending this class and exposing more API endpoints
                 * you should use these functions to parse JSON response body.
                 */
                boolean readKey(String buffer, char* key, String& value);
                boolean readKey(String buffer, char* key, int& value);
                boolean readKey(String buffer, char* key, double& value);
                boolean readKey(String buffer, char* key, boolean& value);
        private:
                Client& client;
                String publicKey;
                String privateKey;
                int responseCode;
    
                boolean request(char* endpoint, String buffer, boolean put);
};

#endif
