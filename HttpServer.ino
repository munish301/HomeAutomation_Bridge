#ifdef USE_MODBUS_BRIDGE

  void sendElbusData(uint8_t slave_id, uint8_t reg_addr, uint8_t _value){
    serial_elbus_data[0] = slave_id;
    serial_elbus_data[1] = 0x10;
    serial_elbus_data[2] = 0;
    serial_elbus_data[3] = 0x0D;
    serial_elbus_data[4] = 0;
    serial_elbus_data[5] = 2;
    serial_elbus_data[6] = 4;
    serial_elbus_data[7] = 0;
    serial_elbus_data[8] = 1;
    serial_elbus_data[9] = reg_addr; //reg address
    serial_elbus_data[10] = _value;
  
    uint16_t crc_16 = calculate_crc16x(serial_elbus_data, 11);
    serial_elbus_data[11] = (uint8_t)(crc_16 & 0xff);
    serial_elbus_data[12] = (uint8_t)((crc_16 >> 8) & 0xff);
  
    #ifdef IS_DEBUG
      Serial.print("SEND ON MODBUS: ");
      for(int i=0; i<13; i++){
        Serial.print(serial_elbus_data[i], DEC); Serial.print(" ");
      }
      Serial.println(); 
    #endif 
    
    //ESP_ERROR_CHECK(uart_wait_tx_done(NUMERO_PORTA_ELBUS, 100)); //
    uart_write_bytes(NUMERO_PORTA_ELBUS, serial_elbus_data, 13); //
    ESP_ERROR_CHECK(uart_wait_tx_done(NUMERO_PORTA_ELBUS, 100)); //
  }

  
  String handleRoot() {
      int len                                 = 0;
      unsigned long mmillis                   = 0;
      int array_count                         = 0;
      uint16_t crc_16                         = 0x0000;
      String myJson                           = "";
      #ifdef USE_HTTP
        jsonString                              = server.arg("plain");  //to get raw data
      #endif
         
      DynamicJsonDocument doc(5000);
      DeserializationError error = deserializeJson(doc, jsonString);
      digitalWrite(ETHERNET_RX_LED, HIGH);
      
      myJson = "{\"data\":[";
      if (error) {
        #ifdef IS_DEBUG 
          Serial.print(F("Error parsing JSON "));
          Serial.println(error.c_str());
        #endif
        String msg = error.c_str();
    
        myJson += "{";
        myJson += "\"err\":2";
        myJson += ",\"message\":";
        myJson +=  msg;
        myJson += "}";
    
        //server.send(400, F("text/html"), myJson);
      } else {
        JsonObject postObj = doc.as<JsonObject>();
        //if (server.method() == HTTP_POST) {
          if (postObj.containsKey("data")){
            JsonArray array = postObj["data"].as<JsonArray>();
            int array_size = array.size();
            for (JsonVariant v : array) {
              JsonObject object = v.as<JsonObject>();
              uint8_t slave_id = 0;
              uint16_t reg_addr = 0;
              uint16_t _value = 0;
              uint8_t fxn = 0;
              uint16_t did = 0;  
              if(object.containsKey("slave")){
                slave_id  = (uint8_t)object["slave"].as<int>();
                reg_addr = (uint8_t)object["reg"].as<int>();
                _value  = (uint16_t)object["val"].as<int>();
              }else if(object.containsKey("fxn")){
                fxn  = (uint8_t)object["fxn"].as<int>();
                did = (uint16_t)object["did"].as<int>();
                _value  = (uint16_t)object["val"].as<int>();
              }
              #ifdef USE_MODBUS_TO_ELBUS_CONVERTER
                sendElbusData(slave_id, reg_addr, _value);
              #else
                #ifdef USE_MODBUS_LIBRARY
                  modbus.writeSingleHoldingRegister(slave_id, reg_addr, _value);
                #else
                  bytes_modbus[0] = slave_id;
                  if(slave_id >= 15)
                    bytes_modbus[1] = 0x05;
                  else 
                    bytes_modbus[1] = 0x06; 
                  bytes_modbus[2] = 0;
                  bytes_modbus[3] = reg_addr;
                  
                  if(slave_id >= 15){
                    if(_value == 254){
                      bytes_modbus[4] = 0xff;
                      bytes_modbus[5] = 0x00;
                    }else{
                      bytes_modbus[4] = _value;
                      bytes_modbus[5] = 0;                   
                    }
                  }else{
                    bytes_modbus[4] = 0;
                    bytes_modbus[5] = _value;
                  }
     
                  crc_16 = calculateCRC(bytes_modbus, 6);
                  bytes_modbus[7] = (uint8_t)((crc_16 >> 8) & 0xff);                // CRC_H
                  bytes_modbus[6] = (uint8_t)(crc_16 & 0xff);                       // CRC_L
                  //2 6 0 6 0 254 8 67 
                  digitalWrite(PIN_OUT_RS485_EN, HIGH);
                  #ifdef USE_SIMPLE_UART
                    //uart_wait_tx_done(UART_NUM_1,100); 
                    modbus_serial.write(bytes_modbus, 8);    
                    uart_wait_tx_done(UART_NUM_1,100);
                  #else
                    ESP_ERROR_CHECK(uart_wait_tx_done(NUMERO_PORTB_MODBUS, 100));   // esp error check
                    uart_write_bytes(NUMERO_PORTB_MODBUS, bytes_modbus, 8);
                    ESP_ERROR_CHECK(uart_wait_tx_done(NUMERO_PORTB_MODBUS, 100));   // esp error check
                  #endif
                  digitalWrite(PIN_OUT_RS485_EN, LOW);
                #endif
              #endif
              mmillis = millis();
              #ifdef USE_RETRY_METHOD 
                retry_counts = 0;
              #endif
              while (1) {
                #ifdef USE_MODBUS_TO_ELBUS_CONVERTER
                  if (flag_uart_elbus_data) {
                    flag_uart_elbus_data = false;
                    digitalWrite(MODBUS_RX_LED, HIGH);
                    #ifdef IS_DEBUG 
                      Serial.print("MODBUS_RX_LED_LENGTH: "); Serial.println(UART_ELBUS_data_length, DEC);
                      for(int y=0; y<UART_ELBUS_data_length; y++){
                        Serial.println(UART_ELBUS_data[y], DEC);
                      }
                    #endif
                    
                    myJson += "{";
                    if(object.containsKey("slave")){
                      myJson += "\"slave\":";
                      myJson += (uint8_t)slave_id;
                      myJson += ", \"reg\":";
                      myJson += (uint8_t)reg_addr;
                      myJson += ", \"val\":";
                      myJson += (uint8_t)_value;
                    }else{
                      myJson += "\"fxn\":";
                      myJson += (uint8_t)fxn;
                      myJson += ", \"did\":";
                      myJson += did;
                      myJson += ", \"val\":";
                      myJson += (uint8_t)_value;                      
                    }
                    #ifdef USE_DUMMY_DEVICES
                      myJson += ", \"err\":0";
                      delay(10);
                    #else
                      myJson += ", \"err\":1";
                    #endif
                    myJson += "}";
                    digitalWrite(MODBUS_RX_LED, LOW);
                    break;
                  }
                #else
                  #ifdef USE_MODBUS_LIBRARY
                    if (SerialModbus.available()) {
                      len = SerialModbus.readBytes(modbus_data, 8);
                      digitalWrite(MODBUS_RX_LED, HIGH);
                      #ifdef IS_DEBUG 
                        Serial.print("MODBUS_RX_LENGTH: "); Serial.println(len, DEC);
                        for(int y=0; y<len; y++){
                          Serial.println(modbus_data[y], DEC); //
                        }
                      #endif
                      myJson += "{";
                      if(object.containsKey("slave")){
                        myJson += "\"slave\":";
                        myJson += slave_id;
                        myJson += ", \"reg\":";
                        myJson += reg_addr;
                        myJson += ", \"val\":";
                        myJson += _value;
                        myJson += ", \"err\":0";
                      }else if(object.containsKey("fxn")){
                        myJson += "\"fxn\":";
                        myJson += (uint8_t)fxn;
                        myJson += ", \"did\":";
                        myJson += did;
                        myJson += ", \"val\":";
                        myJson += (uint8_t)_value;                      
                      }
                      myJson += "}";
                      digitalWrite(MODBUS_RX_LED, LOW);
                      #ifdef USE_RETRY_METHOD
                        retry_counts = 0;
                      #endif
                      break;
                    }
                  #else
                    if(flag_uart_modbus_data){
                      flag_uart_modbus_data = false;
                      digitalWrite(MODBUS_RX_LED, HIGH);
                      #ifdef IS_DEBUG 
                        Serial.print("MODBUS_RX_LED_LENGTH: "); Serial.println(UART_MODBUS_data_length, DEC);
                        for(int y=0; y<UART_MODBUS_data_length; y++){
                          Serial.println(UART_MODBUS_data[y], DEC);
                        }
                      #endif
                      myJson += "{";
                      if(object.containsKey("slave")){
                        myJson += "\"slave\":";
                        myJson += slave_id;
                        myJson += ", \"reg\":";
                        myJson += reg_addr;
                        myJson += ", \"val\":";
                        myJson += _value;
                      }else if(object.containsKey("fxn")){
                        myJson += "\"fxn\":";
                        myJson += (uint8_t)fxn;
                        myJson += ", \"did\":";
                        myJson += did;
                        myJson += ", \"val\":";
                        myJson += (uint8_t)_value;                      
                      }
                      if(UART_MODBUS_data_length < 8){
                        myJson += ", \"err\":1";
                      }else{
                        myJson += ", \"err\":0";
                      }
                      myJson += "}";
                      digitalWrite(MODBUS_RX_LED, LOW);
                      #ifdef USE_RETRY_METHOD
                        retry_counts = 0;
                      #endif
                      break;  
                    }
                  #endif
                #endif
                if ((millis() - mmillis) >= FRAME_RESPONSE_TIMEOUT) {
                  #ifdef USE_RETRY_METHOD 
                  if(retry_counts++ >= TOTAL_RETRY_COUNTS){
                  #endif  
                    myJson += "{";
                    if(object.containsKey("slave")){
                      myJson += "\"slave\":";
                      myJson += slave_id;
                      myJson += ", \"reg\":";
                      myJson += reg_addr;
                      myJson += ", \"val\":";
                      myJson += _value;
                    }else if(object.containsKey("fxn")){
                      myJson += "\"fxn\":";
                      myJson += (uint8_t)fxn;
                      myJson += ", \"did\":";
                      myJson += did;
                      myJson += ", \"val\":";
                      myJson += (uint8_t)_value;                      
                    }
                    #ifdef USE_DUMMY_DEVICES
                    myJson += ", \"err\":0";
                    delay(10);
                    #else
                    myJson += ", \"err\":1";
                    #endif
                    myJson += "}";
                    digitalWrite(MODBUS_RX_LED, LOW);     
                    flag_uart_modbus_data = false;
                    flag_uart_elbus_data = false;     
                    break;
                  #ifdef USE_RETRY_METHOD   
                    }else{
                      #ifdef USE_MODBUS_TO_ELBUS_CONVERTER
                        sendElbusData(slave_id, reg_addr, _value);
                      #else 
                        #ifdef USE_MODBUS_LIBRARY
                          modbus.writeSingleHoldingRegister(slave_id, reg_addr, _value);
                        #else               
                          digitalWrite(PIN_OUT_RS485_EN, HIGH);
                          ESP_ERROR_CHECK(uart_wait_tx_done(NUMERO_PORTB_MODBUS, 100));  
                          uart_write_bytes(NUMERO_PORTB_MODBUS, bytes_modbus, 8);
                          ESP_ERROR_CHECK(uart_wait_tx_done(NUMERO_PORTB_MODBUS, 100)); 
                          digitalWrite(PIN_OUT_RS485_EN, LOW); 
                          mmillis = millis();
                        #endif
                      #endif               
                    }
                  #endif
                }
              } //while
              if (array_size > 1) {
                if (array_count < array_size - 1) {
                  myJson += ",";
                }
              }
              array_count++;
            } //for()
          } else {
            myJson += "{";
            myJson += "\"err\":3";
            myJson += ",\"message\":\"json data key not found\"";
            myJson += "}";
            #ifdef IS_DEBUG 
              Serial.print(F("NO DATA KEY"));
            #endif
          }
      }
      myJson += "]}";
      #ifdef IS_DEBUG 
        Serial.print("SENDING HTTP DATA: ");
        Serial.println(myJson);
        Serial.println("*************************************************************");  //
      #endif
      digitalWrite(ETHERNET_RX_LED, LOW);
      #ifdef USE_HTTP
        server.send(200, F("text/html"), myJson);
      #endif
      return myJson;
    }
    
  #ifdef USE_HTTP
    void handleNotFound() {
      String message = F("File Not Found\n\n");
    
      message += F("URI: ");
      message += server.uri();
      message += F("\nMethod: ");
      message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
      message += F("\nArguments: ");
      message += server.args();
      message += F("\n");
    
      for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
      }
      server.send(404, F("text/plain"), message);
    }
    
    void initHttpServer() {
      server.on(F("/"), HTTP_ANY, handleRoot);
      server.on(F("/inline"), []() {
        server.send(200, F("text/plain"), F("SERVER START"));
      });
    
      server.onNotFound(handleNotFound);
      server.begin();
    } 
  #endif
#endif
