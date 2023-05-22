#ifdef USE_ELBUS_HMI_BRIDGE

void onEventsCallback(WebsocketsEvent event, String _data) {
  (void) _data;

  if (event == WebsocketsEvent::ConnectionOpened){
    #ifdef IS_DEBUG 
      Serial.println("Connnection Opened");
    #endif
  }else if (event == WebsocketsEvent::ConnectionClosed){
    #ifdef IS_DEBUG 
      Serial.println("Connnection Closed");
    #endif
    ESP.restart();
  }else if (event == WebsocketsEvent::GotPing){
    #ifdef IS_DEBUG 
      Serial.println("Got a Ping!");
    #endif
  }else if (event == WebsocketsEvent::GotPong){
    #ifdef IS_DEBUG 
      Serial.println("Got a Pong!");
    #endif
  } 
}

void send_data_to_elbus(uint8_t* array_data, int _length){
  //ESP_ERROR_CHECK(uart_wait_tx_done(NUMERO_PORTA_ELBUS, 100));
  uart_write_bytes(NUMERO_PORTA_ELBUS, array_data, _length);
  ESP_ERROR_CHECK(uart_wait_tx_done(NUMERO_PORTA_ELBUS, 100));
  #ifdef IS_DEBUG
    Serial.print("SENDING DEVICES ON HMI: ");
    for(int lmn = 0; lmn<_length; lmn++){   
      Serial.print(array_data[lmn], DEC);
      Serial.print(" "); 
    }
    Serial.println();
  #endif
}

void socketJsonReceived(String jsonStr){ 
    #define FRAME_GAP_TIME_MS             10
    
    int fxn_code                          = 0;
    uint8_t val8                          = 0x00;
    uint8_t color                         = 0x00;
    uint8_t err                           = 0x00;
    uint16_t crc_16                       = 0x0000;
    int index                             = 0;
    int d_size_counter                    = 0; 
    unsigned char OFFSET                  = 0;
    unsigned int devId                    = 0;
    String _data                          = "";
    
    index_devices_counter                 = 0x00;
    index_location_counter                = 0x00;
    index_scene_counter                   = 0x00;
    index_set_time_counter                = 0x00;
    index_set_backlight_counter           = 0x00;
    index_set_curtain_counter             = 0x00;
    index_set_ac_counter                  = 0x00;
   
    StaticJsonDocument<5000> root;
    DeserializationError error            = deserializeJson(root, jsonStr);
    
    JsonArray array;
    JsonObject object;
    
    digitalWrite(LED_NW_RX, HIGH); //
    #ifdef IS_DEBUG 
      Serial.println("GETTING SOCKET DATA"); 
    #endif
    if (error) {
      #ifdef IS_DEBUG
        Serial.println("JSON error!!");
        Serial.print(F("deserializeJson() failed: "));
        // Serial.println(error.f_str());   //
      #endif
    } else {
      if (root.containsKey("data")) {
        array = root["data"].as<JsonArray>();
        for(JsonVariant v : array) {
          object = v.as<JsonObject>();
          fxn_code = (uint8_t)object["fxn"].as<int>();
          if((fxn_code < 6)){
            //set device
            if(d_size_counter == 0){
              socket_data[FRAME_INDEX_DEVICE][0]                                      = 255; //broadcast
              index_devices_counter                                                   = 1;
            }
            d_size_counter++;
            OFFSET                                                                    = 5;
            fxn_code                                                                  = object["fxn"].as<int>();
            if(fxn_code < 4){
              devId = object["did"].as<int>();
              socket_data[FRAME_INDEX_DEVICE][1 + OFFSET*index]                       = (uint8_t) fxn_code;
              socket_data[FRAME_INDEX_DEVICE][2 + OFFSET*index]                       = (uint8_t)((devId >> 8) & 0xff); 
              socket_data[FRAME_INDEX_DEVICE][3 + OFFSET*index]                       = (uint8_t)(devId & 0xff); 
              socket_data[FRAME_INDEX_DEVICE][4 + OFFSET*index]                       = (uint8_t)(object["sta"].as<int>() & 0xff);
              if(object["val"] != NULL){
                val8                                                                  = (uint8_t)object["val"].as<int>(); //
              }else{
                val8                                                                  = 0;
              }
              socket_data[FRAME_INDEX_DEVICE][5 + OFFSET*index]                       = (uint8_t)(val8 & 0xff);                    
            }else if(fxn_code == 4){
              socket_data[FRAME_INDEX_DEVICE][1 + OFFSET*index]                       = (uint8_t) fxn_code;                       
              socket_data[FRAME_INDEX_DEVICE][2 + OFFSET*index]                       = (uint8_t)(object["lid"].as<int>()); 
              socket_data[FRAME_INDEX_DEVICE][3 + OFFSET*index]                       = (uint8_t)(object["sta"].as<int>());
              socket_data[FRAME_INDEX_DEVICE][4 + OFFSET*index]                       = 0;
              socket_data[FRAME_INDEX_DEVICE][5 + OFFSET*index]                       = 0;                       
            }else if(fxn_code == 5){
              socket_data[FRAME_INDEX_DEVICE][1 + OFFSET*index]                       = (uint8_t) fxn_code;
              if (object.containsKey("lid")) {
                socket_data[FRAME_INDEX_DEVICE][2 + OFFSET*index]                     = (uint8_t)(object["lid"].as<int>());
              }else{
                socket_data[FRAME_INDEX_DEVICE][2 + OFFSET*index]                     = 255;                 
              }
              socket_data[FRAME_INDEX_DEVICE][3 + OFFSET*index]                       = (uint8_t)(object["sid"].as<int>()); 
              socket_data[FRAME_INDEX_DEVICE][4 + OFFSET*index]                       = 0;
              socket_data[FRAME_INDEX_DEVICE][5 + OFFSET*index]                       = 0;                                     
            } 
            index_devices_counter                                                    += OFFSET;  // set_offset
            index++;                                                        
          }else if(fxn_code == 6){            // set_time
            //set time
            socket_data[FRAME_INDEX_SET_TIME][index_set_time_counter++]               = 255;                //broadcast
            socket_data[FRAME_INDEX_SET_TIME][index_set_time_counter++]               = fxn_code;           //fxn   
            socket_data[FRAME_INDEX_SET_TIME][index_set_time_counter++]               = (uint8_t)object["hour"].as<int>();
            socket_data[FRAME_INDEX_SET_TIME][index_set_time_counter++]               = (uint8_t)object["min"].as<int>();
            socket_data[FRAME_INDEX_SET_TIME][index_set_time_counter++]               = (uint8_t)object["sec"].as<int>();
            socket_data[FRAME_INDEX_SET_TIME][index_set_time_counter++]               = (uint8_t)object["wday"].as<int>();
            socket_data[FRAME_INDEX_SET_TIME][index_set_time_counter++]               = (uint8_t)object["date"].as<int>();
            socket_data[FRAME_INDEX_SET_TIME][index_set_time_counter++]               = (uint8_t)object["mon"].as<int>();
            socket_data[FRAME_INDEX_SET_TIME][index_set_time_counter++]               = (uint8_t)object["year"].as<int>();                         
          }else if(fxn_code == 8){ //set backlight intensity
            //set backlight intensity
            socket_data[FRAME_INDEX_BACKLIGHT][index_set_backlight_counter++]         = 255;          //broadcast
            socket_data[FRAME_INDEX_BACKLIGHT][index_set_backlight_counter++]         = fxn_code;     //fxn code    
            if (object.containsKey("lid")) {
              socket_data[FRAME_INDEX_BACKLIGHT][index_set_backlight_counter++]       = (uint8_t)(object["lid"].as<int>());
            }else{
              socket_data[FRAME_INDEX_BACKLIGHT][index_set_backlight_counter++]       = 255;                 
            }
            socket_data[FRAME_INDEX_BACKLIGHT][index_set_backlight_counter++]         = (uint8_t)(object["val"].as<int>());                                     
          }else if(fxn_code == 11){                                                           //set curtain
            //set backlight intensity
            socket_data[FRAME_INDEX_CURTAIN][index_set_curtain_counter++]             = 255;              //broadcast
            socket_data[FRAME_INDEX_CURTAIN][index_set_curtain_counter++]             = fxn_code;         //fxn code    
            if (object.containsKey("lid")) {
              socket_data[FRAME_INDEX_CURTAIN][index_set_curtain_counter++]           = (uint8_t)(object["lid"].as<int>());
            }else{
              socket_data[FRAME_INDEX_CURTAIN][index_set_curtain_counter++]           = 255;                 
            }
            devId = object["did"].as<int>();
            socket_data[FRAME_INDEX_CURTAIN][index_set_curtain_counter++]             = (uint8_t)((devId >> 8) & 0xff); 
            socket_data[FRAME_INDEX_CURTAIN][index_set_curtain_counter++]             = (uint8_t)(devId & 0xff);                   
            socket_data[FRAME_INDEX_CURTAIN][index_set_curtain_counter++]             = (uint8_t)(object["val"].as<int>());                  
          }else if(fxn_code == 14){ //set AC
            //set backlight intensity
            socket_data[FRAME_INDEX_AC][index_set_ac_counter++]                       = 255;      //broadcast
            socket_data[FRAME_INDEX_AC][index_set_ac_counter++]                       = fxn_code; //fxn code    
            if (object.containsKey("lid")) {
              socket_data[FRAME_INDEX_AC][index_set_ac_counter++]                     = (uint8_t)(object["lid"].as<int>());
            }else{
              socket_data[FRAME_INDEX_AC][index_set_ac_counter++]                     = 255;                 
            }
            devId = object["ac_id"].as<int>();
            socket_data[FRAME_INDEX_AC][index_set_ac_counter++]                       = (uint8_t)(devId & 0xff); 
            if (object.containsKey("sta")) {                   
              socket_data[FRAME_INDEX_AC][index_set_ac_counter++]                     = (uint8_t)(object["sta"].as<int>());
            }
            uint16_t temp = 0xFF;
            if (object.containsKey("temp")) { 
              temp = (uint16_t)(object["temp"].as<int>()); 
              if(temp > 35){
                temp /= 10;
              } 
            }
            socket_data[FRAME_INDEX_AC][index_set_ac_counter++]                     = (uint8_t)temp;
//            if (object.containsKey("mode")) { 
//              socket_data[FRAME_INDEX_AC][index_set_ac_counter++]                     = (uint8_t)(object["mode"].as<int>());
//            }
//            if (object.containsKey("swing")) { 
//              socket_data[FRAME_INDEX_AC][index_set_ac_counter++]                     = (uint8_t)(object["swing"].as<int>());
//            }
//            if (object.containsKey("fan")) { 
//              socket_data[FRAME_INDEX_AC][index_set_ac_counter++]                     = (uint8_t)(object["fan"].as<int>());
//            }                                                       
          }
       }
       //////////////////////////////////////////////////////////////////////////////////////////////////
       if(index_devices_counter > 0){ 
          crc_16 = calculate_crc16x(socket_data[FRAME_INDEX_DEVICE], index_devices_counter); 
          socket_data[FRAME_INDEX_DEVICE][index_devices_counter++]                    = (uint8_t)(crc_16 & 0xff);
          socket_data[FRAME_INDEX_DEVICE][index_devices_counter++]                    = (uint8_t)((crc_16 >> 8) & 0xff); 
          send_data_to_elbus(socket_data[FRAME_INDEX_DEVICE], index_devices_counter); 
          d_size_counter = 0; index_devices_counter = 0; index = 0;  
       }
       if(index_curtain_counter > 0){
          delay(FRAME_GAP_TIME_MS);
          crc_16 = calculate_crc16x(socket_data[FRAME_INDEX_CURTAIN], index_curtain_counter); 
          socket_data[FRAME_INDEX_CURTAIN][index_curtain_counter++]                   = (uint8_t)(crc_16 & 0xff);
          socket_data[FRAME_INDEX_CURTAIN][index_curtain_counter++]                   = (uint8_t)((crc_16 >> 8) & 0xff);             
          send_data_to_elbus(socket_data[FRAME_INDEX_CURTAIN], index_curtain_counter);            
       }
       if(index_set_ac_counter > 0){
          delay(FRAME_GAP_TIME_MS);
          crc_16 = calculate_crc16x(socket_data[FRAME_INDEX_AC], index_set_ac_counter); 
          socket_data[FRAME_INDEX_AC][index_set_ac_counter++]                         = (uint8_t)(crc_16 & 0xff);
          socket_data[FRAME_INDEX_AC][index_set_ac_counter++]                         = (uint8_t)((crc_16 >> 8) & 0xff);           
          send_data_to_elbus(socket_data[FRAME_INDEX_AC], index_set_ac_counter);            
       }
       //////////////////////////////////////////////////////////////////////////////////////////////////
       if(index_set_time_counter > 0){
          delay(FRAME_GAP_TIME_MS); 
          crc_16 = calculate_crc16x(socket_data[FRAME_INDEX_SET_TIME], index_set_time_counter); 
          socket_data[FRAME_INDEX_SET_TIME][index_set_time_counter++]                 = (uint8_t)(crc_16 & 0xff);
          socket_data[FRAME_INDEX_SET_TIME][index_set_time_counter++]                 = (uint8_t)((crc_16 >> 8) & 0xff);
          send_data_to_elbus(socket_data[FRAME_INDEX_SET_TIME], index_set_time_counter);               
       }  
       if(index_set_backlight_counter > 0){
          crc_16 = calculate_crc16x(socket_data[FRAME_INDEX_BACKLIGHT], index_set_backlight_counter); 
          socket_data[FRAME_INDEX_BACKLIGHT][index_set_backlight_counter++]           = (uint8_t)(crc_16 & 0xff);
          socket_data[FRAME_INDEX_BACKLIGHT][index_set_backlight_counter++]           = (uint8_t)((crc_16 >> 8) & 0xff);
          send_data_to_elbus(socket_data[FRAME_INDEX_BACKLIGHT], index_set_backlight_counter);              
       }          
      }else{
        #ifdef IS_DEBUG
          Serial.println("Contains No Data!!");
        #endif          
      }                                                   
    }
    digitalWrite(LED_NW_RX, LOW);
 }

 void initSocketClient() {
    bool toggle_leds = false;
    //get chip id
    mychip_id = String((uint32_t)ESP.getEfuseMac(), HEX); //
    mychip_id.toUpperCase();
  
    // run callback when messages are received
    client.onMessage([&](WebsocketsMessage message) {
      #ifdef IS_DEBUG 
        Serial.print("Got Socket Message: ");
        Serial.println(message.data());
      #endif
      socketJsonReceived(message.data());
    });

    // run callback when events are occuring
    client.onEvent(onEventsCallback);
    while(1){
      String wsocket_ip = "192.168.";
      wsocket_ip += IP_3;
      wsocket_ip += ".230";
      
      bool connected = client.connect(wsocket_ip, 3031, "/");
      if (connected){
        #ifdef IS_DEBUG 
          Serial.println("Connected!!");
        #endif
        digitalWrite(LED_HB, LOW);  
        digitalWrite(LED_NW_TX, LOW);      
        digitalWrite(LED_NW_RX, LOW);
        break;
      }else{
        digitalWrite(LED_HB, !digitalRead(LED_HB));
        delay(2000); 
        #ifdef IS_DEBUG 
          Serial.println("Not Connected!!"); //
        #endif
        //ESP.restart();
      }
    }  
 }


#endif
