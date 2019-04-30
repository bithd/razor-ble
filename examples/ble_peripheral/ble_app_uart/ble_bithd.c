#include "ble_bithd.h"
#include "sys.h"
#include <string.h>
#include "ble_gap.h"

//extern app_timer_id_t                   m_sec_req_timer_id;
unsigned char                           ParingDataBuf[6];                                //save paring datas bufer
unsigned char                           ParingDataDisFlag=0;                             //paring data display flag/ 1 need display/ 0 not need display
unsigned char                           Ble_ConnnectFlag=BLE_Disconnect;                 //ble connect flag
unsigned char                           Ble_AuthFlag=0;                                  //ble authentication flag / 1 hasbeen authentication/ 0 not authentication

//static dm_application_instance_t        m_app_handle;                                    /**< Application identifier allocated by device manager */
//static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;         /**< Handle of the current connection. */
//static app_bond_table_t                 m_app_bond_table;  
//static dm_handle_t                      m_peer_handle;                                   /**< Identifies the peer that is currently connected. */
unsigned char mac_ascii[24];
unsigned char mac[6]={0x42,0x13,0xc7,0x98,0x95,0x1a}; //Device MAC address

void blueDisconnect(void)
{
	Ble_AuthFlag=BLE_AuthFail;
	//sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}


/***************************************************************
fuction:get local bluetooth address
****************************************************************/
void Mac_address_get(void)
{
	ble_gap_addr_t  Mac_address;
	unsigned char i,j=0;
	//_sd_ble_gap_addr_get(&Mac_address);

 	uint32_t err_code = sd_ble_gap_addr_get(&Mac_address);
    APP_ERROR_CHECK(err_code);
     
    memcpy(mac,Mac_address.addr,6);
	for(i=0;i<6;i++)
	{
	if((mac[i]>>4)<0x0a)
	{
		mac_ascii[j]=0x30+(mac[i]>>4);
		j++;
	}
	else
	{
		mac_ascii[j]=0x31;
		j++;
		mac_ascii[j]=0x30+(mac[i]>>4)%0x0a;
		j++;
	}

	if((mac[i]&0x0f)<0x0a)
	{
			mac_ascii[j]=0x30+(mac[i]&0x0f);
			j++;
	}
	else
	{
		mac_ascii[j]=0x31;
		j++;
		mac_ascii[j]=0x30+(mac[i]&0x0f)%0x0a;
		j++;
	}
	}		

	memcpy(&DEVICE_NAME[5],mac_ascii,12);
}


