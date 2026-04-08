#include "state_machine.h"
#include "temperature.h"
#include "safety.h"
#include "../mcc_generated_files/pin_manager.h"
#include "app.h"
typedef enum
{
    STATE_IDLE,
    STATE_FILL,
    STATE_HEAT,
    STATE_FOG,
    STATE_FAULT

} SYSTEM_STATE;

static SYSTEM_STATE currentState;
static uint32_t fillStartTime = 0;
static uint32_t heatStartTime = 0;

extern volatile uint32_t g_msTicks;

void SM_Init(void)
{
    currentState = STATE_IDLE;
}

void SM_Run(void)
{
    float chamberTemp;
    float heaterTemp;

    switch(currentState)
    {
        case STATE_IDLE:
            currentState = STATE_FILL;
            break;

        case STATE_FILL:

            WaterPump_SetHigh();

            if (fillStartTime == 0)
                fillStartTime = g_msTicks;

            if (WaterPresent_GetValue() == 1)
            {
                WaterPump_SetLow();
                fillStartTime = 0;
                currentState = STATE_HEAT;
            }
            else if ((g_msTicks - fillStartTime) > 120000)
            {
                WaterPump_SetLow();
                currentState = STATE_FAULT;
            }

            break;

        case STATE_HEAT:

            chamberTemp = Temp_GetChamber();
            heaterTemp = Temp_GetHeater();

            if (heatStartTime == 0)
                heatStartTime = g_msTicks;

            if (Safety_CheckOverTemp(chamberTemp, heaterTemp))
            {
                Heater_SetLow();
                currentState = STATE_FAULT;
                break;
            }

            if (chamberTemp < 108.0f)
                Heater_SetHigh();
            else if (chamberTemp > 115.0f)
                Heater_SetLow();

            if (chamberTemp >= 108.0f && chamberTemp <= 115.0f)
            {
                Heater_SetLow();
                heatStartTime = 0;
                currentState = STATE_FOG;
            }
            else if ((g_msTicks - heatStartTime) > 600000)
            {
                Heater_SetLow();
                currentState = STATE_FAULT;
            }

            break;

        case STATE_FOG:

            chamberTemp = Temp_GetChamber();
            heaterTemp = Temp_GetHeater();

            if (Safety_CheckOverTemp(chamberTemp, heaterTemp) ||
                Safety_CheckWater())
            {
                currentState = STATE_FAULT;
                break;
            }

            WaterDosingValve_SetHigh();
            VentValves_SetHigh();

            if (chamberTemp < 108.0f)
                Heater_SetHigh();
            else if (chamberTemp > 115.0f)
                Heater_SetLow();

            break;

        case STATE_FAULT:

            Heater_SetLow();
            WaterPump_SetLow();
            WaterDosingValve_SetLow();
            VentValves_SetLow();

            break;
    }
}