#ifndef M24C0X_H_INCLUDED
#define M24C0X_H_INCLUDED


#include <stdint.h>
#include <main.h>


#ifdef __cplusplus
extern "C" {
#endif



void EEprom24lc16b_Init(void);
int16_t EEprom24lc16b_ReadByte(uint16_t addr);
uint8_t EEprom24lc16b_WriteByte(uint16_t addr, uint8_t data);
uint8_t EEprom24lc16b_ReadByteArray(uint16_t addr, uint8_t *pData, uint16_t len);
uint8_t EEprom24lc16b_WriteByteArray(uint16_t addr, uint8_t *pData, uint16_t len);

#ifdef __cplusplus
}
#endif


#endif /* M24C0X_H_INCLUDED */
