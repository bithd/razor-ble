#include <stdint.h>
#include "flashmcu_bithd.h"
#include "program.h"
#include "sys.h"
#include "pstorage_platform.h"
#include <string.h>
#include <fds.h>
#include "peer_manager.h"


pstorage_handle_t       USEblock_handle;  
pstorage_handle_t       base_handle;     
pstorage_module_param_t param;

volatile unsigned char pstorage_wait_flag=0; 

unsigned char storage_app_point=0;


fds_record_desc_t	g_record_desc={0};
fds_find_token_t	g_ftok ={0};//Important, make sure you zero init the ftok token


void uiSYS_ReadNameTKRecord(void)
{
	uint32_t i ;
	if (('U' == *(uint8_t *)NAME_ADDR)&&('2'==*((uint8_t *)NAME_ADDR+1)))
	{
		for(i=0;i<12;i++)
		{
			g_ucBtName[i] = *((uint8_t *)NAME_ADDR+i) ;
		}
		for(i=12;i<NAMETK_LEN;i++)
		{
			g_ucBleTK[i-12] = *((uint8_t *)NAME_ADDR+i) ;
		}	
	}
}

#if 0

uint32_t 		bak_buff[64];

void read_flash(void)
{
	unsigned char buf456[]={1,2,3,4,5,6,7,8};

	if(0==memcmp(buf456,&bak_buff[8],8))
	{
		memcpy((unsigned char*)(&coinbalance),bak_buff,64);
	}

}
#endif

///////////////////////////////////////////////////////////////

ret_code_t fds_test_find_and_delete (void)
{
	fds_record_desc_t   record_desc;

	g_ftok.page=0;
	g_ftok.p_addr=NULL;
	// Loop and find records with same ID and rec key and mark them as deleted. 
	while (fds_record_find(FILE_ID, REC_KEY, &g_record_desc, &g_ftok) == FDS_SUCCESS)
	{
		fds_record_delete(&record_desc);
	}
	// call the garbage collector to empty them, don't need to do this all the time, this is just for demonstration
	ret_code_t ret = fds_gc();
	if (ret != FDS_SUCCESS)
	{
			return ret;
	}
	return NRF_SUCCESS;
}

ret_code_t fds_test_write(uint8_t file_id, uint32_t *pdata, uint16_t len)
{
	fds_record_t        record;
	//fds_record_desc_t   record_desc;
	fds_record_chunk_t  record_chunk;
	
	memset(&g_record_desc, 0, sizeof(g_record_desc));
	// Set up data.
	record_chunk.p_data         = pdata;
	record_chunk.length_words   = len;
	// Set up record.
	record.file_id              = FILE_ID+file_id;
	record.key              	= REC_KEY;
	record.data.p_chunks       	= &record_chunk;
	record.data.num_chunks   	= 1;
			
	ret_code_t ret = fds_record_write(&g_record_desc, &record);
	if (ret != FDS_SUCCESS)
	{
			return ret;
	}
	while (write_flag==0);
	
	return NRF_SUCCESS;
}

ret_code_t fds_read(uint8_t file_id, uint32_t *pdata, uint16_t *pwords)
{
	fds_flash_record_t  flash_record;
	uint8_t i=0;
	//fds_record_desc_t   record_desc;
	//fds_find_token_t    ftok ={0};//Important, make sure you zero init the ftok token
	uint32_t *data;
	uint32_t err_code;
	
	memset(&g_record_desc, 0, sizeof(g_record_desc));
	memset(&g_ftok, 0, sizeof(g_ftok));
	
	// Loop until all records with the given key and file ID have been found.
	while (fds_record_find(FILE_ID+file_id, REC_KEY, &g_record_desc, &g_ftok) == FDS_SUCCESS)
	{
		err_code = fds_record_open(&g_record_desc, &flash_record);
		if ( err_code != FDS_SUCCESS)
		{
			return err_code;		
		}
		
		data = (uint32_t *) flash_record.p_data;
		for (i=0;i<flash_record.p_header->tl.length_words;i++)
		{
			pdata[i]=data[i];
		}
		// Access the record through the flash_record structure.
		// Close the record when done.
		err_code = fds_record_close(&g_record_desc);
		if (err_code != FDS_SUCCESS)
		{
			return err_code;	
		}
	}
	*pwords = i;
	return NRF_SUCCESS;
		
}

static void my_fds_evt_handler(fds_evt_t const * const p_fds_evt)
{
    switch (p_fds_evt->id)
    {
        case FDS_EVT_INIT:
            if (p_fds_evt->result != FDS_SUCCESS)
            {
                // Initialization failed.
            }
            break;
		case FDS_EVT_WRITE:
			if (p_fds_evt->result == FDS_SUCCESS)
			{
				write_flag=1;
			}
			break;
        default:
            break;
    }
}

ret_code_t fds_usr_init(void)
{	
	ret_code_t ret = fds_register(my_fds_evt_handler);
	if (ret != FDS_SUCCESS)
	{
		return ret;
			
	}
	ret = fds_init();
	if (ret != FDS_SUCCESS)
	{
		return ret;
	}
	return NRF_SUCCESS;
		
}

#if 0
ret_code_t fds_test(void)
{
    uint32_t err_code;
		uint16_t len=0;
	uint8_t file_id=0;
	
	fds_usr_init();
	//err_code = fds_test_find_and_delete();
	//APP_ERROR_CHECK(err_code);
	
//	err_code =fds_test_write(file_id,data);
	APP_ERROR_CHECK(err_code);
	//wait until the write is finished. 
	while (write_flag==0);
	fds_read(file_id,bak_buff,&len);

	memcmp((void *)data,(void *)bak_buff,16);
}

#endif



















