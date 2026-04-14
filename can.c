#include "can.h"
#include <xc.h>

#define FCY 12000000UL

void CAN1_Init(void)
{
    C1CTRL1bits.REQOP = 4;
    while (C1CTRL1bits.OPMODE != 4);

    C1CFG1bits.BRP = 2;
    C1CFG1bits.SJW = 1;

    C1CFG2bits.PRSEG = 2;
    C1CFG2bits.SEG1PH = 5;
    C1CFG2bits.SEG2PH = 5;
    C1CFG2bits.SAM = 1;

    C1RX0CONbits.RXFUL = 0;
    C1RX1CONbits.RXFUL = 0;

    C1RX0CONbits.RXM = 0;
    C1RX1CONbits.RXM = 0;

    C1RXF0SIDbits.SID = 0x200;
    C1RXM0SIDbits.SID = 0x7FF;

    C1CTRL1bits.REQOP = 0;
    while (C1CTRL1bits.OPMODE != 0);
}

void CAN1_Send(uint16_t id, uint8_t *data, uint8_t len)
{
    while (C1TX0CONbits.TXREQ);

    C1TX0SIDbits.SID = id;
    C1TX0SIDbits.SRR = 0;
    C1TX0EIDbits.IDE = 0;

    C1TX0DLCbits.DLC = len;

    C1TX0B1 = ((uint16_t)data[1] << 8) | data[0];
    C1TX0B2 = ((uint16_t)data[3] << 8) | data[2];
    C1TX0B3 = ((uint16_t)data[5] << 8) | data[4];
    C1TX0B4 = ((uint16_t)data[7] << 8) | data[6];

    C1TX0CONbits.TXREQ = 1;
}

uint8_t CAN1_Receive(CAN_MSG *msg)
{
    if (C1RX0CONbits.RXFUL)
    {
        msg->id = C1RX0SIDbits.SID;
        msg->dlc = C1RX0DLCbits.DLC;

        msg->data[0] = (uint8_t)(C1RX0B1 & 0xFF);
        msg->data[1] = (uint8_t)(C1RX0B1 >> 8);
        msg->data[2] = (uint8_t)(C1RX0B2 & 0xFF);
        msg->data[3] = (uint8_t)(C1RX0B2 >> 8);
        msg->data[4] = (uint8_t)(C1RX0B3 & 0xFF);
        msg->data[5] = (uint8_t)(C1RX0B3 >> 8);
        msg->data[6] = (uint8_t)(C1RX0B4 & 0xFF);
        msg->data[7] = (uint8_t)(C1RX0B4 >> 8);

        C1RX0CONbits.RXFUL = 0;
        return 1;
    }

    return 0;
}
