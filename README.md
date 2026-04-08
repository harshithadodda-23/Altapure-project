# Altapure-project

main.c
   ↓
App_Run()
   ↓
SM_Run()   ← decision making
   ↓
Temp_Get...()   ← sensor input
Safety_Check...() ← protection
   ↓
GPIO control (heater, pump, valves)

#Explanation:

The flow starts in main.c, which initializes the system and hands control over to the main application loop via App_Run().

Within this loop, SM_Run() (State Machine) handles the core decision-making. It evaluates the current system state and determines what actions need to be taken.

To support these decisions:

Temp_Get...() provides real-time temperature readings from the sensor
Safety_Check...() ensures all operating conditions remain within safe limits

Based on these inputs, the system then drives the hardware through GPIO controls, such as operating the heater, pump, and valves.

In essence, the flow follows a structured sequence:
read inputs → validate safety → make decisions → control outputs
