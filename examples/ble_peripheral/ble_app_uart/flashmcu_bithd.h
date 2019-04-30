#ifndef _FLASHMCU_BITHD_H_
#define _FLASHMCU_BITHD_H_
#include "pstorage_platform.h"
#include "pstorage.h"
#include "nrf_error.h"

extern pstorage_handle_t       USEblock_handle;  
extern  pstorage_handle_t       base_handle;     
extern pstorage_module_param_t param;
extern unsigned char storage_app_point;
extern volatile unsigned char pstorage_wait_flag;    
void mcuflash_init(void);


#endif


