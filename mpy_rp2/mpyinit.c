#include "py/dynruntime.h"
#include "../src/JPEGDEC.h"
//#define MICROPY_HW_BOARD_NAME pico2
#include "../src/jpeg.inl"
//#include "jpegdec.c"

JPEGIMAGE info_jpeg;

void JPEGdummy()
{
	readFLASH(NULL,NULL,0);
}
#if !defined(__linux__)
void *memset(void *s, int c, size_t n) {
    return mp_fun_table.memset_(s, c, n);
}

void *memcpy(void *d, const void *s, unsigned int n) {
    return mp_fun_table.memmove_(d, s, n);
}
#endif
uint16_t *framebuffer;
int self_height, self_width;

static int JPEGDraw(JPEGDRAW *pDraw) {
	#if 0
    uint16_t *pBuf1 = pDraw->pPixels;
 	mp_int_t x = pDraw->x;
    mp_int_t y = pDraw->y;
    mp_int_t w = pDraw->iWidth;
    mp_int_t h = pDraw->iHeight;
 
    int wmax = 320;//_jpeg.iWidth;
	
	for(int yy = 0; yy < h; yy++)
	{
		int dest = x + (y + yy) * wmax;
		int src = yy * w ;
		memcpy((void *)(framebuffer + dest), (void *)(pBuf1 + src), w * 2);
	}
	#endif
	return 1;
}

static void stjpgdec_init(JPEGIMAGE *pJpeg, int iDataSize, uint8_t *pData, JPEG_DRAW_CALLBACK func) {
    memset((void *)pJpeg, 0, sizeof(JPEGIMAGE));
    pJpeg->ucMemType = JPEG_MEM_RAM;
    pJpeg->pfnRead = readRAM;
    pJpeg->pfnSeek = seekMem;
    pJpeg->pfnDraw = func;
    pJpeg->pfnOpen = NULL;
    pJpeg->pfnClose = NULL;
//	pJpeg->pUser = self;
//   	mp_printf(MP_PYTHON_PRINTER, "pJpeg->pUser(%08x)\n", (int)self);
    pJpeg->iError = 1111;
    pJpeg->JPEGFile.iSize = iDataSize;
    pJpeg->JPEGFile.pData = pData;
	pJpeg->ucPixelType = RGB565_BIG_ENDIAN;

    pJpeg->iMaxMCUs = 1000; // set to an unnaturally high value to start
//    result = JPEGInit(pJpeg);
}
#if 0
static mp_obj_t st7789_ST7789_jpgdec_decodex2(size_t n_args, const mp_obj_t *args) {
    st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    int result;
    mp_buffer_info_t inbuf;

    mp_get_buffer_raise(args[1], &inbuf, MP_BUFFER_READ);
    int iDataSize = inbuf.len;
    uint8_t *pData = (uint8_t *)inbuf.buf;

	stjpgdec_init(self, &_jpeg, iDataSize, pData, JPEGDrawx2);
    result = JPEGInit(&_jpeg);
    if (result == 1) {
	    _jpeg.iXOffset = 0;
	    _jpeg.iYOffset = 0;
	    _jpeg.iOptions = 0;
	    _jpeg.ucPixelType = RGB565_BIG_ENDIAN;
		JPEG_setCropArea(&_jpeg, 0, 0, self->width / 2, self->height / 2);
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
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(st7789_ST7789_jpgdec_decodex2_obj, 2, 2, st7789_ST7789_jpgdec_decodex2);
#endif

static mp_obj_t jpgdec_decode(size_t n_args, const mp_obj_t *args) {
    int result;
    mp_buffer_info_t inbuf, drawbuf;

    mp_get_buffer_raise(args[0], &inbuf, MP_BUFFER_READ);
    int iDataSize = inbuf.len;
    uint8_t *pData = (uint8_t *)inbuf.buf;

	mp_get_buffer_raise(args[1], &drawbuf, MP_BUFFER_READ);
    framebuffer = (uint16_t *)drawbuf.buf;

	stjpgdec_init(&info_jpeg, iDataSize, pData, JPEGDraw);
    result = JPEGInit(&info_jpeg);
    if (result == 1) {
    	if( info_jpeg.iWidth * info_jpeg.iHeight * 2 <= drawbuf.len) {
		    info_jpeg.iXOffset = 0;
		    info_jpeg.iYOffset = 0;
		    info_jpeg.iOptions = 0;
		    info_jpeg.ucPixelType = RGB565_BIG_ENDIAN;
	        JPEG_setFramebuffer(&info_jpeg, framebuffer);
		    result = DecodeJPEG(&info_jpeg);
    	} else {
		    result = 0;
    	}
	}
        mp_obj_t res[3] = {
            mp_obj_new_int(result),
            mp_obj_new_int(info_jpeg.iWidth),
            mp_obj_new_int(info_jpeg.iHeight)
        };

        return mp_obj_new_tuple(3, res);
    // if( result == 0) result = _jpeg.iError;
    // return mp_obj_new_int(result);

}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(jpgdec_decode_obj, 2, 2, jpgdec_decode);
#if 0
static mp_obj_t st7789_ST7789_jpgdec_decode_mt(size_t n_args, const mp_obj_t *args) {
    st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(args[0]);
	JPEGIMAGE *pInfo;
	pInfo = (JPEGIMAGE *)m_malloc(sizeof(JPEGIMAGE));
	
    int x = mp_obj_get_int(args[1]);
    int y = mp_obj_get_int(args[2]);
    int w = mp_obj_get_int(args[3]);
    int h = mp_obj_get_int(args[4]);
    int result;
	
	memcpy(pInfo, &_jpeg, sizeof(JPEGIMAGE));
	JPEG_setCropArea(pInfo, 0, 0, w, h);
	pInfo->pfnDraw = JPEGDraw_mt;
	pInfo->pUser = self;
    pInfo->iXOffset = x;
    pInfo->iYOffset = y;
    result = DecodeJPEG(pInfo);
	m_free(pInfo);
	return mp_obj_new_int(result);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(st7789_ST7789_jpgdec_decode_mt_obj, 5, 5, st7789_ST7789_jpgdec_decode_mt);

#endif

static mp_obj_t jpgdec_init(size_t n_args, const mp_obj_t *args) {

   int result;
    mp_buffer_info_t inbuf;
    mp_get_buffer_raise(args[0], &inbuf, MP_BUFFER_READ);
    int iDataSize = inbuf.len;
    uint8_t *pData = (uint8_t *)inbuf.buf;
	stjpgdec_init(&info_jpeg, iDataSize, pData, JPEGDraw);
	result = JPEGInit(&info_jpeg);
    mp_obj_t res[3];
	res[0]= mp_obj_new_int(result);
    res[1] = mp_obj_new_int(info_jpeg.iWidth);
	res[2] = mp_obj_new_int(info_jpeg.iHeight);
    
	return mp_obj_new_tuple(3, res);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(jpgdec_init_obj, 1, 1, jpgdec_init);


static mp_obj_t jpgdec_getinfo(size_t n_args, const mp_obj_t *args) {

   int result;
    mp_buffer_info_t inbuf;
    mp_get_buffer_raise(args[0], &inbuf, MP_BUFFER_READ);
    int iDataSize = inbuf.len;
    uint8_t *pData = (uint8_t *)inbuf.buf;
	{
	    mp_obj_t res[5];
		res[0] = mp_obj_new_int(8);
		res[1] = mp_obj_new_int((int)(pData));
		res[2] = mp_obj_new_int((int)(&info_jpeg));
		res[3] = mp_obj_new_int((int)(&jpgdec_getinfo));
		return mp_obj_new_tuple(4, res);
	}
	stjpgdec_init(&info_jpeg, iDataSize, pData, JPEGDraw);
	if (pData[12] == 0) {
	    mp_obj_t res[5];
		res[0] = mp_obj_new_int(9);
		res[1] = mp_obj_new_int((int)(pData));
		res[2] = mp_obj_new_int((int)(&info_jpeg));
		return mp_obj_new_tuple(3, res);
	}
	result = JPEGInit(&info_jpeg);
    mp_obj_t res[5];
	res[0] = mp_obj_new_int(result);
    res[1] = mp_obj_new_int(info_jpeg.iWidth);
	res[2] = mp_obj_new_int(info_jpeg.iHeight);
	res[3] = mp_obj_new_int(info_jpeg.iError);
	res[4] = mp_obj_new_int((int)(info_jpeg.pFramebuffer));
	return mp_obj_new_tuple(5, res);

	
#if 0
	int iXOffset, iYOffset; // placement on the display
    int iCropX, iCropY, iCropCX, iCropCY; // crop area
    uint8_t ucBpp, ucSubSample, ucHuffTableUsed;
    uint8_t ucMode, ucOrientation, ucHasThumb, b11Bit;
    uint8_t ucComponentsInScan, cApproxBitsLow, cApproxBitsHigh;
    uint8_t iScanStart, iScanEnd, ucFF, ucNumComponents;
    uint8_t ucACTable, ucDCTable;
    uint8_t ucMemType, ucPixelType;
    uint16_t u16MCUFlags;
    int iEXIF; // Offset to EXIF 'TIFF' file
    int iError;
    int iOptions;
    int iVLCOff; // current VLC data offset
    int iVLCSize; // current quantity of data in the VLC buffer
    int iResInterval, iResCount; // restart interval
    int iMaxMCUs; // max MCUs of pixels per JPEGDraw call
    JPEG_READ_CALLBACK *pfnRead;
    JPEG_SEEK_CALLBACK *pfnSeek;
    JPEG_DRAW_CALLBACK *pfnDraw;
    JPEG_OPEN_CALLBACK *pfnOpen;
    JPEG_CLOSE_CALLBACK *pfnClose;
    JPEGCOMPINFO JPCI[MAX_COMPS_IN_SCAN]; /* Max color components */
    JPEGFILE JPEGFile;
    BUFFERED_BITS bb;
    void *pUser;
    uint8_t *pDitherBuffer; // provided externally to do Floyd-Steinberg dithering
    uint16_t *usPixels; // needs to be 16-byte aligned for S3 SIMD
    uint16_t usUnalignedPixels[MAX_BUFFERED_PIXELS+8];
    int16_t *sMCUs; // needs to be 16-byte aligned for S3 SIMD
    int16_t sUnalignedMCUs[8+(DCTSIZE * MAX_MCU_COUNT)]; // 4:2:0 needs 6 DCT blocks per MCU
    void *pFramebuffer;
    int16_t sQuantTable[DCTSIZE*4]; // quantization tables
    uint8_t ucFileBuf[JPEG_FILE_BUF_SIZE]; // holds temp data and pixel stack
    uint8_t ucHuffDC[DC_TABLE_SIZE * 2]; // up to 2 'short' tables
    uint16_t usHuffAC[HUFF11SIZE * 2];
#endif
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(jpgdec_getinfo_obj, 1, 1, jpgdec_getinfo);


mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    mp_store_global(MP_QSTR_jpgdec_init, MP_OBJ_FROM_PTR(&jpgdec_init_obj));
    mp_store_global(MP_QSTR_jpgdec_decode, MP_OBJ_FROM_PTR(&jpgdec_decode_obj));
    mp_store_global(MP_QSTR_jpgdec_getinfo, MP_OBJ_FROM_PTR(&jpgdec_getinfo_obj));

    MP_DYNRUNTIME_INIT_EXIT
}
