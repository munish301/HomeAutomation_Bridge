 /***********************************************************************
 * void initFlashMemory()
***********************************************************************/
#ifdef USE_SPIFFS
  void initFlashMemory() {
    if (!SPIFFS.begin()) { //
      #ifdef IS_DEBUG 
        Serial.println("SPIFFS Mount Failed");
      #endif  
      // Flash HB LED fast to indicate error
      // digitalWrite(LED_HB, HIGH);
      resetFunc();
      while (1); //Hang In There
    }   
    read_config();
  }// end void initFlashMemory()
  
  /**********************************************************************
   * bool read_config()
  ***********************************************************************/
  bool read_config() {
  //  String str;
  //  String sTmp;
  //  String sCmp="//";
  
  //  File file = SPIFFS.open("/config.csv");
  //  if (!file) {
  //    Serial.println("- failed to open file for reading");
  //    return false;
  //  }
  // // Serial.println("File OPened");
  //  if (file.available()) {    
  //    sTmp="";
  //    while(file.available()){    
  //      str = file.readStringUntil('\n');      
  //      if(!str.startsWith(sCmp)){ // Not a Comment
  //        StringTokenizer tokens(str, ",");
  //        if(tokens.hasNext()){
  //          tokens.nextToken();  // 
  //          //tokens.nextToken();
  //          sTmp += (String)tokens.nextToken(); // 
  //          sTmp += ",";
  //        } // end if(tokens.hasNext())
  //      }// end if(!str.startsWith(sCmp))
  //    }// end while(file.available())
  //   }// end  if(file.available()){  
  //  file.close(); 

  // StringTokenizer tokens(sTmp, ",");
  // if(tokens.hasNext()){  
  //    uint8_t u8Len = 0;    
  //    config.server_ip=(String)tokens.nextToken();  // 
  //    config.server_port=(int16_t)tokens.nextToken().toInt();  //        
  // }// end if(tokens.hasNext())  
  // print_config_file();
   return true;
  }// end bool read_config()
  
  void print_config_file(){
    uint8_t i=0;
    #ifdef IS_DEBUG 
      Serial.println("*************WB Config*************"); 
      Serial.print("My IP : "); Serial.println(config.server_ip);
      Serial.print("My Port : "); Serial.println(config.server_port);
      Serial.println("**********end WB Config**********");
    #endif
    /***********************************/
  }// end void initConfigVar()
#endif
