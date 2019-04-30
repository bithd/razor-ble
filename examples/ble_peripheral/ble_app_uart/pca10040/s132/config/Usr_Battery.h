#ifndef USR_BATTERY_H_
#define USR_BATTERY_H_
#include "sdk_config.h"
#include "ble_bas.h"
#include "app_timer.h"

#if BLE_BAS_ENABLED

#define BLE_UUID_BAS_SERVICE							0x180F			


#define BATTERY_LEVEL_MEAS_INTERVAL       APP_TIMER_TICKS(3000) /**< Battery level measurement interval (ticks). This value corresponds to 10 seconds. */
#define BATTERY_LEVEL2_MEAS_INTERVAL       APP_TIMER_TICKS(100) /**< Battery level measurement interval (ticks). This value corresponds to 10 seconds. */

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS     600                                          /**< Reference voltage (in milli volts) used by ADC while doing conversion. */
#define ADC_PRE_SCALING_COMPENSATION      6                                            /**< The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.*/
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS    270                                          /**< Typical forward voltage drop of the diode (Part no: SD103ATW-7-F) that is connected in series with the voltage supply. This is the voltage drop when the forward current is 1mA. Source: Data sheet of 'SURFACE MOUNT SCHOTTKY BARRIER DIODE ARRAY' available at www.diodes.com. */

#define ADC_REF_VBG_VOLTAGE_IN_MILLIVOLTS 1200                                         /**< Value in millivolts for voltage used as reference in ADC conversion on NRF51. */
#define ADC_INPUT_PRESCALER               3                                            /**< Input prescaler for ADC convestion on NRF51. */
#define ADC_RES_10BIT                     1024                                         /**< Maximum digital value for 10-bit ADC conversion. */

/**@brief Macro to convert the result of ADC conversion in millivolts.
 *
 * @param[in]  ADC_VALUE   ADC result.
 *
 * @retval     Result converted to millivolts.
 */
#ifdef ADC_PRESENT  //51
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
        ((((ADC_VALUE) * ADC_REF_VBG_VOLTAGE_IN_MILLIVOLTS) / ADC_RES_10BIT) * ADC_INPUT_PRESCALER)
#else // SAADC_PRESENT  //52
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
        ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / ADC_RES_10BIT) * ADC_PRE_SCALING_COMPENSATION)
#endif // ADC_PRESENT

extern ble_bas_t 									m_bas;  

void battery_adc_configure(void);

void adc_configure(void);
void bas_init(void);
void battery_level_meas_timeout_handler(void * p_context);

#endif

#endif
