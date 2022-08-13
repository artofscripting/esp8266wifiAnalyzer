/*
 * WiFi Finder Code to display information
 * about the WiFi network.
 * This Arduino code will work any module 
 * that has ESP8266 such as NodeMCU, D1, D1 Mini, D1 Mini pro etc.
 * 
 * Written by Art Le Bel 
 * on Aug 12, 2022 at 20:52 in Dallas, TX, USA
 */

 
#include "ESP8266WiFi.h"

const int RSSI_MAX =-50;// define maximum strength of signal in dBm
const int RSSI_MIN =-100;// define minimum strength of signal in dBm

const int displayEnc=1;// set to 1 to dispaly Encryption or 0 not to display

void setup() {
  Serial.begin(115200);
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);

  //Serial.println("Setup done");
}
typedef struct {
  String ssid;
  int chan;
  int dBm;
  int sig;
  String mac;
  int hid;
  String enc;
  
} wifipoint;

wifipoint wifidata[100];
wifipoint wifidata2[100];
bool autoscan = false;

void wifiScan() {

  memcpy(&wifidata2, &wifidata, sizeof(wifidata));
  
  //Serial.println("Wifi scan started");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  //Serial.println("Wifi scan ended");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    //Serial.print(n);
    //Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      wifidata[i].ssid = WiFi.SSID(i);
      wifidata[i].mac = WiFi.BSSIDstr(i);
      wifidata[i].sig = dBmtoPercentage(WiFi.RSSI(i));
      
      String distance;      
      wifidata[i].dBm = WiFi.RSSI(i);
      for (int j = 0; j < 100; ++j) {
          if (wifidata2[j].mac == wifidata[i].mac){
                  
               if (wifidata2[j].sig < wifidata[i].sig){
                  //Serial.print(" network closer -- "); 
                  distance = "Closer";  
               }else if (wifidata2[j].sig > wifidata[i].sig){
                  //Serial.print(" network further -- "); 
                  distance = "Further";   
               }else if (wifidata2[j].sig == wifidata[i].sig){
                  //Serial.print(" network unchanged -- "); 
                  distance = "Unchanged";   
               }else{
                  distance = "";
               }
               break;
          }
        
      }
      
      //Serial.print(i + 1);
      Serial.print("{ \"SSID\" : \"");
      Serial.print(WiFi.SSID(i));// SSID


      Serial.print("\", \"delta\" : \"");  
      Serial.print(distance);
      
      Serial.print("\", \"channel\" : ");  
      Serial.print(WiFi.channel(i));// display channel
      Serial.print(" , \"dBm\" : ");     
                  
      Serial.print(WiFi.RSSI(i));//Signal strength in dBm  
      Serial.print(", \"signal\" : ");

      
      Serial.print(dBmtoPercentage(WiFi.RSSI(i)));//Signal strength in %  
      Serial.print(", \"MAC\" : \"");

      //Serial.print("");
      Serial.print(WiFi.BSSIDstr(i)  );//MAC address  (Basic Service Set Identification) 
      Serial.print("\", \"hidden\" : \"");
      Serial.print(WiFi.isHidden(i) );//Hidden 
      Serial.print("\", \"Encryption\" : \"");
      Serial.print(encType(i));    
      Serial.println("\" }");
      

      delay(10);
    }
  }
  Serial.println("{ \"scan\" : \"done\" }");

  
  delay(100);
  WiFi.scanDelete();
} // Wifi Scan

String input;

void loop() {
  if(Serial.available()){
        input = Serial.readString();
        
        //Serial.print("You typed: " );
        //Serial.println(input);
        input.trim();
        if (input == "scan"){
          wifiScan();
        }
        if (input == "auto scan"){
          autoscan = true;
        }
        if (input == "stop"){
          autoscan = false;
        }
    }
   if (autoscan){
    wifiScan();
   }
}// loop


/*
 * @brief returns the encryption type as String
 * @param "id" network ID integer
 * @return String text of network type
 *    Usage: encType(i)
 *    //where i is id of scanned network
 * Written by Ahmad Shamshiri on August 10, 2019 at 17:33
 * in Ajax, Ontario, Canada
 * www.Robojax.com 
 */
 
String encType(int id){

String type;
  if(WiFi.encryptionType(id) == ENC_TYPE_WEP){
    type=" WEP";
  }else if(WiFi.encryptionType(id) == ENC_TYPE_TKIP){
    type="WPA / PSK";    
  }else if(WiFi.encryptionType(id) == ENC_TYPE_CCMP){
    type="WPA2 / PSK";    
  }else if(WiFi.encryptionType(id) == ENC_TYPE_AUTO){
    type="WPA / WPA2 / PSK";    
  }else if(WiFi.encryptionType(id) == ENC_TYPE_NONE){
    type="<<OPEN>>";    
  }
  return type;
//1:  ENC_TYPE_WEP – WEP
//2 : ENC_TYPE_TKIP – WPA / PSK
//4 : ENC_TYPE_CCMP – WPA2 / PSK
//7 : ENC_TYPE_NONE – open network
//8 : ENC_TYPE_AUTO – WPA / WPA2 / PSK
}

/*
 * Written by Ahmad Shamshiri
  * with lots of research, this sources was used:
 * https://support.randomsolutions.nl/827069-Best-dBm-Values-for-Wifi 
 * This is approximate percentage calculation of RSSI
 * Wifi Signal Strength Calculation
 * Written Aug 08, 2019 at 21:45 in Ajax, Ontario, Canada
 */

int dBmtoPercentage(int dBm)
{
  int quality;
    if(dBm <= RSSI_MIN)
    {
        quality = 0;
    }
    else if(dBm >= RSSI_MAX)
    {  
        quality = 100;
    }
    else
    {
        quality = 2 * (dBm + 100);
   }

     return quality;
}//dBmtoPercentage 
