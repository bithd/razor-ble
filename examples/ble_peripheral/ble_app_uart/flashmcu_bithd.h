#ifndef _FLASHMCU_BITHD_H_
#define _FLASHMCU_BITHD_H_
#include "pstorage_platform.h"
#include "pstorage.h"
#include "nrf_error.h"
#include "sdk_errors.h"

#define adress PSTORAGE_DATA_START_ADDR
//name&tk record id
//#define COIN_FILE_ID     0x1111
//#define BALLANCE_FILE_ID     0x1112
//#define FLAG_FILE_ID     0x1113

#define COIN_REC_KEY     0x0022

#define COIN_FILE_ID     	0x1111
#define BALLANCE_FILE_ID    0x1116
#define FLAG_FILE_ID     	0x1117

#define FILE_ID 	0x1111
#define REC_KEY 	0x2222
#define REC_KEY2 	0x2223
#define REC_KEY3 	0x2224

#define REC_KEY     		0x2222

typedef struct
{
    uint16_t              length_words; /**< @brief The length of the data in words. */
    uint16_t     					data_id;      /**< @brief ID that specifies the type of data (defines which member of the union is used). */
    uint8_t               * p_application_data;        /**< @brief Arbitrary data to associate with the peer. This data can be freely used by the application. */
} pm_usr_data_t;


extern uint8_t g_ucRecordOpComplete ;
extern pstorage_handle_t       USEblock_handle;  
extern  pstorage_handle_t       base_handle;     
extern pstorage_module_param_t param;
extern unsigned char storage_app_point;
extern volatile unsigned char pstorage_wait_flag;   
extern ret_code_t fds_test_find_and_delete (void);
extern ret_code_t fds_test_write(uint8_t file_id, uint32_t *pdata, uint16_t len);
extern ret_code_t fds_read(uint8_t file_id,uint32_t *pdata,uint16_t *plen);
//extern ret_code_t fds_test(void);
extern void mcuflash_init(void);
extern void read_flash(void);
extern ret_code_t fds_usr_init(void);
extern uint32_t  fs_usr_init(void);
extern void usr_data_init(void);
extern uint32_t usr_flag_save(void);

#endif


