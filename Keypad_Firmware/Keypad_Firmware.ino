
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

#define SerialSpeed 115200
#define CSerial_Tx 11
#define CSerial_Rx 12

SoftwareSerial CSerial(CSerial_Rx, CSerial_Tx);

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


int lcd_key = 0;
int adc_key_in = 0;
int menu_index = 1;
int display_options;
byte incomingByteS = 0; // for incoming serial port data
byte incomingByteC = 0; // for incoming pixel controller data
int message[3];

#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5
#define btnclicktime 300

#define COLOR_R 0
#define COLOR_G 1
#define COLOR_B 2
#define PATTERN 3
#define BRIGHTNESS 4
#define SPEED 5
#define INCREMENT 6
#define DIRECTION 7
#define MESSAGE 252

int default_values[8] = {0xFF, 0x00, 0x00, 0x01, 0xFF, 0x01, 0x0A, 0x01};

int read_LCD_buttons()
{
     adc_key_in = analogRead(0);
     if (adc_key_in > 1000)
          return btnNONE;
     if (adc_key_in < 50)
          return btnRIGHT;
     if (adc_key_in < 150)
          return btnUP;
     if (adc_key_in < 300)
          return btnDOWN;
     if (adc_key_in < 450)
          return btnLEFT;
     if (adc_key_in < 700)
          return btnSELECT;
     return btnNONE;
};
// menus
char *MainMenu[5] = {
    "Main Menu",
    "Patterns",
    "Colors",
    "Options",
    "About"};

char *Patterns[7] = {
    "Patterns",
    "chase full",
    "chase simple",
    "rnbw full",
    "full pulse",
    "simple pulse",
    "rnbw chase",
    };

char *Colors[4] = {
    "Colors",
    "R",
    "G",
    "B"};

char *Options[6] = {
    "Options",
    "Brightness",
    "Speed",
    "Increment",
    "Direction",
    "Reset"};

char *About[5] = {
    "About: {Name}",
    "Version: 1.0.0",
    "Authors:",
    "Thisisx-x",
    "Dark Bunny"};

char *current_menu[7];
char *menu_name;
int menulen = 5;

void load_menu(int index)
{
     switch (index)
     {
     case 0:
          for (int i = 0; i < 5; i++)
          {
               current_menu[i] = MainMenu[i];
          }
          menulen = 5;
          break;
     case 1:
          for (int i = 0; i < 7; i++)
          {
               current_menu[i] = Patterns[i];
          }
          menulen = 6;
          break;
     case 2:
          for (int i = 0; i < 4; i++)
          {
               current_menu[i] = Colors[i];
          }
          menulen = 4;
          break;
     case 3:
          for (int i = 0; i < 6; i++)
          {
               current_menu[i] = Options[i];
          }
          menulen = 6;
          break;
     case 4:
          for (int i = 0; i < 5; i++)
          {
               current_menu[i] = About[i];
          }
          menulen = 5;
          break;
     };
     menu_name = current_menu[0];
     menu_index = 1;
};

void handle_input()
{
     lcd_key = read_LCD_buttons();
     // switch is a more readable stand in for a big "if / elseif" chain
     switch (lcd_key)
     {
     case btnUP:
     {
          // decrease the current menu option
          Serial.println("UP");
          menu_index = menu_index - 1;
          break;
     }
     case btnDOWN:
     {
          // increase the current menu option
          Serial.println("DOWN");
          menu_index = menu_index + 1;
          break;
     }
     case btnSELECT:
     {
          Serial.println("SELECT");
          if (current_menu[0] == "Main Menu")
          {
               load_menu(menu_index);
               break;
          }
          else if (current_menu[menu_index] == "back")
          {
               load_menu(0);
               break;
          }
          else if (current_menu[0] == "Patterns")
          {
               EEPROM[PATTERN] = menu_index;
               break;
          }
          else if (current_menu[0] == "Options")
          {
               switch (menu_index)
               {
               case 5:
               {
                    for (int i = 0; i < 8; i++)
                    {
                         EEPROM[i] = default_values[i];
                    }
                    break;
               }
               }
               break;
          }
          break;
     }
     case btnRIGHT:
     {
          // do something
          Serial.println("RIGHT");
          if (menu_name == "Colors")
          {
               switch (menu_index)
               {
               case 1:
               {
                    EEPROM[COLOR_R] = EEPROM[COLOR_R] + EEPROM[INCREMENT];
                    break;
               }
               case 2:
               {
                    EEPROM[COLOR_G] = EEPROM[COLOR_G] + EEPROM[INCREMENT];
                    break;
               }
               case 3:
               {
                    EEPROM[COLOR_B] = EEPROM[COLOR_B] + EEPROM[INCREMENT];
                    break;
               }
               break;
               }
               break;
          }
          else if (menu_name == "Options")
          {
               switch (menu_index)
               {
               case 1:
               {
                    EEPROM[BRIGHTNESS] = EEPROM[BRIGHTNESS] + 1;
                    break;
               }
               case 2:
               {
                    EEPROM[SPEED] = EEPROM[SPEED] + 1;
                    break;
               }
               case 3:
               {
                    EEPROM[INCREMENT] = EEPROM[INCREMENT] + 1;
                    break;
               }
               case 4:
               {
                    EEPROM[DIRECTION] = EEPROM[DIRECTION] + 1;
                    break;
               }
               }
          }
          break;
     }
     case btnLEFT:
     {
          // do something
          Serial.println("LEFT");
          if (menu_name == "Colors")
          {
               switch (menu_index)
               {
               case 1:
               {
                    EEPROM[COLOR_R] = EEPROM[COLOR_R] - EEPROM[INCREMENT];
                    break;
               }
               case 2:
               {
                    EEPROM[COLOR_G] = EEPROM[COLOR_G] - EEPROM[INCREMENT];
                    break;
               }
               case 3:
               {
                    EEPROM[COLOR_B] = EEPROM[COLOR_B] - EEPROM[INCREMENT];
                    break;
               }
               break;
               }

               break;
          }
          else if (menu_name == "Options")
          {
               switch (menu_index)
               {
               case 1:
               {
                    EEPROM[BRIGHTNESS] = EEPROM[BRIGHTNESS] - EEPROM[INCREMENT];
                    break;
               }
               case 2:
               {
                    EEPROM[SPEED] = EEPROM[SPEED] - EEPROM[INCREMENT];
                    break;
               }
               case 3:
               {
                    EEPROM[INCREMENT] = EEPROM[INCREMENT] - 1;
                    break;
               }
               case 4:
               {
                    EEPROM[DIRECTION] = EEPROM[DIRECTION] - 1;
                    break;
               }
               }
               break;
          }
     }
     }
     if (lcd_key == btnNONE)
     {
          return;
     }
     Serial.print("Menu Index: ");
     Serial.print(menu_index);
     Serial.print(" Menu: ");
     Serial.println(current_menu[0]);
     delay(btnclicktime);
}

void setup()
{
     // lcd setup
     lcd.begin(16, 2);
     Serial.begin(SerialSpeed);
     CSerial.begin(SerialSpeed);
     // load the main menu into our current menu
     load_menu(0);
     // set the last item in the menu to be "back"
}

void handle_serial_i2c(){
     if (CSerial.available() > 0  or Serial.available() > 0){
          incomingByteS = Serial.read();// read the request from the onboard serial connection
          incomingByteC = CSerial.read();// read the request from the led controller
          //requests from the pixel controller
          if (incomingByteC == 0x01){// read request
               Serial.println("Received Read Request from the pixel controller");
               for (int i = 0; i < 3; i++)
               {
                    message[i] = CSerial.read();
               }
               Serial.print("Reading: ");
               Serial.print(message[0]);
               Serial.print(" Value: ");
               Serial.println(EEPROM[message[0]]);
               CSerial.write(EEPROM[message[0]]);
          }
          else if (incomingByteC == 0xDA){//write request
               Serial.println("Received write Request from the pixel controller");
               for (int i = 0; i < 3; i++)
               {
                    message[i] = CSerial.read();
               }
               Serial.print("Writing: ");
               Serial.print(message[0]);
               Serial.print(" Value: ");
               EEPROM[message[0]] = message[1];
               Serial.println(EEPROM[message[0]]);
               CSerial.write(EEPROM[message[0]]);
          }
          else if (incomingByteC == 0xDB){//reset request
               Serial.println("Received Reset Request from the pixel controller");
               for (int i = 0; i < 8; i++)
               {
                    EEPROM[i] = default_values[i];
               }
               CSerial.write(EEPROM[MESSAGE]);
          } 
          else if (incomingByteC == 0xDC){//format request
               for (int i=0;i<1024;i++){
                    EEPROM[i] = 0x00;
               }
          }
          //requests from the serial connection
          if (incomingByteS == 0x01){//read request
               Serial.println("Received Read Request");
               for (int i = 0; i < 3; i++)
               {
                    message[i] = Serial.read();
               }
               Serial.print("Reading: ");
               Serial.print(message[0]);
               Serial.print(" Value: ");
               Serial.println(EEPROM[message[0]]);
               CSerial.write(EEPROM[message[0]]);
          }
          else if (incomingByteS == 0xFA){//write request
               Serial.println("Received write Request");
               for (int i = 0; i < 3; i++)
               {
                    message[i] = Serial.read();
               }
               Serial.print("Writing: ");
               Serial.print(message[0]);
               Serial.print(" Value: ");
               EEPROM[message[0]] = message[1];
               Serial.println(EEPROM[message[0]]);
               CSerial.write(EEPROM[message[0]]);
          }
          else if (incomingByteS == 0xFB){//reset request
               Serial.println("Received Reset Request");
               for (int i = 0; i < 8; i++)
               {
                    EEPROM[i] = default_values[i];
               }
               Serial.println(EEPROM[MESSAGE]);
               CSerial.write(EEPROM[MESSAGE]);
          } 
          else if (incomingByteS == 0xFC){//format request
               for (int i=0;i<1024;i++){
                    EEPROM[i] = 0x00;
               }
          }
     }
}

void loop()
{
     // clear the screen,
     // read the incoming byte:
     // say what you got:
     handle_serial_i2c();
     current_menu[menulen] = "back";
     lcd.clear();
     lcd.setCursor(0, 0);
     // print the name of the menu,
     lcd.print(menu_name);
     // and print the option we are selecting
     lcd.setCursor(0, 1);
     lcd.print(current_menu[menu_index]);
     menu_index = constrain(menu_index, 1, menulen);
     int r = EEPROM[COLOR_R];
     int g = EEPROM[COLOR_G];
     int b = EEPROM[COLOR_B];
     r = constrain(r, 0, 255);
     g = constrain(g, 0, 255);
     b = constrain(b, 0, 255);
     EEPROM[COLOR_R] = r;
     EEPROM[COLOR_G] = g;
     EEPROM[COLOR_B] = b;
     // delay for a little bit, just in case we arent pressing anything
     if (menu_name == "Colors")
     {
          switch (menu_index)
          {
          case 1:
               lcd.print(": ");
               lcd.print(EEPROM[COLOR_R]);
               break;
          case 2:
               lcd.print(": ");
               lcd.print(EEPROM[COLOR_G]);
               break;
          case 3:
               lcd.print(": ");
               lcd.print(EEPROM[COLOR_B]);
               break;
          }
     }
     else if (menu_name == "Options")
     {
          switch (menu_index)
          {
          case 1:
               lcd.print(": ");
               lcd.print(EEPROM[BRIGHTNESS]);
               break;
          case 2:
               lcd.print(": ");
               lcd.print(EEPROM[SPEED]);
               break;
          case 3:
               lcd.print(": ");
               lcd.print(EEPROM[INCREMENT]);
               break;
          case 4:
               lcd.print(": ");
               lcd.print(EEPROM[DIRECTION]);
               break;
          }
     } else if (menu_name == "Patterns"){
          if (menu_index == EEPROM[PATTERN]){
               lcd.print(" <-");
          }
     }

     delay(10);
     //(if we dont do this, the display will go dim)
     handle_input();
}