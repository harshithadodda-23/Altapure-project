#include "temperature.h"
#include "../mcc_generated_files/adc1.h"

static float Convert_ADC_To_Fahrenheit(uint16_t adcValue)
{
    return (adcValue * 150.0f) / 4095.0f;
}

static uint16_t Read_ADC(ADC1_CHANNEL channel)
{
    ADC1_ChannelSelect(channel);
    ADC1_SoftwareTriggerEnable();

    while (!ADC1_IsConversionComplete(channel));

    return ADC1_ConversionResultGet(channel);
}

void Temp_Init(void)
{
    ADC1_Enable();
}

float Temp_GetChamber(void)
{
    uint16_t adc = Read_ADC(TempSenseChamber);
    return Convert_ADC_To_Fahrenheit(adc);
}

float Temp_GetHeater(void)
{
    uint16_t adc = Read_ADC(TempSenseHeater);
    return Convert_ADC_To_Fahrenheit(adc);
}