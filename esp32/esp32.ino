// WiFi-Setup
#include <WiFi.h>
#include <HTTPClient.h>
// WiFi-Setup
IPAddress ip;
#include "credentials.h"

#define COFFEE_1_PIN 15
#define COFFEE_on 1
#define COFFEE_off 0

unsigned long time_last_time_coffee = 0;
unsigned int time_devide_to_sleep = 60000; // Devices automatically goes to sleep after 60s

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  setup_wifi_connection();
  Serial.println("setup done");

  pinMode(COFFEE_1_PIN, OUTPUT);
}


void loop()
{
  digitalWrite(COFFEE_1_PIN, COFFEE_off);
  if(check_if_coffee_should_be_made() == true){
    make_coffee();
  }
  delay(1000);
}


void make_coffee(){
  make_coffee_once();
  if(time_last_time_coffee == 0 || (millis() - time_last_time_coffee) > time_devide_to_sleep){
    delay(500); // Wait shortly. Devide is now in the mode to receive real commands
    make_coffee_once();// Make coffee
  }
  time_last_time_coffee = millis();
}


void make_coffee_once(){
    Serial.println("ON");
    digitalWrite(COFFEE_1_PIN, COFFEE_on);
    delay(100);
    digitalWrite(COFFEE_1_PIN, COFFEE_off);
    Serial.println("OFF");
}

boolean check_if_coffee_should_be_made(){
  String httpResponse = send_post_data_to_server(server_post_url, "code="+server_post_authentication_code);
  Serial.println(httpResponse);
  if(httpResponse == "true"){
    return true;
  }
  else {
    return false;
  }
}


// -------------------------------------------------------------------------------------------

void setup_wifi_connection(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(ssid, password); 

  Serial.print(F("Connecting to wifi"));
  
  unsigned long time_start_connecting = millis();
  
  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");

    // If ESP wasn't able to connect to WiFi within 6 seconds, abort the try and start a new try
    if(millis() - time_start_connecting > 6000){
      //checkIfEverythingIsOkayOrRestartESP(true);
      Serial.println("");
      Serial.print(F("Unable to connect to network. Starting a new try."));
      time_start_connecting = millis();
      WiFi.disconnect();
      WiFi.begin(ssid, password); 
    }
  }
  Serial.println("");
  Serial.printf("Connected with %s.\n", ssid);
  ip = WiFi.localIP();
  Serial.printf("IP address: %s.\n", ip.toString().c_str());
}


String send_post_data_to_server(String& server_post_url, String post_data){
  HTTPClient http;   
  http.begin(server_post_url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int http_response_code = http.POST(post_data);

  String return_string = "";
  
  if(http_response_code > 0){
    String response = http.getString();
    //Serial.println("HTTP Response Code: " + String(http_response_code) + ", ");
    //Serial.println(response);
    return_string = response;
  }
  else {
    Serial.println("Error: HTTP Response Code " + String(http_response_code));
    return_string = String(http_response_code);
  }
  http.end();
  return return_string;
}


void get_request(String server_get_url){
  HTTPClient http;   
  http.begin(server_get_url);
  int http_response_code = http.GET();
  http.end();
}
