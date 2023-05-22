
#define DEBUG_ETHERNET_WEBSERVER_PORT           Serial
// Debug Level from 0 to 4
#define _ETHERNET_WEBSERVER_LOGLEVEL_           3
#include <Arduino.h>
#include <HardwareSerial.h>
#include <SPIFFS.h>
#include <StringTokenizer.h>
//#include <esp32ModbusRTU.h>
//#include <algorithm>
// Load Wi-Fi library
#include <WiFi.h>
#include "driver/uart.h"
#include "defs.h"
#include <ArduinoJson.h>  
#include <ArduinoUnit.h>

#ifdef USE_WIFI
  void  Set_Static_IP(){
    //Set your Static IP Address
    if(!WiFi.config(myIP, myGW, mySN)) {
      Serial.println("STA Failed To Configure!!");  //
    }
  }
  
  void  Connect_to_WiFi(){
    int retry = 0; 
    #ifdef IS_DEBUG
      Serial.println("Disconnected from WiFi access point");
    #endif
    WiFi.disconnect();
    digitalWrite(LED_HB, HIGH); 
    //Try to Connect with  Router SSID and Password
    #ifdef IS_DEBUG
      Serial.println("");
      Serial.println("Connecting with Router SSID");
    #endif 
    WiFi.begin(wifiSSID, wifiPSWD);
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(200); //200 msec
      digitalWrite(LED_HB, !digitalRead(LED_HB));
      Serial.print(".");
      if(retry++ >= 300){   // 1 Minute wait for the connection
        retry = 0; ESP.restart();
      }
    } 
    #ifdef IS_DEBUG
        Serial.println("");
        Serial.print("Connected with ");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP()); //     
    #endif
    digitalWrite(LED_HB, LOW);    
    digitalWrite(LED_NW_RX, LOW);
  }
#endif

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(LED_HB, OUTPUT);
  pinMode(PIN_OUT_RS485_EN, OUTPUT);
  pinMode(ETHERNET_RX_LED, OUTPUT);
  pinMode(MODBUS_RX_LED, OUTPUT);

  #ifndef USE_WIFI
    pinMode(17, OUTPUT);
    digitalWrite(17, HIGH);
  #endif
  
  digitalWrite(LED_HB, LOW);
  digitalWrite(ETHERNET_RX_LED, LOW);
  digitalWrite(MODBUS_RX_LED, LOW);
  digitalWrite(PIN_OUT_RS485_EN, LOW);
  
  #ifdef USE_SPIFFS
    initFlashMemory();
  #endif

  #ifdef USE_WIFI
    WiFi.mode(WIFI_STA);   //Set Device Wi-Fi Mode
    #ifdef USE_MODBUS_BRIDGE
    Set_Static_IP();
    #endif
    Connect_to_WiFi();
  #else
    // To be called before ETH.begin()
    WT32_ETH01_onEvent();
    ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);
    ETH.config(myIP, myGW, mySN, myDNS);
    WT32_ETH01_waitForConnect();
  
    //String my_ip = ETH.localIP();
    //my_ip.toCharArray(server_ip, my_ip.length()+1); 
    #ifdef IS_DEBUG
      Serial.print(F("HTTP EthernetWebServer is @ IP : "));
      Serial.println(ETH.localIP());
    #endif
  #endif
 
  #ifdef IS_DEBUG 
    #ifdef USE_SOCKET_SERVER
    Serial.print("Is server live? ");
    Serial.println(server.available()); 
    #endif       
  #endif
  //Init Http Websserver for Modbus Devices
  #ifdef USE_MODBUS_BRIDGE
    #ifdef USE_HTTP
      initHttpServer();
    #endif
    #ifdef USE_SOCKET_SERVER    
        Create_Server();
    #endif     
  #endif

  //Init Socket Client for HMIs Display
  #ifdef USE_ELBUS_HMI_BRIDGE
    initSocketClient();
  #endif
  initElbusUart();

  #ifdef USE_MODBUS_BRIDGE
    #ifndef USE_MODBUS_TO_ELBUS_CONVERTER
      //Init Modbus RS485
      #ifdef USE_MODBUS_LIBRARY
        SerialModbus.begin(RS485_BAUD, SERIAL_8N1, MODBUS_RXD, MODBUS_TXD);  //15-RXD, 13-TXD
        SerialModbus.setTimeout(20);
        modbus.begin();
      #else
        #ifdef USE_SIMPLE_UART
           modbus_serial.begin(RS485_BAUD, SERIAL_8N1, MODBUS_RXD, MODBUS_TXD); // MODBUS_BAUDRATE
           modbus_serial.setRxTimeout(3);// Symbols   
           modbus_serial.onReceive(modbus_onReceive, 0);
        #else
           initModbusUart();  
        #endif
      #endif
    #endif
  #endif
  //init elbus dcb1m module on uart
  setup_dcb1m();
  current_millis = millis();
  hb_previousMillis = current_millis;
}

void loop() {
  ////////////////////////////////////////////////////////////////////
  //toogle LED heart beat
  current_millis = millis();
  if (current_millis - hb_previousMillis >= hb_interval) {
    hb_previousMillis = millis();
    digitalWrite(LED_HB, !digitalRead(LED_HB));
    #ifdef USE_WIFI
    if(WiFi.status() != WL_CONNECTED) { Connect_to_WiFi(); }
    #endif    
  }
  ////////////////////////////////////////////////////////////////////
  //run http server on port 8081
  #ifdef USE_MODBUS_BRIDGE
    #ifdef USE_HTTP
      server.handleClient();
    #endif
  #endif
  #ifdef USE_ELBUS_HMI_BRIDGE
    serialElbusReceiver();
    //run web socket client on port 3031
    if (client.available()) {
      client.poll();
    }     
  #endif
  Test::run();
}
