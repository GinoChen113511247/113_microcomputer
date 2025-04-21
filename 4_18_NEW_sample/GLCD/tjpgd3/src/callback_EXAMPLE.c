#include "tjpgd.h"
#include <stdio.h>

typedef size_t (*CALLBACK_for_JPG_IN) (JDEC* jd, uint8_t* buff, size_t nbyte);
CALLBACK_for_JPG_IN p_callback=0;//variable declaration
void register_JPG_callback (CALLBACK_for_JPG_IN p_func);
void register_JPG_callback (CALLBACK_for_JPG_IN p_func)
	{
		p_callback=p_func;
	}//register function
	
	
	
	
size_t in_func_ROM (    /* Returns number of bytes read (zero on error) */
    JDEC* jd,       /* Decompression object */
    uint8_t* buff,  /* Pointer to the read buffer (null to remove data) */
    size_t nbyte    /* Number of bytes to read/remove */
)
{
		uint8_t* s = (uint8_t*) jd->device;
		uint8_t* d = buff;
		uint32_t cnt = nbyte;
		if (buff) { /* Raad data from input stream */
 //       return fread(buff, 1, nbyte, dev->fp);
				while (cnt--){
				*d++ = *s++;
				}
				jd->device = s;
				return nbyte;
    } else {    /* Remove data from input stream */
//        return fseek(dev->fp, nbyte, SEEK_CUR) ? 0 : nbyte;
				s = s + nbyte;
				jd->device = s;
				return nbyte;
    }
}


size_t in_func_FILE (    /* Returns number of bytes read (zero on error) */
    JDEC* jd,       /* Decompression object */
    uint8_t* buff,  /* Pointer to the read buffer (null to remove data) */
    size_t nbyte    /* Number of bytes to read/remove */
)
{
    FILE *dev_fp = (FILE*)jd->device;   /* Session identifier (5th argument of jd_prepare function) */


    if (buff) { /* Raad data from imput stream */
        return fread(buff, 1, nbyte, dev_fp);
    } else {    /* Remove data from input stream */
        return fseek(dev_fp, nbyte, SEEK_CUR) ? 0 : nbyte;
    }
}
