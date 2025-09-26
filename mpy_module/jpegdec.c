#include "../src/JPEGDEC.h"
#include "../src/jpeg.inl"

void JPEGUpdate(uint16_t *pBuf, uint16_t x,  uint16_t y,  uint16_t w,  uint16_t h);

static JPEGIMAGE _jpeg;
void *JPEGdummy = {readFLASH};
static uint16_t *framebuffer;
static uint16_t disp_height, disp_width;

static int JPEGDraw(JPEGDRAW *pDraw) {
    uint16_t *pBuf1 = pDraw->pPixels;
 	mp_int_t x = pDraw->x;
    mp_int_t y = pDraw->y;
    mp_int_t w = pDraw->iWidth;
    mp_int_t h = pDraw->iHeight;
 
    JPEGUpdate(pBuf1, x, y, w, h);

	return 1;
}


static int JPEGDrawx2(JPEGDRAW *pDraw) {
#if 0
    uint16_t *pBuf1 = pDraw->pPixels;
	mp_int_t x = pDraw->x;
    mp_int_t y = pDraw->y;
    mp_int_t w = pDraw->iWidth;
    mp_int_t h = pDraw->iHeight;
    uint16_t *pBuf2 = m_malloc(sizeof(uint16_t) * disp_width);

    mp_int_t x2, y2;
	x2 = MIN(disp_width, x * 2 + w * 2);
	y2 = MIN(disp_height, y * 2 + h * 2);

	int hmax = disp_height / 2;
	int wmax = disp_width / 2;
	hmax = MIN(hmax,  h);
	wmax = MIN(wmax,  w);
	uint16_t *src, *dst;
    for (int ln = 0; ln < hmax; ln++) {
		src = (uint16_t *)pBuf1 + w * ln;
		dst = (uint16_t *)pBuf2;
        for (int ro = 0; ro < wmax; ro++) {
			*dst++ = *src;
			*dst++ = *src++;
		}
	    const int buf_size = 4096;
		int limit = wmax * 2 * 2;	// width * 2(pixel) * 2(uint16)
	    int chunks = limit / buf_size;
	    int rest = limit % buf_size;
        for (int j = 0; j < 2; j++) {
		    int i = 0;
			const uint8_t *ptr = (const uint8_t *)pBuf2;
		    for (; i < chunks; i++) {
//		        write_spi(self->spi_obj, ptr + i * buf_size, buf_size);
		    }
		    if (rest) {
//		        write_spi(self->spi_obj, ptr + i * buf_size, rest);
		    }
		}
	}
    m_free(pBuf2);
#endif
    return 1;
}
void JPEGGetDisp(uint16_t *w, uint16_t *h);

static void st_jpegdec_init(JPEGIMAGE *pJpeg, int iDataSize, uint8_t *pData, JPEG_DRAW_CALLBACK func) {
	JPEGGetDisp(&disp_width, &disp_height);
    memset((void *)pJpeg, 0, sizeof(JPEGIMAGE));
    pJpeg->ucMemType = JPEG_MEM_RAM;
    pJpeg->pfnRead = readRAM;
    pJpeg->pfnSeek = seekMem;
    pJpeg->pfnDraw = func;
    pJpeg->pfnOpen = NULL;
    pJpeg->pfnClose = NULL;
	pJpeg->pUser = NULL;
//   	mp_printf(MP_PYTHON_PRINTER, "pJpeg->pUser(%08x)\n", (int)self);
    pJpeg->iError = 1111;
    pJpeg->JPEGFile.iSize = iDataSize;
    pJpeg->JPEGFile.pData = pData;
	pJpeg->ucPixelType = RGB565_BIG_ENDIAN;

    pJpeg->iMaxMCUs = 1000; // set to an unnaturally high value to start
//    result = JPEGInit(pJpeg);
}

static mp_obj_t jpegdec_decodex2(size_t n_args, const mp_obj_t *args) {
    int result;
    mp_buffer_info_t inbuf, drawbuf;

    mp_get_buffer_raise(args[0], &inbuf, MP_BUFFER_READ);
    int iDataSize = inbuf.len;
    uint8_t *pData = (uint8_t *)inbuf.buf;
	
	mp_get_buffer_raise(args[1], &drawbuf, MP_BUFFER_READ);
    framebuffer = (uint16_t *)drawbuf.buf;

	st_jpegdec_init(&_jpeg, iDataSize, pData, JPEGDrawx2);
    result = JPEGInit(&_jpeg);
    if (result == 1) {
	    _jpeg.iXOffset = 0;
	    _jpeg.iYOffset = 0;
	    _jpeg.iOptions = 0;
	    _jpeg.ucPixelType = RGB565_BIG_ENDIAN;
		JPEG_setCropArea(&_jpeg, 0, 0, disp_width / 2, disp_height / 2);
		result = DecodeJPEG(&_jpeg);
	}
    mp_obj_t res[3] = {
        mp_obj_new_int(result),
        mp_obj_new_int(_jpeg.iWidth),
        mp_obj_new_int(_jpeg.iHeight)
    };

    return mp_obj_new_tuple(3, res);
    // if( result == 0) result = _jpeg.iError;
    // return mp_obj_new_int(result);

}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(jpegdec_decodex2_obj, 2, 2, jpegdec_decodex2);

uint8_t JPEGGetMode();
void JPEGModeStart(uint8_t mode);
void JPEGModeEnd();
void JPEGSetDrawPage(uint8_t page);
void JPEGSetViewPage(uint8_t page);
uint8_t JPEGGetViewPage();

static mp_obj_t jpegdec_decode(size_t n_args, const mp_obj_t *args) {
    int result;
    mp_buffer_info_t inbuf, drawbuf;
	
    mp_get_buffer_raise(args[0], &inbuf, MP_BUFFER_READ);
    int iDataSize = inbuf.len;
    uint8_t *pData = (uint8_t *)inbuf.buf;
	
	mp_get_buffer_raise(args[1], &drawbuf, MP_BUFFER_READ);
//    framebuffer = (uint16_t *)drawbuf.buf;

	int drawmode = mp_obj_get_int(args[2]);	// mode 0:simple, 1:flip screen

	st_jpegdec_init(&_jpeg, iDataSize, pData, JPEGDraw);
    result = JPEGInit(&_jpeg);
    if (result == 1) {
//    	if( _jpeg.iWidth * _jpeg.iHeight * 2 <= drawbuf.len) {}
		    _jpeg.iXOffset = 0;
		    _jpeg.iYOffset = 0;
		    _jpeg.iOptions = JPEG_USES_DMA;
		    _jpeg.ucPixelType = RGB565_BIG_ENDIAN;
   			JPEG_setCropArea(&_jpeg, 0, 0, disp_width, disp_height);

		    //JPEG_setFramebuffer(&_jpeg, framebuffer);
    	if(drawmode == 0) {
    		JPEGModeStart(0);
		    result = DecodeJPEG(&_jpeg);
			JPEGModeEnd();
    	} else if( drawmode == 1) {
    		JPEGModeStart(1);
			uint8_t pageNum = JPEGGetViewPage();
			if( pageNum == 0 || pageNum == 2) {	// not initialized or page 2
				JPEGSetDrawPage(1);
			    result = DecodeJPEG(&_jpeg);
				JPEGSetViewPage(1);
			} else {
				JPEGSetDrawPage(2);
			    result = DecodeJPEG(&_jpeg);
				JPEGSetViewPage(2);
			}
			JPEGModeEnd();
    	}
    }
    mp_obj_t res[3] = {
        mp_obj_new_int(result),
        mp_obj_new_int(_jpeg.iWidth),
        mp_obj_new_int(_jpeg.iHeight)
    };

    return mp_obj_new_tuple(3, res);
    // if( result == 0) result = _jpeg.iError;
    // return mp_obj_new_int(result);

}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(jpegdec_decode_obj, 3, 3, jpegdec_decode);


static mp_obj_t jpegdec_getinfo(size_t n_args, const mp_obj_t *args) {
	int result;

    mp_buffer_info_t inbuf;
    mp_get_buffer_raise(args[0], &inbuf, MP_BUFFER_READ);
    int iDataSize = inbuf.len;
    uint8_t *pData = (uint8_t *)inbuf.buf;
	//mt_lock = -1;
	st_jpegdec_init(&_jpeg, iDataSize, pData, JPEGDraw);
	result = JPEGInit(&_jpeg);
    mp_obj_t res[4] = {
        mp_obj_new_int(result),
        mp_obj_new_int(_jpeg.iWidth),
        mp_obj_new_int(_jpeg.iHeight),
		mp_obj_new_int((int)(&_jpeg))
    };
    return mp_obj_new_tuple(3, res);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(jpegdec_getinfo_obj, 1, 1, jpegdec_getinfo);
#if 0
static mp_obj_t jpegdec_getdebuginfo(size_t n_args, const mp_obj_t *args) {
   int result;
    mp_buffer_info_t inbuf;
    mp_get_buffer_raise(args[0], &inbuf, MP_BUFFER_READ);
    int iDataSize = inbuf.len;
    uint8_t *pData = (uint8_t *)inbuf.buf;
	{
	    mp_obj_t res[5];
		res[0] = mp_obj_new_int(8);
		res[1] = mp_obj_new_int((int)(pData));
		res[2] = mp_obj_new_int((int)(&_jpeg));
		res[3] = mp_obj_new_int((int)(&jpegdec_getinfo));
		return mp_obj_new_tuple(4, res);
	}
	st_jpegdec_init(&_jpeg, iDataSize, pData, JPEGDraw);
	if (pData[12] == 0) {
	    mp_obj_t res[5];
		res[0] = mp_obj_new_int(9);
		res[1] = mp_obj_new_int((int)(pData));
		res[2] = mp_obj_new_int((int)(&_jpeg));
		return mp_obj_new_tuple(3, res);
	}
	result = JPEGInit(&_jpeg);
    mp_obj_t res[5];
	res[0] = mp_obj_new_int(result);
    res[1] = mp_obj_new_int(_jpeg.iWidth);
	res[2] = mp_obj_new_int(_jpeg.iHeight);
	res[3] = mp_obj_new_int(_jpeg.iError);
	res[4] = mp_obj_new_int((int)(_jpeg.pFramebuffer));
	return mp_obj_new_tuple(5, res);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(jpegdec_getinfo_obj, 1, 1, jpegdec_getinfo);
#endif 
static const mp_rom_map_elem_t jpegdec_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_jpegdec) },
//    {MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&jpegdec_init_obj)},
    {MP_ROM_QSTR(MP_QSTR_getinfo), MP_ROM_PTR(&jpegdec_getinfo_obj)},
    {MP_ROM_QSTR(MP_QSTR_decode), MP_ROM_PTR(&jpegdec_decode_obj)},
    {MP_ROM_QSTR(MP_QSTR_decodex2), MP_ROM_PTR(&jpegdec_decodex2_obj)},
};
static MP_DEFINE_CONST_DICT(jpegdec_globals, jpegdec_globals_table);
/* methods end */


const mp_obj_module_t jpegdec_cmodule = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&jpegdec_globals,
};

MP_REGISTER_MODULE(MP_QSTR_jpegdec, jpegdec_cmodule);
