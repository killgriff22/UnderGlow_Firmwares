#include <Adafruit_NeoPixel.h>

#include <EEPROM.h>

#include <SoftwareSerial.h>

#define SerialSpeed 115200
#define K_Serial_Tx 2
#define K_Serial_Rx 3
#define K_Wait_Period 50
SoftwareSerial K_Serial(K_Serial_Rx, K_Serial_Tx);

#define PIXEL_PIN 6    // subject to change
#define PIXEL_COUNT 53 // subject to change
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_RGB + NEO_KHZ800);

#define COLOR_R 0
#define COLOR_G 1
#define COLOR_B 2
#define PATTERN 3
#define BRIGHTNESS 4
#define SPEED 5
signed int DIRECTION = 1; // 0x07, but we cant do signed ints with EEPROM
#define MESSAGE 252
long rainbow_constant = 5*65536;
long hue = 0;
int last_pattern = 0;
int default_values[8] = {
  0xFF, //R, 0x00
  0x00, //G, 0x01
  0x00, //B, 0x02
  0x04, //Pattern, 0x03
  0xFF, //Brightness, 0x04
  0x02, //Speed, 0x05
  0x00, //Increment, 0x06, Unused
  0x01  //Direction, 0x07, Unused
  };

int pos = 0;
int frames = 0;
// shorthand for writing data to the Keypad serial connection
void Kwrite(byte data)
{
    K_Serial.write(data);
}

byte get_data(int address)
{
    Serial.println("Requesting data");
    Kwrite(0x01);
    Kwrite(address);
    Kwrite(0xFF);
    Serial.print("Sent request for data at address: ");
    Serial.println(address);
    Serial.println("Waiting for data");
    int i = 0;
    while ((K_Serial.available() <= 0) and (i < K_Wait_Period)){i++;delay(1);}
    if (not (i < K_Wait_Period)){
        Serial.println("No data received");
        Serial.println(i);
        i=0;
        return EEPROM[address];
    }
    i = 0;
    byte data = K_Serial.read();
    //Serial.println(data);
    while (K_Serial.available() > 0){K_Serial.read();Serial.println("Flushing");}
    Serial.print("Received data: ");
    Serial.println(data);
    delay(1);
    return data;
}
float led_chase[9] = {0, .25, .5, .75, 1.0, .75, .5, .25, 0};
float short_chase[5] = {0, .5, 1.0, .5, 0};

float *patterns[][7] = {
    led_chase,
    short_chase
};

void fetch_color()
{
    EEPROM.write(COLOR_R, get_data(COLOR_R));
    EEPROM.write(COLOR_G, get_data(COLOR_G));
    EEPROM.write(COLOR_B, get_data(COLOR_B));
}

void rainbow() {
    strip.rainbow(hue);
    strip.show(); // Update strip with new contents
    hue += 256;
    if (hue >= rainbow_constant) {
        hue = 0;
    }
  }

void chase(){
  unsigned int j_ = 0;
  switch (EEPROM[PATTERN])
  {
  case 1:
  case 4:
    //Long Chase
    for (signed int j = -4; j <= 4; j++)
    {
      strip.setPixelColor(pos+j, strip.Color((int) ((float) EEPROM[COLOR_R]*led_chase[j_]), (int) ((float) EEPROM[COLOR_G]*led_chase[j_]), (int) ((float) EEPROM[COLOR_B]*led_chase[j_])));
      j_++;
    }
    break;
  case 2:
  case 5:
    //Short Chase
    for (signed int j = -2; j <= 2; j++)
    {
      strip.setPixelColor(pos+j, strip.Color((int) ((float) EEPROM[COLOR_R]*short_chase[j_]), (int) ((float) EEPROM[COLOR_G]*short_chase[j_]), (int) ((float) EEPROM[COLOR_B]*short_chase[j_])));
      j_++;
    }
    break;
  }
  pos += DIRECTION;
  if (EEPROM[PATTERN] < 3){
    if ((pos > PIXEL_COUNT-4 and DIRECTION == 1) or (pos < 4 and DIRECTION == -1)){
      DIRECTION = DIRECTION*-1;
    }
  } else {
    if (pos > PIXEL_COUNT-4){
      pos = 0;
    }
  }
}



void setup()
{
    K_Serial.begin(SerialSpeed);
    Serial.begin(SerialSpeed);
    strip.begin();
    strip.show();
    //fetch_color();
}

void loop()
{
    if (frames > 50){
        frames = 0;
        fetch_color();
        EEPROM[BRIGHTNESS] = get_data(BRIGHTNESS);
        strip.setBrightness(EEPROM.read(BRIGHTNESS));
        EEPROM[PATTERN] = get_data(PATTERN);
        EEPROM[SPEED] = get_data(SPEED);
    }
    if (EEPROM[PATTERN] != last_pattern){
        strip.clear();
        last_pattern = EEPROM[PATTERN];
    }
    switch (EEPROM[PATTERN])
    {
      case 1:
          chase();
          break;
      case 2:
          chase();
          break;
      case 3:
          rainbow();
          break;
      default:
          chase();
          break;
    }
    strip.show();
    delay(50);
    frames++;
}