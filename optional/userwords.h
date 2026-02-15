/**
Connect ESP32forth to TFT_eSPI and LVGL libraries
   Filename:      userwords.h
   Date:          15 feb 2026
   Updated:       15 feb 2026
   File Version:  1.0
   MCU:           ESP32-S3 | ESP32 WROOM
   Forth:         ESP32forth all versions 7.0.7.21+
   Library        TFT_eSPI (by Bodmer)
   Copyright:     Marc PETREMANN
   Author:        Marc PETREMANN
   GNU General Public License
*/

#include <Arduino.h>
#include <TFT_eSPI.h>       // La bibliothèque graphique de base
// source: https://github.com/Bodmer/TFT_eSPI
#include <lvgl.h>           // La bibliothèque LVGL
// source: https://github.com/lvgl/lvgl/blob/master/lvgl.h 

// On s'assure que l'objet tft est accessible
extern TFT_eSPI tft;

#define ST7735_DRIVER

// --- Variables LVGL v9 ---
static lv_display_t * disp;
// Tampon de rendu : 128x20 pixels en 16 bits (2 octets par pixel)
static uint8_t draw_buf[128 * 20 * 2]; 

// --- Fonction de Flush (le pont entre LVGL et l'écran) ---
void my_disp_flush(lv_display_t * display, const lv_area_t * area, uint8_t * px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    // Dans LVGL v9, px_map est déjà au bon format de couleur
    tft.pushColors((uint16_t *)px_map, w * h, true);
    tft.endWrite();

    // Signale à LVGL que le transfert est terminé
    lv_display_flush_ready(display);
}

// --- Initialisation groupée ---
void setup_lvgl_v9() {
    lv_init();
    
    // 1. Création de l'objet display
    disp = lv_display_create(128, 160);
    
    // 2. Configuration du buffer de dessin
    lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    // 3. Liaison avec la fonction de Flush
    lv_display_set_flush_cb(disp, my_disp_flush);
}

#define USER_WORDS \
  /* --- TFT_eSPI : Gestion de l'écran --- */ \
  X("tft.init",        TFT_INIT,   tft.init()) \
  X("tft.setRotation", TFT_SETROT, tft.setRotation(n0); DROP) \
  X("tft.fillScreen",  TFT_FILL,   tft.fillScreen(static_cast<uint16_t>(n0)); DROP) \
  X("tft.invert",      TFT_INV,    tft.invertDisplay(n0); DROP) \
  \
  /* --- LVGL : Coeur et Tick --- */ \
  X("lv.init",         LV_INIT,    lv_init()) \
  X("lv.tick.inc",     LV_TICK,    lv_tick_inc(n0); DROP) \
  X("lv.timer.handler",LV_TIMER,   PUSH static_cast<cell_t>(lv_timer_handler())) \
  \
  /* --- Exemple : tft.drawPixel(x, y, color) --- */ \
  /* Usage Forth : x y color tft.pixel */ \
  X("tft.pixel",       TFT_PIXEL,  tft.drawPixel(n2, n1, n0); DROPn(3) ) \
  \
  /* --- Exemple : lv.color_make(r, g, b) --- */ \
  /* Usage Forth : r g b lv.rgb -> renvoie la couleur combinée sur TOS */ \
  X("lv.rgb",          LV_RGB,     \
    n2 = static_cast<cell_t>(n0 = lv_color_make(n2, n1, n0).full); DROPn(2) )

