int16_t calculate_crc16x(uint8_t *buf, uint8_t len)
{  
  int16_t crc = 0xFFFF;
  for (uint8_t pos = 0; pos < len; pos++){
    crc ^= (uint16_t)buf[pos];    // XOR byte into least sig. byte of crc

    for (int16_t i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      } else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  return crc;
}

uint16_t calculateCRC(uint8_t *buffer, int length)
{
    int i, j;
    uint16_t crc = 0xFFFF;
    uint16_t tmp;

    // Calculate the CRC.
    for (i = 0; i < length; i++)
    {
        crc = crc ^ buffer[i];

        for (j = 0; j < 8; j++)
        {
            tmp = crc & 0x0001;
            crc = crc >> 1;
            if (tmp)
            {
                crc = crc ^ 0xA001;
            }
        }
    }

    return crc;
}

bool crc_test_rx(uint8_t* u8Bytes, int nCount) {
    uint16_t crc_temp_reg=calculate_crc16x(u8Bytes, nCount-2);
    uint8_t crc_H = (uint8_t)((crc_temp_reg >> 8) & 0xff);
    uint8_t crc_L = (uint8_t)(crc_temp_reg & 0xff);
    if ((crc_H == u8Bytes[nCount-1]) && (crc_L == u8Bytes[nCount-2])) 
        return true;
    return false;
}// end bool validate_crc16(uint8_t* u8Bytes, int nCount)
