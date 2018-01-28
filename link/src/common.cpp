#include "common.h"

/**
 * @crc: init value
 * @msg: msg need to do crc
 * @msgLen: length of msg
 * */
unsigned char CalCrc8( unsigned char crc, unsigned char *msg, unsigned long msgLen) {
    unsigned char Byte = 0;

    while (msgLen--)
    {
        Byte = crc ^ (*msg);
        crc = Crc8Table[Byte];
        msg++;
    }

    return crc;
}

unsigned short CalCrc16( unsigned short crc, unsigned char *msg,unsigned long msgLen) {
    unsigned char chData;

    if (msg == NULL) {
        return 0xFFFF;
    }

    while(msgLen--) {
        chData = *msg++;
        (crc) = ((unsigned short)(crc) >> 8)  ^ CRC16Table[((unsigned short)(crc) ^ (unsigned short)(chData)) & 0x00ff];
    }

    return crc;
}

uint32_t CalCrc32(uint32_t crc32val,unsigned char *msg, uint32_t msgLen) {
    uint32_t crc = crc32val ^ 0xffffffff;

    for (unsigned int i = 0; i < msgLen;  i ++) {
        crc = (crc >> 8) ^ CRC32Table[ (crc & 0xff) ^ msg[i] ] ;
    }

    return crc ^ 0xffffffff;
}

void Simple_EncryptAndDecrypt(INT8U *pucData, INT32U ulLen, INT16U usSeq) {
#define SIMPLE_KEY "xO$3(-2@#ld*viAQ~ixFE"

    static CHAR *s_pcsimple_key = SIMPLE_KEY;
    static INT32U   s_ulsimple_key_len = sizeof(SIMPLE_KEY) - 1;

    INT32S  i, j;
    INT8U   *pucIn;

    j = 0;
    pucIn = pucData;
    for (i = 0; i < (int)ulLen; i++) {
        j = (j + 1) ^ (i & 0x0F);
        if (j > s_ulsimple_key_len) {
            j = 0;
        }

        pucIn[i] = pucIn[i] ^ s_pcsimple_key[j];
        if(i & 0x01) {
            pucIn[i] = pucIn[i] ^ (INT8U)(usSeq >> 8);
        } else {
            pucIn[i] = pucIn[i] ^ (INT8U)(usSeq & 0xFF);
        }
    }
}

/**
 * result: 1 for success 0 for failure
 * */
int32_t getINT16LittleEnd(void *pOut, void *pIn) {
    if (!pOut || !pIn)
        return 0;

    memcpy(pOut, pIn, 2);
    return 1;
}
