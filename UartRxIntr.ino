
#define BUF_SIZE (1024 * 2)
#define RD_BUF_SIZE (1024)
static QueueHandle_t uart_elbus_queue;
static QueueHandle_t uart_modbus_queue; 
static const char * TAG = "";                  

void initElbusUart(){
      //Configuro la porta Serial2 (tutti i parametri hanno anche un get per effettuare controlli)
    uart_config_t Configurazione_UART2 = {
        .baud_rate = DCB1M_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(NUMERO_PORTA_ELBUS, &Configurazione_UART2);
    //Firma: void esp_log_level_set(const char *tag, esp_log_level_tlevel)
    esp_log_level_set(TAG, ESP_LOG_INFO);
    //Firma: esp_err_tuart_set_pin(uart_port_tuart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num)
    uart_set_pin(NUMERO_PORTA_ELBUS, PIN_ELBUS_TXD, PIN_ELBUS_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
 
    //Firma: uart_driver_install(UART_NUM_2, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));
    //       uart_driver_install(Numero_porta, RXD_BUFFER, TXD_Buffer, event queue handle and size, flags to allocate an interrupt)
    uart_driver_install(NUMERO_PORTA_ELBUS, BUF_SIZE, BUF_SIZE, 100, &uart_elbus_queue, 0);
    //Create a task to handler UART event from ISR
    xTaskCreatePinnedToCore(UART_ELBUS_ISR_ROUTINE, "UART_ELBUS_ISR_ROUTINE", 2048, NULL, 100, NULL, 0); 
}
 
static void UART_ELBUS_ISR_ROUTINE(void *pvParameters){
    uart_event_t event;
    //Infinite loop to run main bulk of task
    while (1) {
      //Loop will continually block (i.e. wait) on event messages from the event queue
      if(xQueueReceive(uart_elbus_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
          //Handle received event
          if (event.type == UART_DATA) {
              //Handle your uart reading here
              ESP_ERROR_CHECK(uart_get_buffered_data_len(NUMERO_PORTA_ELBUS, (size_t*)&UART_ELBUS_data_length));
              UART_ELBUS_data_length = uart_read_bytes(NUMERO_PORTA_ELBUS, UART_ELBUS_data, UART_ELBUS_data_length, 10);
              flag_uart_elbus_data = true;
          }else if (event.type == UART_FRAME_ERR) {
              //Handle frame error event
          }
           //Keep adding else if statements for each UART event you want to support
          else {
              //Final else statement to act as a default case
          }      
      }
    }
}

#ifdef USE_MODBUS_BRIDGE
  #ifndef USE_MODBUS_LIBRARY
    #ifndef USE_SIMPLE_UART
      void initModbusUart(){
           //Configuro la porta Serial1 (tutti i parametri hanno anche un get per effettuare controlli)
          uart_config_t Configurazione_UART1 = {
              .baud_rate = RS485_BAUD,
              .data_bits = UART_DATA_8_BITS,
              .parity = UART_PARITY_DISABLE,
              .stop_bits = UART_STOP_BITS_1,
              .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
          };
          uart_param_config(NUMERO_PORTB_MODBUS, &Configurazione_UART1);
          //Firma: void esp_log_level_set(const char *tag, esp_log_level_tlevel)
          esp_log_level_set(TAG, ESP_LOG_INFO);
          //Firma: esp_err_tuart_set_pin(uart_port_tuart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num)
          uart_set_pin(NUMERO_PORTB_MODBUS, MODBUS_TXD, MODBUS_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
       
          //Firma: uart_driver_install(UART_NUM_2, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));
          //       uart_driver_install(Numero_porta, RXD_BUFFER, TXD_Buffer, event queue handle and size, flags to allocate an interrupt)
          uart_driver_install(NUMERO_PORTB_MODBUS, BUF_SIZE, BUF_SIZE, 100, &uart_modbus_queue, 0);
          //Create a task to handler UART event from ISR
          xTaskCreatePinnedToCore(UART_MODBUS_ISR_ROUTINE, "UART_MODBUS_ISR_ROUTINE", 2048, NULL, 100, NULL, 1); 
      }
      
      static void UART_MODBUS_ISR_ROUTINE(void *pvParameters){
          uart_event_t event;
          //Infinite loop to run main bulk of task
          while (1) {
            //Loop will continually block (i.e. wait) on event messages from the event queue
            if(xQueueReceive(uart_modbus_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
                //Handle received event
                if (event.type == UART_DATA) {
                    //Handle your uart reading here
                    ESP_ERROR_CHECK(uart_get_buffered_data_len(NUMERO_PORTB_MODBUS, (size_t*)&UART_MODBUS_data_length));
                    UART_MODBUS_data_length = uart_read_bytes(NUMERO_PORTB_MODBUS, UART_MODBUS_data, UART_MODBUS_data_length, 10);
                    flag_uart_modbus_data = true;
                }else if (event.type == UART_FRAME_ERR) {
                    //Handle frame error event
                }else { //Keep adding else if statements for each UART event you want to support
                    //Final else statement to act as a default case
                }      
            }
          }
      }
    #else
      uint16_t modbus_calculateCRC(uint8_t *buffer, int length){
          int i, j;
          uint16_t crc = 0xFFFF;
          uint16_t tmp;
      
          // Calculate the CRC.
          for (i = 0; i < length; i++){
              crc = crc ^ buffer[i];
              for (j = 0; j < 8; j++){
                  tmp = crc & 0x0001;
                  crc = crc >> 1;
                  if (tmp){
                      crc = crc ^ 0xA001;
                  }
              }
          }
          return crc;
      }// end uint16_t modbus_calculateCRC(uint8_t *buffer, int length)
      
      bool modbus_validateCRC(uint8_t *buffer, int length){
          int i, j;
          uint16_t crc = 0xFFFF;
          uint16_t tmp;    
          // Calculate the CRC.
          for (i = 0; i < length; i++){
              crc = crc ^ buffer[i];
              for (j = 0; j < 8; j++){
                  tmp = crc & 0x0001;
                  crc = crc >> 1;
                  if (tmp){
                      crc = crc ^ 0xA001;
                  }
              }
          }
          if(!crc)
            return true;
          else
           return false;
      }// end bool modbus_validateCRC(uint8_t *buffer, int length)
    
      void modbus_onReceive(void){
        UART_MODBUS_data_length=modbus_serial.read(UART_MODBUS_data, MAX_RX_DATA); //
        if(modbus_validateCRC(UART_MODBUS_data, UART_MODBUS_data_length)){
           flag_uart_modbus_data = true;
        }// end if(rx_data[INDEX_SLAVE_ID]==sttDevConfig.u8ID)
      }// end void modbus_onReceive(void)
    #endif
  #endif
#endif
