
#include <stdint.h>
volatile uint32_t g_msTicks = 0;   // ? DEFINE HERE

#include "xc.h"

#include "app.h"

#include "app.h"
#include "state_machine.h"
#include "temperature.h"

void App_Init(void)
{
    Temp_Init();
    SM_Init();
}

void App_Run(void)
{
    SM_Run();
}