
#ifdef IS_DCB1M

  //uint8_t reg0_array[3] = {0xF5, 0x00, 0x48};
  uint8_t reg0_array[3] = {0xF5, 0x00, 0x4A};
  uint8_t reg1_array[3] = {0xF5, 0x01, 0xF9};
  uint8_t reg2_array[3] = {0xF5, 0x02, 0x05};

  uint8_t fvalue        = 3;
  uint8_t fsta          = 0;
  uint8_t acvalue       = 25;
  uint8_t acsta         = 0;
  
  void writeDCB1MReg(uint8_t* arrays) {
    digitalWrite(PIN_OUT_DCB1M_DC, LOW);
    delayMicroseconds(100);
    uart_write_bytes(NUMERO_PORTA_ELBUS, arrays, 3);
    ESP_ERROR_CHECK(uart_wait_tx_done(NUMERO_PORTA_ELBUS, 100));
    delayMicroseconds(100);
    digitalWrite(PIN_OUT_DCB1M_DC, HIGH); //
  }//end void initDCB1M()

  void writeDCB1MBaudrateReg(){
    uint8_t _array[1] = {0xF5};
    digitalWrite(PIN_OUT_DCB1M_DC, LOW);
    delayMicroseconds(100);
    uart_write_bytes(NUMERO_PORTA_ELBUS, _array, 1);
    ESP_ERROR_CHECK(uart_wait_tx_done(NUMERO_PORTA_ELBUS, 100));
    delayMicroseconds(100);
    digitalWrite(PIN_OUT_DCB1M_DC, HIGH); //
  }//end void initDCB1M()


  ////////////////////////////////////////////////////////////////////////
  void setup_dcb1m() {
    // put your setup code here, to run once:
    //SerialElbus.begin(DCB1M_BAUD, SERIAL_8N1, PIN_ELBUS_RXD, PIN_ELBUS_TXD);  //14-RXD, 12-TXD
    /*
      Serial.setTimeout() sets the maximum milliseconds to wait for serial data when using 
      Serial.readBytesUntil(), Serial.readBytes(), Serial.parseInt() or Serial.parseFloat(). 
      It defaults to 1000 milliseconds.
    */
    pinMode(PIN_OUT_DCB1M_DC, OUTPUT);                  //ISG60 D/C
    digitalWrite(PIN_OUT_DCB1M_DC, HIGH);               //HIGH = data, LOW = command
    delay(500);
    writeDCB1MBaudrateReg();
    delay(50);
    writeDCB1MReg(reg0_array);
    delay(50);
    writeDCB1MReg(reg1_array);
    delay(50);
    #ifdef USE_ELBUS_HMI_BRIDGE
      writeDCB1MReg(reg2_array);                        //REG_2 = 0x05 for freq = 5.5 MHz
    #endif
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void makeJsonSetDeviceToggle(unsigned char fxn, unsigned int dev_id){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":";
     strJson += fxn;
     strJson += ",\"did\":";
     strJson += dev_id;
     strJson += "}]}";
     #ifdef IS_DEBUG
        Serial.println("strJsonToggle: "+strJson);
     #endif
     #ifdef USE_ELBUS_HMI_BRIDGE
       client.send(strJson); 
     #endif 
  }
  void makeJsonSetDeviceStatusOnly(unsigned char fxn, unsigned int dev_id, unsigned char _status){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":";
     strJson += fxn;
     strJson += ",\"did\":";
     strJson += dev_id;
     if(_status != -1){
       strJson += ",\"sta\":";
       strJson += _status;
     }
     strJson += "}]}";
     #ifdef IS_DEBUG
        Serial.println("strJsonStatus: "+strJson);
     #endif
     #ifdef USE_ELBUS_HMI_BRIDGE
       client.send(strJson); 
     #endif    
  } 
  void makeJsonSetDevice(unsigned char fxn, unsigned int dev_id, unsigned char _status, unsigned char _value){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":";
     strJson += fxn;
     strJson += ",\"did\":";
     strJson += dev_id;
     if(_status != -1){
       strJson += ",\"sta\":";
       strJson += _status;      //
     }
     if((_value != -1)){
       strJson += ",\"val\":";
       strJson += _value;
     }
     strJson += "}]}";
     #ifdef IS_DEBUG
        Serial.println("strJson: "+strJson);
     #endif
     if(dev_id > 0 && dev_id < 1000){
       #ifdef USE_ELBUS_HMI_BRIDGE
         client.send(strJson); 
       #endif 
     }   
  }

  void makeJsonSetMasterStatus(unsigned char loc_id, unsigned char _status){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":4";
     strJson += ",\"lid\":";
     strJson += loc_id;
     strJson += ",\"sta\":";
     strJson += _status;
     strJson += "}]}";
     
     #ifdef USE_ELBUS_HMI_BRIDGE
     if(loc_id != 0)
       client.send(strJson); 
     #endif 
  }

  void makeJsonSetScene(unsigned char scn_id){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":5";
     strJson += ",\"sid\":";
     strJson += scn_id;
     strJson += "}]}";
     
     #ifdef USE_ELBUS_HMI_BRIDGE
       if(scn_id != 0) client.send(strJson); 
     #endif 
  }

  void makeJsonSetCurtain(unsigned char cur_id, unsigned char cur_value){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":11";
     strJson += ",\"did\":";
     strJson += cur_id;
     strJson += ",\"val\":";
     strJson += cur_value;
     strJson += "}]}";
     #ifdef USE_ELBUS_HMI_BRIDGE
       client.send(strJson); 
     #endif  
  }

  void makeJsonSetAcOn(uint8_t lid, uint8_t ac_id, uint8_t ac_sta, uint8_t ac_temp, uint8_t ac_mode, uint8_t ac_swing, uint8_t ac_fspeed){
     uint16_t temp = ac_temp;
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":14";
     strJson += ",\"lid\":";
     strJson += lid;
     strJson += ",\"ac_id\":";
     strJson += ac_id;
     strJson += ",\"mode\":";
     strJson += ac_mode;
     strJson += ",\"temp\":";
     if(temp < 35) temp *= 10;
     strJson += temp;
     strJson += ",\"swing\":";
     strJson += ac_swing; 
     strJson += ",\"fan\":";
     strJson += ac_fspeed;
     strJson += ",\"sta\":";
     strJson += ac_sta;
     strJson += "}]}";
     #ifdef USE_ELBUS_HMI_BRIDGE
       client.send(strJson); 
     #endif     
  }
  
  void makeJsonSetAcOff(uint8_t lid, uint8_t ac_id, uint8_t ac_sta ){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":14";
     strJson += ",\"lid\":";
     strJson += lid;
     strJson += ",\"ac_id\":";
     strJson += ac_id; 
     strJson += ",\"sta\":";
     strJson += ac_sta;     
     strJson += "}]}";
     #ifdef USE_ELBUS_HMI_BRIDGE
       client.send(strJson); 
     #endif    
  } 
  void makeJsonSetAcParameters(uint8_t fxn, uint8_t lid, uint8_t ac_id){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":";
     strJson += fxn;
     strJson += ",\"lid\":";
     strJson += lid;
     strJson += ",\"ac_id\":";
     strJson += ac_id;   
     strJson += "}]}";
     #ifdef USE_ELBUS_HMI_BRIDGE
       client.send(strJson); 
     #endif    
  } 
  void makeJsonSetAcTemp(uint8_t lid, uint8_t ac_id, uint8_t ac_sta, uint8_t ac_temp ){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":14";
     strJson += ",\"lid\":";
     strJson += lid;
     strJson += ",\"ac_id\":";
     strJson += ac_id;
     strJson += ",\"sta\":";
     strJson += ac_sta;             
     strJson += ",\"temp\":";
     strJson += ac_temp;    
     strJson += "}]}";
     #ifdef USE_ELBUS_HMI_BRIDGE
       client.send(strJson); 
     #endif      
  }

  void makeJsonSetAcSwing(uint8_t lid, uint8_t ac_id, uint8_t ac_sta, uint8_t ac_swing ){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":14";
     strJson += ",\"lid\":";
     strJson += lid;
     strJson += ",\"ac_id\":";
     strJson += ac_id; 
     strJson += ",\"sta\":";
     strJson += ac_sta;            
     strJson += ",\"swing\":";
     strJson += ac_swing;    
     strJson += "}]}";
     #ifdef USE_ELBUS_HMI_BRIDGE
       client.send(strJson); 
     #endif      
  }
  void makeJsonSetAcFanSpeed(uint8_t lid, uint8_t ac_id, uint8_t ac_sta, uint8_t ac_fspeed ){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":14";
     strJson += ",\"lid\":";
     strJson += lid;
     strJson += ",\"ac_id\":";
     strJson += ac_id; 
     strJson += ",\"sta\":";
     strJson += ac_sta;  
     strJson += ",\"fan\":";
     strJson += ac_fspeed;    
     strJson += "}]}";
     #ifdef USE_ELBUS_HMI_BRIDGE
       client.send(strJson); 
     #endif     
  }
  void makeJsonSetAcMode(uint8_t lid, uint8_t ac_id, uint8_t ac_sta, uint8_t ac_mode ){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":14";
     strJson += ",\"lid\":";
     strJson += lid;
     strJson += ",\"ac_id\":";
     strJson += ac_id;
     strJson += ",\"sta\":";
     strJson += ac_sta;             
     strJson += ",\"mode\":";
     strJson += ac_mode;    
     strJson += "}]}";
     #ifdef USE_ELBUS_HMI_BRIDGE
       client.send(strJson); 
     #endif    
  }    
  void makeJsonRequestTimeFromServer(unsigned char lid){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":6";
     strJson += ",\"lid\":";
     strJson += lid;  
     strJson += "}]}";
     #ifdef USE_ELBUS_HMI_BRIDGE
       client.send(strJson); 
     #endif  
  }
  void makeJsonRequestDataFromServer(unsigned char lid){
     String strJson = "";
     strJson = "{\"data\":[{";
     strJson += "\"fxn\":9";
     strJson += ",\"lid\":";
     strJson += lid;  
     strJson += "}]}";
     #ifdef USE_ELBUS_HMI_BRIDGE
       client.send(strJson); 
     #endif 
  }
  
  void serialElbusReceiver(){
    unsigned int did = 0;
    unsigned char jk;
    if(flag_uart_elbus_data){
      flag_uart_elbus_data = false;
      if(UART_ELBUS_data_length > 0) {
        #ifdef IS_DEBUG
          for(int i=0; i<UART_ELBUS_data_length; i++){
            Serial.print(UART_ELBUS_data[i]); Serial.print(" "); 
          }
          Serial.println(); 
        #endif
        //added on 14-07-2022
        //SLVAE_ID from HMI is 1
        /*
          FXN_CODE=3 SET DEVICE        | SLAVE_ID == 1 | FXN_CODE | DEVICE_H | DEVICE_L | DEVICE_STATUS | DEVICE_VALUE | CRC_L | CRC_H | 
          FXN_CODE=4 SET LOCATION      | SLAVE_ID == 1 | FXN_CODE | LOCATION_ID | LOCATION_STATUS | CRC_L | CRC_H | 
          FXN_CODE=5 SET SCENE         | SLAVE_ID == 1 | FXN_CODE | SCENE_ID | CRC_L | CRC_H |
          FXN_CODE=7 SET SYNC          | SLAVE_ID == 1 | FXN_CODE | LOCATION_ID | CRC_L | CRC_H |
        */      
        if(UART_ELBUS_data[0] == 1){          
          if(UART_ELBUS_data[1] < 4){
            did = UART_ELBUS_data[2]<<8 | UART_ELBUS_data[3];
            if(UART_ELBUS_data[1] == 1){ //ON
              makeJsonSetDeviceStatusOnly(3, did, 1);
            }else if(UART_ELBUS_data[1] == 2){ //OFF
              makeJsonSetDeviceStatusOnly(3, did, 0);
            }else if(UART_ELBUS_data[1] == 3){
              makeJsonSetDevice(3, did, UART_ELBUS_data[4], UART_ELBUS_data[5]);
            }
          }else if(UART_ELBUS_data[1] == 4){                
            makeJsonSetMasterStatus(UART_ELBUS_data[2], UART_ELBUS_data[3]);
          }else if(UART_ELBUS_data[1] == 5){                
            makeJsonSetScene(UART_ELBUS_data[2]);
  //        }else if(UART_ELBUS_data[1] == 6){
  //          makeJsonRequestTimeFromServer(UART_ELBUS_data[2]);
          }else if(UART_ELBUS_data[1] == 7){
            makeJsonRequestDataFromServer(UART_ELBUS_data[2]);
          }else if(UART_ELBUS_data[1] == 9){  //set curtain
            makeJsonSetCurtain(UART_ELBUS_data[2], UART_ELBUS_data[3]); 
          }else if(UART_ELBUS_data[1] == 10){ //set ac
            //uint8_t lid, uint8_t ac_id, uint8_t ac_sta, uint8_t temp, uint8_t ac_mode, uint8_t ac_swing, uint8_t ac_fspeed
            if(UART_ELBUS_data[4] > 0)
              makeJsonSetAcOn(UART_ELBUS_data[2], UART_ELBUS_data[3], UART_ELBUS_data[4], UART_ELBUS_data[5], UART_ELBUS_data[6], UART_ELBUS_data[7], UART_ELBUS_data[8]);
            else   
              makeJsonSetAcOff(UART_ELBUS_data[2], UART_ELBUS_data[3], UART_ELBUS_data[4]); 
          }else if(UART_ELBUS_data[1] == 11){ //set ac temperature
            makeJsonSetAcTemp(UART_ELBUS_data[2], UART_ELBUS_data[3], UART_ELBUS_data[4], UART_ELBUS_data[5]);
          }else if(UART_ELBUS_data[1] == 12){ //set ac mode
            makeJsonSetAcMode(UART_ELBUS_data[2], UART_ELBUS_data[3], UART_ELBUS_data[4], UART_ELBUS_data[5]); 
          }else if(UART_ELBUS_data[1] == 13){ //set ac swing
            makeJsonSetAcSwing(UART_ELBUS_data[2], UART_ELBUS_data[3], UART_ELBUS_data[4], UART_ELBUS_data[5]);
          }else if(UART_ELBUS_data[1] == 14){ //set ac fan speed
            makeJsonSetAcFanSpeed(UART_ELBUS_data[2], UART_ELBUS_data[3], UART_ELBUS_data[4], UART_ELBUS_data[5]);
          }else if(UART_ELBUS_data[1] == 23){ //Increment AC Temperature   
            makeJsonSetAcParameters(23, UART_ELBUS_data[2], UART_ELBUS_data[3]);                 
          }else if(UART_ELBUS_data[1] == 24){ //Decrement AC Temperature
            makeJsonSetAcParameters(24, UART_ELBUS_data[2], UART_ELBUS_data[3]);   
          }else if(UART_ELBUS_data[1] == 22){ //Toggle   AC
            makeJsonSetAcParameters(22, UART_ELBUS_data[2], UART_ELBUS_data[3]);           
          }else if(UART_ELBUS_data[1] == 19){ //Toggle Device or Fan
            did = UART_ELBUS_data[2]<<8 | UART_ELBUS_data[3];
            makeJsonSetDeviceToggle(19, did);
          }else if(UART_ELBUS_data[1] == 20){ //Increment Fan 
            did = UART_ELBUS_data[2]<<8 | UART_ELBUS_data[3];
            makeJsonSetDeviceToggle(20, did);         
          }else if(UART_ELBUS_data[1] == 21){ //Decrement Fan
            did = UART_ELBUS_data[2]<<8 | UART_ELBUS_data[3];
            makeJsonSetDeviceToggle(21, did);   
          }else if(UART_ELBUS_data[1] == 17){ //Send Multiple Devices
            String strJson = "{\"data\":[";
            for(jk=0; jk<UART_ELBUS_data[2]; jk++){
               did = (UART_ELBUS_data[3+(jk*2)]<<8) | UART_ELBUS_data[4+(jk*2)];
               strJson += "{\"fxn\":3";
               strJson += ",\"did\":";
               strJson += did;
               strJson += ",\"sta\":0}";
               if(jk < UART_ELBUS_data[2]-1){
                  strJson += ","; 
               }
            }
            strJson += "]}";
            #ifdef IS_DEBUG
               Serial.println("strJson: "+strJson); //
            #endif
            #ifdef USE_ELBUS_HMI_BRIDGE
              client.send(strJson);
            #endif
          }
        }
      }
    }   
  }
#endif
