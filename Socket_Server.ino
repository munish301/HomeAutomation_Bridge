
#ifdef USE_SOCKET_SERVER
  void taskOne( void * parameter ) {
    while(1){
      WebsocketsClient client = server.accept();
      if(client.available()) {
        WebsocketsMessage msg = client.readBlocking();
        // log message
        #ifdef USE_WebSerial
          WebSerial.print("Got Message:");
          WebSerial.println(msg.data());
        #else
          Serial.print("Got Message: ");
          Serial.println(msg.data());
        #endif
        jsonString = msg.data();
        String mssg = handleRoot();
        #ifdef USE_WebSerial
          WebSerial.print("return_mssg: "); WebSerial.println(mssg);
        #else
          Serial.print("return_mssg: "); Serial.println(mssg);
        #endif
        // return echo
        client.send(mssg);
        // close the connection
        //client.close();
      }
    }
  }
  
  void Create_Server() {
    server.listen(8081);
    Serial.print("Is server live? ");
    Serial.println(server.available());
  
    xTaskCreatePinnedToCore(
        taskOne,      // Function that should be called
        "TaskOne",    // Name of the task (for debugging)
        10000,        // Stack size (bytes)
        0,            // Parameter to pass
        1,            // Task priority
        0,            // Task handle
        1             // Core you want to run the task on (0 or 1)
    );
  }
#endif
