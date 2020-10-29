#ifndef _CRC_H_
#define _CRC_H_

#include <stdint.h>

uint8_t CRC5(uint8_t *ptr, uint32_t len);
uint16_t CRC16(const uint8_t* p_data, uint16_t w_len);

#endif
