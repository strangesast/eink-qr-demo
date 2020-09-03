// GxEPD_MinimumExample by Jean-Marc Zingg

#include <GxEPD.h>

// select the display class to use, only one, copy from GxEPD_Example
#include <GxGDEW0371W7/GxGDEW0371W7.h>    // 3.7" b/w

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>


GxIO_Class io(SPI, /*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2); // arbitrary selection of D3(=0), D4(=2), selected for default of GxEPD_Class
GxEPD_Class display(io, /*RST=D4*/ 2, /*BUSY=D2*/ 4); // default selection of D4(=2), D2(=4)


int dx = 0;
int dy = 0;


void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(500);
  while (!Serial) {}
  Serial.println("Serial ready");

  display.init();
  display.eraseDisplay();
  display.setRotation(3);
  display.setTextColor(GxEPD_BLACK);
 
  int height = display.height();
  int width = display.width();
   
  while (true) {
    if (Serial.available() > 0) {
      String str = Serial.readStringUntil('\r\n');
      if (str.length() > 6 && str.substring(0, 5) == "text:") {
        int i = str.indexOf(',');
        if (i < 0) {
          continue;
        }
        int s = str.substring(5, i).toInt();
        if (s > 0) {
          int l = s * s / 8 + 1;
          int rectsize = max(min(min(width, height) / s, 4), 1);
          byte *p = (byte*)malloc(l);
          if (p == NULL) {
            continue;
          }
          Serial.readBytes(p, l);
          
          display.fillRect(0, 0, width, height, GxEPD_WHITE);
          
          String text = str.substring(i + 1);
          int di = 0;
          if (text.startsWith("http://")) {
            di = 7;
          } else if (text.startsWith("https://")) {
            di = 8;
          }
          if (text.substring(di).startsWith("www.")) {
            di = di + 4;
          }
          display.setCursor(0, 0);
          display.setTextSize(3);
          display.print(text.substring(di));
          dx = width - s * rectsize;
          dy = height - s * rectsize;
          
          display.fillRect(dx, dy, s, s, GxEPD_WHITE);
          deserialize(p, s, l, rectsize);



          display.update();
          Serial.println("done");
          
          free(p);
        }
      }
    }
  }
//

//  display.print("Try Lemons");
//  display.setTextSize(3);

  display.update();
  // comment out next line to have no or minimal Adafruit_GFX code
  //display.drawPaged(drawHelloWorld); // version for AVR using paged drawing, works also on other processors
}

void deserialize(byte buf[], int s, int l, int rectsize) {
  for (int j = 0; j < s; j++) {
    for (int i = 0; i < s; i++) {
      int ij = j * s + i;
      int c = (buf[ij / 8] >> (ij % 8)) & 1 ? GxEPD_BLACK : GxEPD_WHITE;
      display.fillRect(dx + i * rectsize, dy + j * rectsize, rectsize, rectsize, c);

    }
  }
}

void drawHelloWorld()
{
  display.setRotation(3);
  display.setTextColor(GxEPD_BLACK);
  display.setTextSize(4);
  display.print("Hello World!");
}

void loop() {};
