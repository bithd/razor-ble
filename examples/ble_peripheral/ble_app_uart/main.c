#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "nrf_ble_gatt.h"
#include "app_timer.h"
#include "app_button.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp.h"
#include "nrf_delay.h"
#include "peer_manager.h"
#include "fds.h"
#include "fstorage.h"
#include "ble_conn_state.h"
#include "nrf_pwr_mgmt.h"
#include "scheduler_bithd.h"
#include "ble_bithd.h"
#include "uart_bithd.h"
#include "usr_display.h"
#include "Bluetooth_HWprotocol.h"
#include "uart_bithd.h"
#include "program.h"
#include "closeoled.h"
#include "ADC_BITHD.h"
#include "timedis.h"
#include "bluetoothKEY_bithd.h"
#include "flashmcu_bithd.h"
#include "ble_bithd.h"
#include "Timer_Interrupt_Function.h"

#if BLE_BAS_ENABLED
#include "ble_bas.h"
#endif
#if BLE_DIS_ENABLED
#include "ble_dis.h"
#endif
#include "sys.h"
#ifdef ADC_PRESENT
#include "nrf_drv_adc.h"
#else
#include "nrf_drv_saadc.h"
#endif //ADC_PRESENT

#define CONN_CFG_TAG                    1                                           /**< A tag that refers to the BLE stack configuration we set with @ref sd_ble_cfg_set. Default tag is @ref BLE_CONN_CFG_TAG_DEFAULT. */

#define IS_SRVC_CHANGED_CHARACT_PRESENT 	1                                           /**< Include the service_changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device. */
//move to ble_nus.h define
//#if (NRF_SD_BLE_API_VERSION == 3)
//#define NRF_BLE_MAX_MTU_SIZE            158 //GATT_MTU_SIZE_DEFAULT                       /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
//#endif

//#define CENTRAL_LINK_COUNT              0                                           /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
//#define PERIPHERAL_LINK_COUNT           1                                           /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_BLE                  /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_ADV_INTERVAL                256 //64                                    /**< The advertising interval (in units of 0.625 ms. This value corresponds to 160 //40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      120                                         /**< The advertising timeout (in units of seconds). */
// Transfer to sys.h
//#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
//#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */
//Transfer to sys.h
//#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
//#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
//#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
//#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
//#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
//#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
//#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */
// Transfer to usr_bonding.c
//#define SEC_PARAM_BOND                   1                                           /**< Perform bonding. */
//#define SEC_PARAM_MITM                   0                                           /**< Man In The Middle protection not required. */
//#define SEC_PARAM_LESC                   0                                           /**< LE Secure Connections not enabled. */
//#define SEC_PARAM_KEYPRESS               0                                           /**< Keypress notifications not enabled. */
//#define SEC_PARAM_IO_CAPABILITIES        BLE_GAP_IO_CAPS_DISPLAY_ONLY //BLE_GAP_IO_CAPS_NONE //                        /**< No I/O capabilities. */
//#define SEC_PARAM_OOB                    0                                           /**< Out Of Band data not available. */
//#define SEC_PARAM_MIN_KEY_SIZE           7                                           /**< Minimum encryption key size. */
//#define SEC_PARAM_MAX_KEY_SIZE           16                                          /**< Maximum encryption key size. */

#define PASSKEY_TXT_LENGTH              8                                           /**< Length of message to be displayed together with the pass-key. */
#define PASSKEY_LENGTH                  6                                           /**< Length of pass-key received by the stack for display. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */

ble_nus_t                        		m_nus;                                      /**< Structure to identify the Nordic UART Service. */
//sys.c need use it
/*static */uint16_t                     m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
//add for SDK13
static nrf_ble_gatt_t                   m_gatt;   
//static uint16_t                         m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;  /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
#if (BLE_BAS_ENABLED && BLE_DIS_ENABLED)
static ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE},
														{BLE_UUID_BAS_SERVICE,BLE_UUID_TYPE_BLE},
														{BLE_UUID_DEVICE_INFORMATION_SERVICE,BLE_UUID_TYPE_BLE}};  /**< Universally unique service identifier. */
#elif (BLE_BAS_ENABLED)
static ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE},
														{BLE_UUID_BAS_SERVICE,BLE_UUID_TYPE_BLE}};
#elif (BLE_DIS_ENABLED)
static ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE},
														{BLE_UUID_DEVICE_INFORMATION_SERVICE,BLE_UUID_TYPE_BLE}};
#else
static ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}};
#endif


extern void peer_manager_init(bool erase_bonds);
/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyse
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/*****************************************************************************
 函数:  gap_params_init
 输入:	无
 输出:	无
 		
 返回:	无
 		
 功能:
			gap参数初始化
******************************************************************************/
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

	//set fixed Passkey
	//ble_opt_t ble_opt; 
	//ble_opt.gap_opt.passkey.p_passkey = g_ucBleTK; 

	//
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
										(const uint8_t *) DEVICE_NAME,
										strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);

	//set fixed Passkey													
	//err_code = sd_ble_opt_set(BLE_GAP_OPT_PASSKEY, &ble_opt);	
	//APP_ERROR_CHECK(err_code);
}

/*****************************************************************************
 函数:  nus_data_handler
 输入:	p_nus:FIDO服务环境变量
				p_data：本次接收数据指针
				length	本次接收数据长度
 输出:	无
 		
 返回:	无
 		
 功能:
			服务初始化
******************************************************************************/
static void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
//	uint32_t err_code;
	
	g_timeout_shutdown = SHUTDOWN_TIME;
	Recivedataflag=1;						//have recive data,the flag been set

	#if 0
	memcpy(g_transfer_buff,p_data,length);
	
	ble_nus_string_send(&m_nus,g_transfer_buff, length);
	#endif

	phone_to_ble_data_parse(p_data,length);
				
#if 0
    for (uint32_t i = 0; i < length; i++)
    {
        do
        {
            err_code = app_uart_put(p_data[i]);
            if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_BUSY))
            {
                //NRF_LOG_ERROR("Failed receiving NUS message. Error 0x%x. \r\n", err_code);
                APP_ERROR_CHECK(err_code);
            }
        } while (err_code == NRF_ERROR_BUSY);
    }
    //if (p_data[length-1] == '\r')
    //{
        //while (app_uart_put('\n') == NRF_ERROR_BUSY);
    //}
#endif    
}
/**@snippet [Handling the data received over BLE] */

/*****************************************************************************
 函数:  services_init
 输入:	无
 输出:	无
 		
 返回:	无
 		
 功能:
			服务初始化
******************************************************************************/
static void services_init(void)
{
    uint32_t       err_code;
    ble_nus_init_t nus_init;
#if BLE_DIS_ENABLED
		ble_dis_init_t dis_init;
    // Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, (char *)MANUFACTURER_NAME); 
		ble_srv_ascii_to_utf8(&dis_init.fw_rev_str, (char *)FW_VERSION); 
	
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
#endif	
		//Battery Service
#if BLE_BAS_ENABLED
	bas_init();
#endif
    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = nus_data_handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}

/*****************************************************************************
 函数:  on_conn_params_evt
 输入:	p_evt：连接参数事件变量
 输出:	无
 		
 返回:	无
 		
 功能:
			连接参数事件
******************************************************************************/
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        //err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        //APP_ERROR_CHECK(err_code);
        
        // Don't disconnect yet, try to be more lenient and see ...
        ble_gap_conn_params_t   gap_conn_params;
  
        memset(&gap_conn_params, 0, sizeof(gap_conn_params));

        gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
        gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL+MSEC_TO_UNITS(10, UNIT_1_25_MS);
        gap_conn_params.slave_latency     = SLAVE_LATENCY;
        gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

        err_code = ble_conn_params_change_conn_params(&gap_conn_params);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/*****************************************************************************
 函数:  conn_params_init
 输入:	无
 输出:	无
 		
 返回:	无
 		
 功能:
			连接参数初始化
******************************************************************************/
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
//static void sleep_mode_enter(void)
//{
//    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);
//    APP_ERROR_CHECK(err_code);

//    // Prepare wakeup buttons.
//    err_code = bsp_btn_ble_sleep_mode_prepare();
//    APP_ERROR_CHECK(err_code);

//    // Go to system-off mode (this function will not return; wakeup will cause a reset).
//    err_code = sd_power_system_off();
//    APP_ERROR_CHECK(err_code);
//}

/*****************************************************************************
 函数:  on_adv_evt
 输入:	ble_adv_evt: 广播事件变量
 输出:	无
 		
 返回:	无
 		
 功能:
			BLE广播相关事件处理函数
******************************************************************************/
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
					
            break;
        case BLE_ADV_EVT_IDLE:
            //sleep_mode_enter();
						err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
						APP_ERROR_CHECK(err_code);
            break;
        default:
            break;
    }
}

/*****************************************************************************
 函数:  on_ble_evt
 输入:	p_ble_evt: BLE协议栈事件
 输出:	无
 		
 返回:	无
 		
 功能:
			BLE功能相关事件处理函数
******************************************************************************/
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code;
//		uint16_t  server_rx_mtu;
//		ble_gatts_value_t gatts_value;
//    uint8_t Pbuf[2] ;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
			err_code = sd_ble_gattc_exchange_mtu_request(m_conn_handle, /*client_rx_mtu*/NRF_BLE_GATT_MAX_MTU_SIZE);
			APP_ERROR_CHECK(err_code);

			// Start Security Request timer.
           	err_code = start_req_timer();
			APP_ERROR_CHECK(err_code);
            break; // BLE_GAP_EVT_CONNECTED

        case BLE_GAP_EVT_DISCONNECTED:
        	m_conn_handle = BLE_CONN_HANDLE_INVALID;
        	Ble_AuthFlag=BLE_AuthFail;
            break; // BLE_GAP_EVT_DISCONNECTED

         case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST:
        {
            ble_gap_data_length_params_t dl_params;

            // Clearing the struct will effectivly set members to @ref BLE_GAP_DATA_LENGTH_AUTO
            memset(&dl_params, 0, sizeof(ble_gap_data_length_params_t));
            err_code = sd_ble_gap_data_length_update(p_ble_evt->evt.gap_evt.conn_handle, &dl_params, NULL);
            APP_ERROR_CHECK(err_code);
        } break;
        #if 0
		case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:
			server_rx_mtu = p_ble_evt->evt.gattc_evt.params.exchange_mtu_rsp.server_rx_mtu;
			/* New ATT_MTU is now applied to GATT procedures for this connection */
			/*Note
			The SoftDevice sets ATT_MTU to the minimum of:
			- The Client RX MTU value from BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST, and
			- The Server RX MTU value.
			However, the SoftDevice never sets ATT_MTU lower than
			GATT_MTU_SIZE_DEFAULT.
			*/
			/* Store ATT_MTU for later use */
			g_usMTUSize = MIN( MAX(BLE_GATT_ATT_MTU_DEFAULT, server_rx_mtu), /*client_rx_mtu*/NRF_BLE_MAX_MTU_SIZE) - 3 ;

	    	// Initialize value struct.below is update char value
	    	Pbuf[0] = (uint8_t)(g_usMTUSize<<8);
			Pbuf[1] = (uint8_t)g_usMTUSize;
			memset(&gatts_value, 0, sizeof(gatts_value));
			gatts_value.len     = sizeof(uint16_t);
			gatts_value.offset  = 0;
			gatts_value.p_value = Pbuf;
			#if 0
			// Update database.
			err_code = sd_ble_gatts_value_set(m_nus.conn_handle,
																				m_nus.Control_Point_Length.value_handle,
																				&gatts_value);
			APP_ERROR_CHECK(err_code);
			#endif
				
			break;
		#endif
		
        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTC_EVT_TIMEOUT

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_TIMEOUT
            
		case BLE_GAP_EVT_PASSKEY_DISPLAY:
		{
			ParingDataDisFlag=1;
			memcpy(ParingDataBuf, p_ble_evt->evt.gap_evt.params.passkey_display.passkey, PASSKEY_LENGTH);
			// Don't send delayed Security Request if security procedure is already in progress.
			err_code = stop_req_timer();
			APP_ERROR_CHECK(err_code);
		}break; // BLE_GAP_EVT_PASSKEY_DISPLAY
		case BLE_GAP_EVT_AUTH_STATUS:
			if(p_ble_evt->evt.gap_evt.params.auth_status.auth_status != BLE_GAP_SEC_STATUS_SUCCESS)   
            {
                Ble_AuthFlag=BLE_AuthFail;							
                sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            }
            else
            {
                Ble_AuthFlag=BLE_AuthSuccess;				//authentication success
            }
			
			break;
        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
            break; // BLE_EVT_USER_MEM_REQUEST

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                               &auth_reply);
                    APP_ERROR_CHECK(err_code);
                }
            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST
#if 0        
#if (NRF_SD_BLE_API_VERSION >= 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                       NRF_BLE_MAX_MTU_SIZE);
            APP_ERROR_CHECK(err_code);
						
						server_rx_mtu = p_ble_evt->evt.gatts_evt.params.exchange_mtu_request.client_rx_mtu;
						g_usMTUSize = MIN( MAX(BLE_GATT_ATT_MTU_DEFAULT, server_rx_mtu), NRF_BLE_MAX_MTU_SIZE) - 3 ;

            break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif
#endif
        default:
            // No implementation needed.
            break;
    }
}

/*****************************************************************************
 函数:  ble_evt_dispatch
 输入:	p_ble_evt: 协议栈事件
 输出:	无
 		
 返回:	无
 		
 功能:
			系统事件调度函数当协议栈底层发送中断会传导到此函数
******************************************************************************/
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
	ble_conn_state_on_ble_evt(p_ble_evt);  
	pm_on_ble_evt(p_ble_evt);
	ble_nus_on_ble_evt(&m_nus, p_ble_evt);
#if BLE_BAS_ENABLED
	ble_bas_on_ble_evt(&m_bas, p_ble_evt);   
#endif	
	ble_conn_params_on_ble_evt(p_ble_evt);
	on_ble_evt(p_ble_evt);
	ble_advertising_on_ble_evt(p_ble_evt);
	nrf_ble_gatt_on_ble_evt(&m_gatt, p_ble_evt);
}

/*****************************************************************************
 函数:  sys_evt_dispatch
 输入:	sys_evt: 系统事件
 输出:	无
 		
 返回:	无
 		
 功能:
			系统事件调度函数当底层发送中断会传导到此函数
******************************************************************************/
static void sys_evt_dispatch(uint32_t sys_evt)
{
    // Dispatch the system event to the fstorage module, where it will be
    // dispatched to the Flash Data Storage (FDS) module.
    fs_sys_event_handler(sys_evt);

    // Dispatch to the Advertising module last, since it will check if there are any
    // pending flash operations in fstorage. Let fstorage process system events first,
    // so that it can report correctly to the Advertising module.
    ble_advertising_on_sys_evt(sys_evt);
}

/*****************************************************************************
 函数:  ble_stack_init
 输入:	无
 输出:	无
 		
 返回:	无
 		
 功能:
			协议栈初始化
******************************************************************************/
static void ble_stack_init(void)
{
    uint32_t err_code;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize SoftDevice.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = softdevice_app_ram_start_get(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Overwrite some of the default configurations for the BLE stack.
    ble_cfg_t ble_cfg;

    // Configure the maximum number of connections.
    memset(&ble_cfg, 0, sizeof(ble_cfg));
    ble_cfg.gap_cfg.role_count_cfg.periph_role_count  = BLE_GAP_ROLE_COUNT_PERIPH_DEFAULT;
    ble_cfg.gap_cfg.role_count_cfg.central_role_count = 0;
    ble_cfg.gap_cfg.role_count_cfg.central_sec_count  = 0;
    err_code = sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT, &ble_cfg, ram_start);
    APP_ERROR_CHECK(err_code);

    // Configure the maximum ATT MTU.
    memset(&ble_cfg, 0x00, sizeof(ble_cfg));
    ble_cfg.conn_cfg.conn_cfg_tag                 = CONN_CFG_TAG;
    ble_cfg.conn_cfg.params.gatt_conn_cfg.att_mtu = NRF_BLE_GATT_MAX_MTU_SIZE;
    err_code = sd_ble_cfg_set(BLE_CONN_CFG_GATT, &ble_cfg, ram_start);
    APP_ERROR_CHECK(err_code);

    // Configure the maximum event length.
    memset(&ble_cfg, 0x00, sizeof(ble_cfg));
    ble_cfg.conn_cfg.conn_cfg_tag                     = CONN_CFG_TAG;
    ble_cfg.conn_cfg.params.gap_conn_cfg.event_length = 320;
    ble_cfg.conn_cfg.params.gap_conn_cfg.conn_count   = BLE_GAP_CONN_COUNT_DEFAULT;
    err_code = sd_ble_cfg_set(BLE_CONN_CFG_GAP, &ble_cfg, ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = softdevice_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable DCDC
    err_code = sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
    APP_ERROR_CHECK(err_code);
		
    // Subscribe for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

		// Add large PDU end
    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);

}


/*****************************************************************************
 函数:  gatt_evt_handler
 输入:	p_gatt：gatt层环境变量
				p_evt：gatt事件变量从底层传出来
 输出:	无
 		
 返回:	无
 		
 功能:
			gatt事件处理函数，主要处理MTU
******************************************************************************/
void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, const nrf_ble_gatt_evt_t * p_evt)
{
    if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
        //m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
				g_usMTUSize =  p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        //NRF_LOG_INFO("Data len is set to 0x%X(%d)\r\n", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
    }
    //NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x\r\n", p_gatt->att_mtu_desired_central, p_gatt->att_mtu_desired_periph);
}


/*****************************************************************************
 函数:  gatt_init
 输入:	无
 输出:	无
 		
 返回:	无
 		
 功能:
			gatt层初始化函数
******************************************************************************/
void gatt_init(void)
{
#if 1
	ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
	APP_ERROR_CHECK(err_code);

#else
    ret_code_t err_code;

    err_code = nrf_ble_gatt_init(&m_gatt,gatt_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, BLE_GATT_ATT_MTU_DEFAULT);//64
    APP_ERROR_CHECK(err_code);
#endif    
}

#if 0
/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' '\n' (hex 0x0A) or if the string has reached the maximum data length.
 */
/**@snippet [Handling the data received over UART] */
void uart_event_handle(app_uart_evt_t * p_event)
{
    static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
    static uint8_t index = 0;
    uint32_t       err_code;

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            index++;

            if ((data_array[index - 1] == '\n') || (index >= (m_ble_nus_max_data_len)))
            {
                do
                {
                    err_code = ble_nus_string_send(&m_nus, data_array, index);
                    if ( (err_code != NRF_ERROR_INVALID_STATE) && (err_code != NRF_ERROR_BUSY) )
                    {
                        APP_ERROR_CHECK(err_code);
                    }
                } while (err_code == NRF_ERROR_BUSY);

                index = 0;
            }
            break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}
/**@snippet [Handling the data received over UART] */
#endif




/*****************************************************************************
 函数:  advertising_init
 输入:	无
 输出:	无
 		
 返回:	无
 		
 功能:
			广播初始化函数
******************************************************************************/
void advertising_init(void)
{
    uint32_t               err_code;
    ble_advdata_t          advdata;
    ble_adv_modes_config_t options;
	ble_advdata_t          scanrsp;
	
    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = false;
    advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    memset(&scanrsp, 0, sizeof(scanrsp));
    scanrsp.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    scanrsp.uuids_complete.p_uuids  = m_adv_uuids;
	
    memset(&options, 0, sizeof(options));
    options.ble_adv_fast_enabled  = true;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata,&scanrsp, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
		
	ble_advertising_conn_cfg_tag_set(CONN_CFG_TAG);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
#ifdef DPBOARD
static void buttons_leds_init(bool * p_erase_bonds)
{
    bsp_event_t startup_event;

    uint32_t err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}
#endif



///**@brief Function for placing the application in low power state while waiting for events.
// */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}
#if 0

/**@brief Function for initializing power management.
 */
void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}
#endif

/*****************************************************************************
 函数:  HardFault_Handler
 输入:	无
 输出:	无
 		
 返回:	无
 		
 功能:
			错误中断函数
******************************************************************************/
void HardFault_Handler(void)
{
		//reset chip
		NVIC_SystemReset();		
}

void main_loop(void)
{
	app_sched_event_put(NULL,NULL,Bluetooth_ReciveANDSend);
	app_sched_event_put(NULL,NULL,BleCmdProcess);
	app_sched_event_put(NULL,NULL,UartDataSendrecive);

	if((nrf_gpio_pin_read(CHARG_STA_PIN)==0)&&(USB_connectFLag==USBDisconnect))
	{
		if(chargstatustime_flag==0)
		{
			chargstatustime_flag=1;
			app_timer_start(chargestatus_time_id, _20ms_INTERVAL, NULL);
		}
	}	
		

	switch(Main_status)
	{
		case Main_status_closeoled:        app_sched_event_put(NULL,0, progam_closeoled);       
		break;
		case Main_status_changbatlowpower: app_sched_event_put(NULL,0, Chargingandmanagerdisplay);
		break;		 			
		case Main_status_timedisplay:      app_sched_event_put(NULL,0, timedisplay);
		break;			
		case Main_status_download:         app_sched_event_put(NULL,0, BlueUPfirmware);		
		break;		 		
		case Main_status_blekey:           app_sched_event_put(NULL,0, Bluetooyhkeydisplay);
		break;		
		case Main_status_ParingDis:        app_sched_event_put(NULL,0, Bluetooyparingdisplay);
		break;		
    	case Main_status_firmware:         app_sched_event_put(NULL,0, firmwaresigned);
		break;
	}

}
/*****************************************************************************
 函数:  main
 输入:	无
 输出:	无
 		
 返回:	无
 		
 功能:
			主入口函数
******************************************************************************/
int main(void)
{
	uint32_t err_code;
	bool erase_bonds = false;
	
	system_init();	
	
	//uart_init();

	//power_management_init();
	scheduler_init();
		
	ble_stack_init();	
	// if not write name&tk write
	uiSYS_ReadNameTKRecord();
	Mac_address_get();
	gap_params_init();
	gatt_init();
	services_init();
	advertising_init();
	conn_params_init();	
	peer_manager_init(erase_bonds);	
	err_code =fds_usr_init();
	usr_data_init();
	
	APP_ERROR_CHECK(err_code);
	vSYS_APPTimers_Start();
	err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
	APP_ERROR_CHECK(err_code);

	for(;;)
	{
		main_loop();
    	app_sched_execute();
		power_manage();
	}

}


/**
 * @}
 */
