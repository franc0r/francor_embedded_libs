#include "stm32f446xx.h"
#include "core_cm4.h"


SCnSCB_Type     SIM_SCS_BASE;
SCB_Type        SIM_SCB_BASE;
SysTick_Type    SIM_SysTick_BASE;
NVIC_Type       SIM_NVIC_BASE;
ITM_Type        SIM_ITM_BASE;
DWT_Type        SIM_DWT_BASE;
TPI_Type        SIM_TPI_BASE;
CoreDebug_Type  SIM_CoreDebug_BASE;