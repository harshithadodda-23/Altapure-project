#ifndef CAN_H
#define CAN_H

#include <stdint.h>

typedef struct
{
    uint16_t id;
    uint8_t dlc;
    uint8_t data[8];

} CAN_MSG;

void CAN1_Init(void);
void CAN1_Send(uint16_t id, uint8_t *data, uint8_t len);
uint8_t CAN1_Receive(CAN_MSG *msg);

#endif
