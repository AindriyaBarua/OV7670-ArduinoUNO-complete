
// set EXAMPLE to EXAMPLE_TFT_BUFFERED in setup.h to activate
#include "setup.h"
#if EXAMPLE == 4



#include "Arduino.h"
#include "Adafruit_ST7735_mod.h"
#include <BufferedCameraOV7670_QQVGA_20hz_Grayscale.h>
#include "GrayScaleTable.h"


BufferedCameraOV7670_QQVGA_20hz_Grayscale camera;



int TFT_RST = 10;
int TFT_CS  = 9;
int TFT_DC  = 8;
// TFT_SPI_clock = 13 and TFT_SPI_data = 11
Adafruit_ST7735_mod tft = Adafruit_ST7735_mod(TFT_CS, TFT_DC, TFT_RST);



// this is called in Arduino setup() function
void initializeScreenAndCamera() {
  bool cameraInitialized = camera.init();
  tft.initR(INITR_BLACKTAB);
  if (cameraInitialized) {
    // flash green screen if camera setup was successful
    tft.fillScreen(ST7735_GREEN);
    delay(1000);
    tft.fillScreen(ST7735_BLACK);
  } else {
    // red screen if failed to connect to camera
    tft.fillScreen(ST7735_RED);
    delay(3000);
  }

  TIMSK0 = 0; // disable "millis" timer interrupt
}





inline void sendLineToDisplay() __attribute__((always_inline));
inline void screenLineStart(void) __attribute__((always_inline));
inline void screenLineEnd(void) __attribute__((always_inline));
inline void sendPixelByte(uint8_t byte) __attribute__((always_inline));


// Normally it is a portrait screen. Use it as landscape
uint8_t screen_w = ST7735_TFTHEIGHT_18;
uint8_t screen_h = ST7735_TFTWIDTH;
uint8_t screenLineIndex;



bool alternateLine = true;

// this is called in Arduino loop() function
void processFrame() {
  screenLineIndex = screen_h;

  camera.waitForVsync();

  if (alternateLine) {
    screenLineIndex--;
    camera.readLine();
  }
  alternateLine = !alternateLine;

  for (uint8_t i = 0; i < camera.getLineCount() / 2; i++) {
    camera.readLine();
    sendLineToDisplay();
  }
}





void sendLineToDisplay() {
  if (screenLineIndex > 0) {

    screenLineStart();

    for (uint16_t i=0; i<camera.getLineLength(); i++) {
      sendPixelByte(graysScaleTableHigh[camera.getPixelByte(i)]);
      sendPixelByte(graysScaleTableLow[camera.getPixelByte(i)]);
    }
    screenLineEnd();
  }
}


void screenLineStart()   {
  if (screenLineIndex > 0) screenLineIndex-=2;
  tft.startAddrWindow(screenLineIndex, 0, screenLineIndex, screen_w-1);
}

void screenLineEnd() {
  tft.endAddrWindow();
}


void sendPixelByte(uint8_t byte) {
  SPDR = byte;

  // this must be adjusted if sending loop has more/less instructions

  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");

  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");

}


#endif
