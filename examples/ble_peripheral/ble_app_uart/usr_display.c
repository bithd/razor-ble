#include "usr_display.h"
#include "IO_CTL.h"
#include "uart_bithd.h"
#include "nrf_delay.h"

#if 0
void UpdateBleStatus(void)
{
	if(g_FlagBleConnect == BLE_CONNECT_FLAG)
	{
		g_FlagBleConnect = BLE_DEFAULT_FLAG;
		BLE_CONNECT();
		nrf_delay_ms(5);
	}
	else if(g_FlagBleConnect == BlE_DISCONNECT_FLAG)
	{
		g_FlagBleConnect = BLE_DEFAULT_FLAG;
		BlE_DISCONNECT();
		nrf_delay_ms(5);
	}
	else
	{
		
	}
}

void UpdateBatVolt(void)
{
	if(g_BatUpdateFlag == TO_UPDATE)
	{
		g_BatUpdateFlag = NO_UPDATE;
		//BATPERCENT_FUNC();
	}
	else
	{
		
	}

	//CHARGE
	if(g_ChargeFlag == YES_CHARGE)
	{
		g_ChargeFlag = INIT_CHARGE;
		CHARGE_FUNC();
		nrf_delay_ms(5);
	}
	else if(g_ChargeFlag == MV_CHARGE)
	{
		g_ChargeFlag = INIT_CHARGE;
		MVCHARGE_FUNC();
		nrf_delay_ms(5);
	}
	else
	{
		
	}
}

void f_InitialDisplay(void)
{
	INIT_FUNC();
	UpdateBleStatus();
	UpdateBatVolt();		
	INIT_FUNC();
}

void f_BalanceDisplay(void)
{	
	SendKey2Action();
}

void f_BlePairCode(void)
{
	
}
void f_ProductInfoDisplay(void)
{
	SendKey1Action();
}
void f_InitIO(void)
{
	ResetFunctionIo();
}

#endif
