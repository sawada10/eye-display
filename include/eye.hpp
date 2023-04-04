#include <math.h>
#include <functional>

#if defined(BOARD_TYPE_URUKATECH_001)

#include <Arduino.h>
#include <SPIFFS.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#elif defined(BOARD_TYPE_T_RGB)

#include <Arduino.h>
#include <SPIFFS.h>
#include <Arduino_GFX.h>
#include <Arduino_GFX_Library.h>
#include <JpegFunc.h>
#include <Wire.h>
#define GFX_EXTRA_PRE_INIT() Wire.begin(8 /* SDA */, 48 /* SCL */, 800000L /* speed */);
#define GFX_BL 46

#elif defined(BOARD_TYPE_M5STACK_CORE2)

#include <M5Core2.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#define LGFX_M5STACK_CORE2
#include <LGFX_AUTODETECT.hpp>

#endif

#if defined(BOARD_TYPE_URUKATECH_001)
class LGFX_M5Stamp_SPI_GC9A01 : public lgfx::LGFX_Device
{
  lgfx::Panel_GC9A01 _panel_instance;
  lgfx::Bus_SPI _bus_instance;  // SPIバスのインスタンス
  lgfx::Light_PWM _light_instance;

public:
  LGFX_M5Stamp_SPI_GC9A01(void)
  {
    {                                     // バス制御の設定を行います。
      auto cfg = _bus_instance.config();  // バス設定用の構造体を取得します。

      // SPIバスの設定
      cfg.spi_host =
          SPI2_HOST;  // 使用するSPIを選択  ESP32-S2,C3 : SPI2_HOST or SPI3_HOST / ESP32 : VSPI_HOST or HSPI_HOST
      // ※ ESP-IDFバージョンアップに伴い、VSPI_HOST ,
      // HSPI_HOSTの記述は非推奨になるため、エラーが出る場合は代わりにSPI2_HOST , SPI3_HOSTを使用してください。
      cfg.spi_mode = 0;  // SPI通信モードを設定 (0 ~ 3)
      cfg.freq_write = 40000000;  // 送信時のSPIクロック (最大80MHz, 80MHzを整数で割った値に丸められます)
      cfg.freq_read = 16000000;  // 受信時のSPIクロック
      cfg.spi_3wire = true;      // 受信をMOSIピンで行う場合はtrueを設定
      cfg.use_lock = true;       // トランザクションロックを使用する場合はtrueを設定
      cfg.dma_channel =
          SPI_DMA_CH_AUTO;  // 使用するDMAチャンネルを設定 (0=DMA不使用 / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=自動設定)
      // ※
      // ESP-IDFバージョンアップに伴い、DMAチャンネルはSPI_DMA_CH_AUTO(自動設定)が推奨になりました。1ch,2chの指定は非推奨になります。
      cfg.pin_sclk = 4;   // SPIのSCLKピン番号を設定
      cfg.pin_mosi = 6;   // SPIのMOSIピン番号を設定
      cfg.pin_miso = -1;  // SPIのMISOピン番号を設定 (-1 = disable)
      cfg.pin_dc = 1;     // SPIのD/Cピン番号を設定  (-1 = disable)
      // SDカードと共通のSPIバスを使う場合、MISOは省略せず必ず設定してください。
      _bus_instance.config(cfg);               // 設定値をバスに反映します。
      _panel_instance.setBus(&_bus_instance);  // バスをパネルにセットします。
    }

    {                                       // 表示パネル制御の設定を行います。
      auto cfg = _panel_instance.config();  // 表示パネル設定用の構造体を取得します。
      cfg.pin_cs = 7;                       // CSが接続されているピン番号   (-1 = disable)
      cfg.pin_rst = 0;                      // RSTが接続されているピン番号  (-1 = disable)
      cfg.pin_busy = -1;                    // BUSYが接続されているピン番号 (-1 = disable)
      // ※
      // 以下の設定値はパネル毎に一般的な初期値が設定されていますので、不明な項目はコメントアウトして試してみてください。
      cfg.memory_width = 240;    // ドライバICがサポートしている最大の幅
      cfg.memory_height = 240;   // ドライバICがサポートしている最大の高さ
      cfg.panel_width = 240;     // 実際に表示可能な幅
      cfg.panel_height = 240;    // 実際に表示可能な高さ
      cfg.offset_x = 0;          // パネルのX方向オフセット量
      cfg.offset_y = 0;          // パネルのY方向オフセット量
      cfg.offset_rotation = 0;   // 回転方向の値のオフセット 0~7 (4~7は上下反転)
      cfg.dummy_read_pixel = 8;  // ピクセル読出し前のダミーリードのビット数
      cfg.dummy_read_bits = 1;   // ピクセル以外のデータ読出し前のダミーリードのビット数
      cfg.readable = true;       // データ読出しが可能な場合 trueに設定
      cfg.invert = true;         // パネルの明暗が反転してしまう場合 trueに設定
      cfg.rgb_order = false;     // パネルの赤と青が入れ替わってしまう場合 trueに設定
      cfg.dlen_16bit = false;    // データ長を16bit単位で送信するパネルの場合 trueに設定
      cfg.bus_shared = true;  // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

      _panel_instance.config(cfg);
    }
    {  // バックライト制御の設定を行います。（必要なければ削除）
      auto cfg = _light_instance.config();  // バックライト設定用の構造体を取得します。

      cfg.pin_bl = 10;      // バックライトが接続されているピン番号
      cfg.invert = false;   // バックライトの輝度を反転させる場合 true
      cfg.freq = 44100;     // バックライトのPWM周波数
      cfg.pwm_channel = 3;  // 使用するPWMのチャンネル番号

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  // バックライトをパネルにセットします。
    }
    setPanel(&_panel_instance);  // 使用するパネルをセットします。
  }
};
#elif defined(BOARD_TYPE_T_RGB)
Arduino_DataBus* bus;
Arduino_ESP32RGBPanel* rgbpanel;
Arduino_RGB_Display* gfx;
Arduino_Canvas* canvas_eye;
Arduino_Canvas* canvas_outline;
Arduino_Canvas* canvas_pupil;
Arduino_Canvas* canvas_reflex;

int jpegDrawCallback(Arduino_Canvas* canvas, JPEGDRAW* pDraw)
{
  canvas->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

int jpegOutlineDrawCallback(JPEGDRAW* pDraw)
{
  return jpegDrawCallback(canvas_outline, pDraw);
}

int jpegPupilDrawCallback(JPEGDRAW* pDraw)
{
  return jpegDrawCallback(canvas_pupil, pDraw);
}

int jpegReflexDrawCallback(JPEGDRAW* pDraw)
{
  return jpegDrawCallback(canvas_reflex, pDraw);
}
#endif

class Eye
{
private:
#if defined(BOARD_TYPE_URUKATECH_001)
  LGFX_M5Stamp_SPI_GC9A01 lcd;
#elif defined(BOARD_TYPE_M5STACK_CORE2)
  LGFX lcd;
#endif

#if defined(BOARD_TYPE_URUKATEC_001) || defined(BOARD_TYPE_M5STACK_CORE2)
  LGFX_Sprite sprite_eye;
  LGFX_Sprite sprite_outline;
  LGFX_Sprite sprite_pupil;
  LGFX_Sprite sprite_reflex;
#endif

  float zoom_ratio;

  int image_width;
  int image_height;

public:
  void init(const char* path_jpg_outline, const char* path_jpg_pupil, const char* path_jpg_reflex,
            const int image_width, const int image_height, int rotation = 0)
  {
    this->image_width = image_width;
    this->image_height = image_height;

#if defined(BOARD_TYPE_URUKATEC_001) || defined(BOARD_TYPE_M5STACK_CORE2)
    lcd.init();
    lcd.setRotation(rotation);

    sprite_eye.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);

    sprite_outline.createSprite(image_width, image_height);
    sprite_outline.fillScreen(TFT_WHITE);
    sprite_outline.drawJpgFile(SPIFFS, path_jpg_outline);
    sprite_pupil.createSprite(image_width, image_height);
    sprite_pupil.fillScreen(TFT_WHITE);
    sprite_pupil.drawJpgFile(SPIFFS, path_jpg_pupil);
    sprite_reflex.createSprite(image_width, image_height);
    sprite_reflex.fillScreen(TFT_WHITE);
    sprite_reflex.drawJpgFile(SPIFFS, path_jpg_reflex);

    lcd.setPivot(lcd.width() >> 1, lcd.height() >> 1);
    lcd.fillScreen(TFT_WHITE);

    zoom_ratio = (float)lcd.width() / image_width;
    float ztmp = (float)lcd.height() / image_height;
    if (zoom_ratio > ztmp)
    {
      zoom_ratio = ztmp;
    }
#elif defined(BOARD_TYPE_T_RGB)
    bus = new Arduino_XL9535SWSPI(8 /* SDA */, 48 /* SCL */, 2 /* XL PWD */, 3 /* XL CS */, 5 /* XL SCK */,
                                  4 /* XL MOSI */);
    rgbpanel = new Arduino_ESP32RGBPanel(
        45 /* DE */, 41 /* VSYNC */, 47 /* HSYNC */, 42 /* PCLK */, 21 /* R0 */, 18 /* R1 */, 17 /* R2 */, 16 /* R3 */,
        15 /* R4 */, 14 /* G0 */, 13 /* G1 */, 12 /* G2 */, 11 /* G3 */, 10 /* G4 */, 9 /* G5 */, 7 /* B0 */,
        6 /* B1 */, 5 /* B2 */, 3 /* B3 */, 2 /* B4 */, 1 /* hsync_polarity */, 50 /* hsync_front_porch */,
        1 /* hsync_pulse_width */, 30 /* hsync_back_porch */, 1 /* vsync_polarity */, 20 /* vsync_front_porch */,
        1 /* vsync_pulse_width */, 30 /* vsync_back_porch */, 1 /* pclk_active_neg */);
    gfx = new Arduino_RGB_Display(480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
                                  bus, GFX_NOT_DEFINED /* RST */, st7701_type4_init_operations,
                                  sizeof(st7701_type4_init_operations));

    gfx->begin();

    canvas_eye = new Arduino_Canvas(image_width, image_height, gfx);
    canvas_eye->fillScreen(WHITE);

    canvas_outline = new Arduino_Canvas(image_width, image_height, canvas_eye);
    canvas_outline->fillScreen(WHITE);
    jpegDraw(path_jpg_outline, jpegOutlineDrawCallback, true, 0, 0, image_width, image_height);
    canvas_pupil = new Arduino_Canvas(image_width, image_height, canvas_eye);
    canvas_pupil->fillScreen(WHITE);
    jpegDraw(path_jpg_pupil, jpegPupilDrawCallback, true, 0, 0, image_width, image_height);
    canvas_reflex = new Arduino_Canvas(image_width, image_height, canvas_eye);
    canvas_reflex->fillScreen(WHITE);
    jpegDraw(path_jpg_reflex, jpegReflexDrawCallback, true, 0, 0, image_width, image_height);

    zoom_ratio = (float)gfx->width() / image_width;
    float ztmp = (float)gfx->height() / image_height;
    if (zoom_ratio > ztmp)
    {
      zoom_ratio = ztmp;
    }
#endif
  }

  void update_look(float dx = 0.0, float dy = 0.0, float scale = 10.0, float random_scale = 5.0)
  {
    long rx = (int)(random_scale * random(100) / 100);
    long ry = (int)(random_scale * random(100) / 100);
#if defined(BOARD_TYPE_URUKATEC_001) || defined(BOARD_TYPE_M5STACK_CORE2)
    sprite_eye.fillScreen(TFT_WHITE);
    sprite_outline.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
    sprite_pupil.pushSprite(&sprite_eye, (int)(scale * dx), (int)(scale * dy), TFT_WHITE);
    sprite_reflex.pushSprite(&sprite_eye, (int)(scale * dx) + rx, (int)(scale * dy) + ry, TFT_WHITE);
    sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
#elif defined(BOARD_TYPE_T_RGB)
    canvas_eye->fillScreen(WHITE);
    canvas_outline->flush();
    canvas_pupil->flush();
    canvas_reflex->flush();
#endif
  }
};