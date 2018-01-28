#define LOG_TAG "LinkEvent"

#include <utils/Log.h>

#include "common.h"
#include "LinkEvent.h"

using namespace android;

/**
 *FIX ME: kakazhang 2018-01-11
 * parse link event here
 * */
void LinkEvent::parseData(uint8_t* buffer, int32_t len) {
#ifdef DebugLinkEvent
    //current position
    int curr = 0;
    int parseState = NO_MAGIC;
    int packetLen = 0;

    while (curr < len) {
        if (NO_MAGIC == parseState) {
            if (PROTOCOL_MAGIC == buffer[curr]) {
                parseState = FIND_VER_LEN_CRC;
                mPos = 0;
                mRecvBuffer[mPos++] = buffer[curr];
            }
        } else if (FIND_VER_LEN_CRC == parseState) {
            mRecvBuffer[mPos++] = buffer[curr];
            if(4 == mPos) {
                uint8_t ucCrc8 = CalCrc8(MSG_HCRC_INITCRC, mRecvBuffer, 3);
                //In the first 4 bytes, the 4th(index : 3) byte store crc8 value of previous 3 bytes(index: 0-2)
                if(ucCrc8 != mRecvBuffer[mPos-1]) {
                    parseState = NO_MAGIC;
                    continue;
                } else {
                    //calculate packet length
                    packetLen = mRecvBuffer[1] + (mRecvBuffer[2] & 0x03) * 256;
                    parseState = FIND_BODY;
                    if (packetLen > MAX_PACKET_LEN)
                        parseState = NO_MAGIC;
                }
            }
        } else if (FIND_BODY == parseState) {
            if (mPos >= packetLen) {
                parseState = NO_MAGIC;
                //the last 2 bytes in packet is crc16 value of all previous bytes(0 ~ mPos - 2)
                unsigned short  usCrc16Cal = CalCrc16(MSG_WCRC_INITCRC, mRecvBuffer, mPos-2);
                unsigned short usCrc16;
                getINT16LittleEnd(&usCrc16, mRecvBuffer+mPos-2);
                //crc check success
                if (usCrc16Cal == usCrc16) {
                    T_ProtocolHeaderStruct* pHeader = (T_ProtocolHeaderStruct*)mRecvBuffer;
                    if(pHeader->uCmdType.u.ucEncrypt == 3) {
                        Simple_EncryptAndDecrypt(mRecvBuffer+9, mPos-11, pHeader->usSeqNum);
                        pHeader->uCmdType.u.ucEncrypt = 0;
                        unsigned short usCrc16Cal = CalCrc16(MSG_WCRC_INITCRC, mRecvBuffer, mPos-2);
                        memcpy(mRecvBuffer+pHeader->uLV.u.usLength-2, &usCrc16Cal, 2);
                    }

                    //callback
                    mCallback->callback(mRecvBuffer, mPos);
                }

                //reset pos
                mPos = 0;
            }
        }
        curr++;
    }
#else
    mCallback->callback((void *)buffer, len);
#endif
}
