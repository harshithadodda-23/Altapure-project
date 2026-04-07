
#include "xc.h"
#include "app.h"
#include "app.h"
#include "../mcc_generated_files/pin_manager.h"
#include "../mcc_generated_files/adc1.h"
#include "../mcc_generated_files/adc1.h"
static uint16_t Read_Temperature_ADC(ADC1_CHANNEL channel);
static float Convert_ADC_To_Fahrenheit(uint16_t adcValue);
static uint32_t heatStartTime = 0;
volatile uint32_t g_msTicks = 0;
typedef enum
{
    APP_STATE_IDLE = 0,
    APP_STATE_FILL,
    APP_STATE_HEAT,
    APP_STATE_FOG,
    APP_STATE_FAULT

} APP_STATE;

static APP_STATE currentState;

static uint32_t fillTimer = 0;
static uint32_t fillStartTime = 0;
#define FILL_TIMEOUT 120000   // 2 min in ms (adjust later)

void App_Init(void)
{
    currentState = APP_STATE_IDLE;
}

void App_Run(void)
{
    fillTimer++;
    switch(currentState)
    {
        case APP_STATE_IDLE:
    // Start condition (for now auto start)
    currentState = APP_STATE_FILL;
            break;

        case APP_STATE_FILL:
     WaterPump_SetHigh();

    if (fillStartTime == 0)
        fillStartTime = g_msTicks;

    if (WaterPresent_GetValue() == 1)
    {
        WaterPump_SetLow();
        fillStartTime = 0;
        currentState = APP_STATE_HEAT;
    }
    else if ((g_msTicks - fillStartTime) > 120000)   // 2 min
    {
        WaterPump_SetLow();
        fillStartTime = 0;
        currentState = APP_STATE_FAULT;
    }
            break;

        case APP_STATE_HEAT:
{
    uint16_t adcVal;
    float tempF;
    uint16_t heaterAdc;
    float heaterTemp;

    adcVal = Read_Temperature_ADC(TempSenseChamber);
    tempF = Convert_ADC_To_Fahrenheit(adcVal);

    if (heatStartTime == 0)
        heatStartTime = g_msTicks;

    // Heater sensor safety
    heaterAdc = Read_Temperature_ADC(TempSenseHeater);
    heaterTemp = Convert_ADC_To_Fahrenheit(heaterAdc);

    if (heaterTemp >= 120.0f)
    {
        Heater_SetLow();
        heatStartTime = 0;
        currentState = APP_STATE_FAULT;
        break;
    }

    // Chamber safety
    if (tempF >= 120.0f)
    {
        Heater_SetLow();
        heatStartTime = 0;
        currentState = APP_STATE_FAULT;
        break;
    }

    // Control
    if (tempF < 108.0f)
    {
        Heater_SetHigh();
    }
    else if (tempF > 115.0f)
    {
        Heater_SetLow();
    }

    // Target reached
    if (tempF >= 108.0f && tempF <= 115.0f)
    {
        Heater_SetLow();
        heatStartTime = 0;
        currentState = APP_STATE_FOG;
    }

    // Timeout
    else if ((g_msTicks - heatStartTime) > 600000)
    {
        Heater_SetLow();
        heatStartTime = 0;
        currentState = APP_STATE_FAULT;
    }

    break;
}

case APP_STATE_FOG:
{
    uint16_t adcVal;
    float tempF;

    // Read chamber temperature
    adcVal = Read_Temperature_ADC(TempSenseChamber);
    tempF = Convert_ADC_To_Fahrenheit(adcVal);

    // ? Safety: Over-temperature
    if (tempF >= 120.0f)
    {
        Heater_SetLow();
        WaterDosingValve_SetLow();
        VentValves_SetLow();

        currentState = APP_STATE_FAULT;
        break;
    }

    // ? Safety: No liquid
    if (WaterPresent_GetValue() == 0)
    {
        Heater_SetLow();
        WaterDosingValve_SetLow();
        VentValves_SetLow();

        currentState = APP_STATE_FAULT;
        break;
    }

    // ? Start fog system
    WaterDosingValve_SetHigh();
    VentValves_SetHigh();

    // ? Maintain temperature (same hysteresis)
    if (tempF < 108.0f)
    {
        Heater_SetHigh();
    }
    else if (tempF > 115.0f)
    {
        Heater_SetLow();
    }

    break;
}
    
    case APP_STATE_FAULT:

    Heater_SetLow();
    WaterPump_SetLow();
    WaterDosingValve_SetLow();
    VentValves_SetLow();

    // Optional: indicate fault (LED / buzzer)
    // Buzzer_SetHigh();

    break;    
        default:
            currentState = APP_STATE_FAULT;
            break;
    }
}
static uint16_t Read_Temperature_ADC(ADC1_CHANNEL channel)
{
    ADC1_ChannelSelect(channel);           // Select channel
    ADC1_SoftwareTriggerEnable();          // Start conversion

    while (!ADC1_IsConversionComplete(channel));  // Wait

    return ADC1_ConversionResultGet(channel);     // Read result
}

static float Convert_ADC_To_Fahrenheit(uint16_t adcValue)
{
    return (adcValue * 150.0f) / 4095.0f;
}