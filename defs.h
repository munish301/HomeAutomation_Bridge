  
  #define SUNIL_SIR_HOME_LUDHIANA     1
  #define RAJKUMAR_HOME_LUDHIANA      2
  #define MODELTOWN_LUDHIANA          3
  #define PP176_MOHALI                4
  #define COMPAC_MOHALI               5
  #define BATHINDA                    6
  #define VIMALDEEP_LUDHIANA          7
  #define ANDROID_TESTER              8
  
//  #define CUSTOMER          SUNIL_SIR_HOME_LUDHIANA
//  #define CUSTOMER          RAJKUMAR_HOME_LUDHIANA
//  #define CUSTOMER          MODELTOWN_LUDHIANA
//  #define CUSTOMER          PP176_MOHALI
//  #define CUSTOMER          COMPAC_MOHALI
//  #define CUSTOMER          BATHINDA
  #define CUSTOMER          VIMALDEEP_LUDHIANA
//  #define USE_SMALL_BLACK_BOX_PCB
  
  #if(CUSTOMER == COMPAC_MOHALI)
    #define IP_3            29
    #ifdef USE_SMALL_BLACK_BOX_PCB              //HMI WIFI BRIDGE
        #define USE_WIFI
        #define USE_ELBUS_HMI_BRIDGE 
        //#define USE_MODBUS_BRIDGE 
        //#define USE_MODBUS_TO_ELBUS_CONVERTER 
        //#define USE_DUMMY_DEVICES      
    #else                                       //USE BOTH HMI+MODBUS BRIDGE
        //#define USE_ELBUS_HMI_BRIDGE 
        #define USE_MODBUS_BRIDGE 
        #define USE_MODBUS_TO_ELBUS_CONVERTER 
        #define USE_DUMMY_DEVICES
    #endif
    const char* wifiSSID    = "compac322";
    const char* wifiPSWD    = "afbuteba@322";
  #elif(CUSTOMER == ANDROID_TESTER)
    #define IP_3            1
    #define USE_WIFI
    #define USE_ELBUS_HMI_BRIDGE 
    #define USE_MODBUS_BRIDGE 
    #define USE_MODBUS_TO_ELBUS_CONVERTER 
    #define USE_DUMMY_DEVICES    
    const char* wifiSSID    = "Cracked";
    const char* wifiPSWD    = "Mohit@0513"; 
//    const char* wifiSSID    = "compac322";
//    const char* wifiPSWD    = "afbuteba@322";
  #elif(CUSTOMER == PP176_MOHALI)
    #define IP_3            1
    #define USE_WIFI
    #define USE_SMALL_BLACK_BOX_PCB
    #define USE_MODBUS_TO_ELBUS_CONVERTER
    const char* wifiSSID    = "ElkraftHa";
    const char* wifiPSWD    = "afbuteba@409";  
  #elif(CUSTOMER == MODELTOWN_LUDHIANA)         
    #define IP_3            1
    //#define USE_WIFI
    #define USE_ELBUS_HMI_BRIDGE 
    #define USE_MODBUS_BRIDGE 
    const char* wifiSSID    = "Deco";
    const char* wifiPSWD    = "Ansh@40044"; 
  #elif(CUSTOMER == RAJKUMAR_HOME_LUDHIANA)
    #define IP_3            1
    #define USE_WIFI
    const char* wifiSSID    = "Raj amar";
    const char* wifiPSWD    = "raj@1969";
    #define USE_MODBUS_TO_ELBUS_CONVERTER  
  #elif(CUSTOMER == BATHINDA)
    #define IP_3            1
//    #define USE_WIFI
//    #define USE_MODBUS_TO_ELBUS_CONVERTER 
    #ifdef USE_SMALL_BLACK_BOX_PCB              //HMI WIFI BRIDGE
        #define USE_WIFI
        #define USE_ELBUS_HMI_BRIDGE 
        //#define USE_MODBUS_BRIDGE 
        //#define USE_MODBUS_TO_ELBUS_CONVERTER 
        //#define USE_DUMMY_DEVICES      
    #else                                       //USE BOTH HMI+MODBUS BRIDGE
        //#define USE_ELBUS_HMI_BRIDGE 
        #define USE_MODBUS_BRIDGE 
        #define USE_MODBUS_TO_ELBUS_CONVERTER 
        #define USE_DUMMY_DEVICES
    #endif  
    const char* wifiSSID    = "Sidhuz";
    const char* wifiPSWD    = "kimsimgavin77"; 
  #elif(CUSTOMER == SUNIL_SIR_HOME_LUDHIANA)
    #define IP_3            1
    #define USE_WIFI
    #define USE_MODBUS_TO_ELBUS_CONVERTER 
    //#define USE_BOTH
    const char* wifiSSID    = "Airel_9815510062";
    const char* wifiPSWD    = "air14405"; 
  #elif(CUSTOMER == VIMALDEEP_LUDHIANA)
    #define IP_3            29
    #define USE_ELBUS_HMI_BRIDGE 
    #define USE_MODBUS_BRIDGE 
    #define USE_MODBUS_TO_ELBUS_CONVERTER 
    #define USE_DUMMY_DEVICES 
    const char* wifiSSID    = "compac322";
    const char* wifiPSWD    = "afbuteba@322";           
  #endif
  
  #ifdef USE_MODBUS_BRIDGE
    //#define USE_HTTP
    #define USE_SOCKET_SERVER
    #include <WebServer_WT32_ETH01.h>
  #endif
  
  String jsonString         = "";
  
  IPAddress                 myIP(192, 168, IP_3, 200);
  IPAddress                 myGW(192, 168, IP_3, 1);
  IPAddress                 mySN(255, 255, 255, 0);
  IPAddress                 myDNS(8, 8, 8, 8);
 
  #ifdef USE_HTTP   
    #ifdef USE_MODBUS_BRIDGE    
      WebServer                 server(8081);              //
    #endif
  #endif                                                   //
  #ifdef USE_SOCKET_SERVER   
     #include <ArduinoWebsockets.h>
     using namespace websockets;
     WebsocketsServer server;
     #ifdef USE_ELBUS_HMI_BRIDGE
      WebsocketsClient client;
     #endif
  #endif                                                   //

//  #ifdef USE_ELBUS_HMI_BRIDGE
//    #include <WebSockets2_Generic.h>
//    using namespace websockets2_generic;
//    WebsocketsClient client;
//  #endif
  //#define USE_SPIFF
  //#define USE_RETRY_METHOD
  //#define USE_MODBUS_LIBRARY
  //#define USE_SIMPLE_UART
  //#define USE_SPIFFS
  #define IS_DCB1M
  #define IS_DEBUG
  //////////////////////////////////////////////////////////////////
  #define MAX_RX_DATA                               1000
  #define DCB1M_BAUD                                230400 //115200
  #define RS485_BAUD                                19200
  
  // declare reset fuction at address 0
  void(* resetFunc) (void)                          = 0;
  
  char server_ip[20];
  uint16_t server_port; 

  #ifdef USE_SMALL_BLACK_BOX_PCB
    const int LED_HB                                = 14;
    const int LED_NW_RX                             = 27;
    const int LED_NW_TX                             = 33;
    const int MODBUS_RX_LED                         = 33;
    const int ETHERNET_RX_LED                       = 27;
    const int PIN_OUT_DCB1M_DC                      = 26;
    const int PIN_OUT_RS485_EN                      = 26;
    const int PIN_ELBUS_RXD                         = 15;
    const int PIN_ELBUS_TXD                         = 13; 
  #else
    const int LED_HB                                = 5;
    const int MODBUS_RX_LED                         = 4;
    const int LED_NW_TX                             = 4;
    const int LED_NW_RX                             = 16;
    const int ETHERNET_RX_LED                       = 16;
    const int PIN_OUT_DCB1M_DC                      = 33;
    const int PIN_OUT_SIG60_DC                      = 33;
    const int PIN_OUT_RS485_EN                      = 32;
    const int PIN_ELBUS_RXD                         = 14;
    const int PIN_ELBUS_TXD                         = 12; 
    const int MODBUS_RXD                            = 15;
    const int MODBUS_TXD                            = 13;
  #endif
   
  bool flag_uart_modbus_data                      = false;
  bool flag_uart_elbus_data                       = false;
  
  uint8_t UART_MODBUS_data[128];
  int UART_MODBUS_data_length                     = 0;
  
  uint8_t UART_ELBUS_data[1000];
  int UART_ELBUS_data_length                      = 0;
  
  String mychip_id                                = "";
  ////////////////////////////////////////////////////
  uint8_t index_curtain_counter                   = 0;
  uint8_t index_set_ac_counter                    = 0;
  
  #ifdef USE_RETRY_METHOD
    unsigned char retry_counts                    = 0;
    #define TOTAL_RETRY_COUNTS                    2 
  #endif
  
  #define FRAME_INDEX_LOCATION                    0
  #define FRAME_INDEX_SCENE                       1
  #define FRAME_INDEX_SET_TIME                    2
  #define FRAME_INDEX_BACKLIGHT                   3
  #define FRAME_INDEX_CURTAIN                     4
  #define FRAME_INDEX_AC                          5
  #define FRAME_INDEX_DEVICE                      6                        
  
  uint8_t socket_data[8][250];
  unsigned int elbus_data_len                     = 0;
  uint8_t serial_elbus_data[15]                   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  unsigned long hb_interval                       = 500;
  unsigned long current_millis                    = 0;
  unsigned long hb_previousMillis                 = 0;

  #define FRAME_RESPONSE_TIMEOUT                  30
  
  uint8_t u8_ip[4]                                = {0,0,0,0};
  unsigned char modbus_data[20]                   = {0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0};                                                                                                   
  uint8_t bytes_modbus[8]                         = {0,0,0,0,0,0,0,0};
  #define NUMERO_PORTA_ELBUS                      UART_NUM_2

  #ifdef USE_MODBUS_BRIDGE
    #ifdef USE_MODBUS_LIBRARY 
      HardwareSerial                              SerialModbus(1);  
      esp32ModbusRTU                              modbus(&SerialModbus, PIN_OUT_RS485_EN);  // use Serial1 and pin PIN_OUT_RS485_EN as RTS
    #else
      #ifdef USE_SIMPLE_UART
        HardwareSerial                            modbus_serial(1);
      #else
        #define NUMERO_PORTB_MODBUS               UART_NUM_1
      #endif   
    #endif
  #endif
  uint16_t index_devices_counter                  = 0x00;
  uint8_t index_location_counter                  = 0x00;
  uint8_t index_scene_counter                     = 0x00;
  uint8_t index_set_time_counter                  = 0x00;
  uint8_t index_set_backlight_counter             = 0x00;
  uint16_t index_set_curtain_counter              = 0x00;
