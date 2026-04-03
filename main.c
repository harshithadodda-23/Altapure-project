
/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"                                    //bob added this ... assumed this was automatic. No.
#include "mcc_generated_files/tmr1.h"                                           //bob added this ... assumed this was automatic. No.
#include "mcc_generated_files/interrupt_manager.h"
#include "mcc_generated_files/adc1.h" 


#include "stdio.h"
#include "slaveCommon.h"
#include "dspicSlave.h"
#include "setup.h"


void fsm_1(void);
void fsm_2(void);
void fsm_3(void);

extern TMR_OBJ tmr1_obj;      

/*
                         Main application
 */

unsigned char workbyte[2];

//******************************************************************************
int main(void)
{

    //RPINR18 = 0x0061;       //U1TX now using RF1 (can transmit)   ONLY IF UART IS TO BE USED
    //RPOR7 = 0x0001;         //U1RXR now using RF0 (can receive)
    
    // initialize the device
    SYSTEM_Initialize();
    
    ioSetup();
    
    TMR1_SetInterruptHandler(Timer1_Isr);                                       //Timer1_Isr is pointer to interrupt function... so what does this do?
        
    TMR1_Start();

//************** setup *********************************************************
    
    LedDiagnostic_1_Green_SetDigitalOutput();
    ChamberLedWhite_SetDigitalOutput();
    ChamberLedRed_SetDigitalOutput();
    ChamberLedGreen_SetDigitalOutput();
    ChamberLedBlue_SetDigitalOutput();

    Enable2562_SetLow();        // enable can xceiver (for differential serial)
//************** loop **********************************************************   
    while (1)
    {
        MsDelay(10);
        
        fsm_1();
        
        
        fsm_2(); 
        
        
        fsm_3();
        
     
        DecTimerArray();
        
        
        
    }
    return 1; 
}


//******************************************************************************


/**
 End of File
*/

