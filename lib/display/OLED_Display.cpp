#include "OLED_Display.h"

OLED_Display::OLED_Display() : 
    display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET),
    i2c(Wire),
    initialized(false) {
}

bool OLED_Display::begin() {
    i2c.begin(OLED_SDA, OLED_SCL);
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        return false;
    }
    
    display.clearDisplay();
    display.display();
    initialized = true;
    return true;
}

void OLED_Display::updateDisplay(float lightLevel, int gasLevel, bool ledState, uint8_t r, uint8_t g, uint8_t b) {
    if (!initialized) return;
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    // 显示光照强度
    display.setCursor(0,0);
    display.print("Light: ");
    if(lightLevel >= 0) {
        display.print(lightLevel);
        display.println(" lx");
    } else {
        display.println("Error");
    }
    
    // 显示气体浓度
    display.setCursor(0,16);
    display.print("Gas: ");
    display.println(gasLevel);
    
    // 显示LED状态
    display.setCursor(0,32);
    display.print("LED: ");
    if(ledState) {
        display.print("ON (RGB:");
        display.print(r);
        display.print(",");
        display.print(g);
        display.print(",");
        display.print(b);
        display.println(")");
    } else {
        display.println("OFF");
    }
    
    display.display();
}

void OLED_Display::clear() {
    if (!initialized) return;
    display.clearDisplay();
    display.display();
} 