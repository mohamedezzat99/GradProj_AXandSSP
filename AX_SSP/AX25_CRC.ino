/*--------------------------------------------------------------------------*
 * AX25_CRC.c
 * FCS operations of the AX.25 frame.
 *--------------------------------------------------------------------------*/

#include "AX25_CRC.h"


/*--------------------------------------------------------------------------*
 * AX.25 FCS calculation (bitwise method).
 * CRC-16-CCITT G(x) = x16 + x12 + x5 + 1.
 * Polynom = 0x1021.
 *
 * PARAMETERS:
 * *buffer       pointer of the frame buffer.
 * *OpArrSize    it stores the index of the last inserted element in the array to keep track of size
 *
 * RETURN:
 * the CRC with a final XORed operation.
 *--------------------------------------------------------------------------*/

uint16 computeCRC( uint8* data_p, uint16 *length) {
  unsigned char x;
  unsigned short crc = 0xFFFF;
  uint16 len;
  len = *length;
  uint8 data_copy;
  while (len--) {
      data_copy = *data_p;
    /*  reverse the bits in each 8-bit byte going in */
    data_copy = (data_copy & 0x55555555) << 1 | (data_copy & 0xAAAAAAAA) >> 1;
    data_copy = (data_copy & 0x33333333) << 2 | (data_copy & 0xCCCCCCCC) >> 2;
    data_copy = (data_copy & 0x0F0F0F0F) << 4 | (data_copy & 0xF0F0F0F0) >> 4;

    x = crc >> 8 ^ data_copy;
    data_p++;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x << 5)) ^ ((unsigned short)x);
  }
  /*reverse the 16-bit CRC*/
  crc = (crc & 0x55555555) << 1 | (crc & 0xAAAAAAAA) >> 1;
  crc = (crc & 0x33333333) << 2 | (crc & 0xCCCCCCCC) >> 2;
  crc = (crc & 0x0F0F0F0F) << 4 | (crc & 0xF0F0F0F0) >> 4;
  crc = (crc & 0x00FF00FF) << 8 | (crc & 0xFF00FF00) >> 8;

  return crc;
}
/*--------------------------------------------------------------------------*
 * AX.25 FCS positioning.
 * Put the FCS in the right place in the frame. The FCS is sent MSB first
 * so we prepare the 15th bit of the CRC to be sent first.
 *
 * PARAMETERS:
 * *buffer        pointer of the frame buffer.
 * *OpArrSize     it stores the index of the last inserted element in the array to keep track of size
 *--------------------------------------------------------------------------*/
void AX25_putCRC(uint8 *buffer, uint16 *OpArrSize) {
  uint16 crc;

  /* FCS calculation. */
  crc = computeCRC(buffer, OpArrSize);

  /* Put the FCS in the right place with the 15th bit to be sent first. */
  buffer[*OpArrSize] = ((crc >> 8) & 0xff);
  *OpArrSize=*OpArrSize+1;
  buffer[*OpArrSize] = (crc & 0xff);
  *OpArrSize=*OpArrSize+1;
}
