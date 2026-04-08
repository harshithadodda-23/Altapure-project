#include "safety.h"
#include "../mcc_generated_files/pin_manager.h"

bool Safety_CheckOverTemp(float chamberTemp, float heaterTemp)
{
    if (chamberTemp >= 120.0f)
        return true;

    if (heaterTemp >= 120.0f)
        return true;

    return false;
}

bool Safety_CheckWater(void)
{
    if (WaterPresent_GetValue() == 0)
        return true;

    return false;
}