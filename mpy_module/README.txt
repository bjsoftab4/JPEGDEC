関数仕様

ユーザー向けIF
jpegdec_getinfo(buf:bytearray)->result: tuple[int, int, int, int]
jpegdec_init(buf)->->result: tuple[int, int, int]
jpegdec_decode(buf, option, 

内部IF
static int JPEGDraw(JPEGDRAW *pDraw) {


PicoDisplay向けIF
void JPEGUpdate(uint16_t *pBuf, uint16_t x,  uint16_t y,  uint16_t w,  uint16_t h);
void JPEGSetDrawPage(uint8_t page);
void JPEGSetViewPage(uint8_t page);
uint8_t JPEGGetViewPage();
uint8_t JPEGGetPartial();
void JPEGSetPartial(uint8_t newmode);
void JPEGModeStart(uint8_t mode);
void JPEGModeEnd();

ｰｰ
改造残
make_new 対応
JPEGIMAGE を m_malloc で確保
サイズに対応したスケーリング
================================
PicoDisplay 内部追加
static void set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
static uint8_t Partialmode = 0;
static uint8_t JPEG_Drawpage = 0;
static uint8_t JPEG_Viewpage = 0;
static uint8_t JPEG_dma = 0;

uint8_t JPEGGetPartial() {
static void JPEGCleanVRAM() {
static void JPEGDmaWait(){



モード整理
