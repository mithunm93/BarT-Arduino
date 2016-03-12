/*

Copyright (c) 2012, 2013 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <Servo.h>
#include <SPI.h>
#include <boards.h>
#include <RBL_nRF8001.h>
#include <services.h>
#include "Boards.h"
#include <LiquidCrystal.h>
#include <TimerOne.h>
#include "LPD6803.h"

//#include "Drinks.h"

#define PROTOCOL_MAJOR_VERSION   0 //
#define PROTOCOL_MINOR_VERSION   0 //
#define PROTOCOL_BUGFIX_VERSION  2 // bugfix

#define PIN_CAPABILITY_NONE      0x00
#define PIN_CAPABILITY_DIGITAL   0x01
#define PIN_CAPABILITY_ANALOG    0x02
#define PIN_CAPABILITY_PWM       0x04
#define PIN_CAPABILITY_SERVO     0x08
#define PIN_CAPABILITY_I2C       0x10


// pin modes
//#define INPUT                 0x00 // defined in wiring.h
//#define OUTPUT                0x01 // defined in wiring.h
#define ANALOG                  0x02 // analog pin in analogInput mode
#define PWM                     0x03 // digital pin in PWM output mode
#define SERVO                   0x04 // digital pin in Servo output mode

byte pin_mode[TOTAL_PINS];
byte pin_state[TOTAL_PINS];
byte pin_pwm[TOTAL_PINS];
byte pin_servo[TOTAL_PINS];

Servo servos[MAX_SERVOS];

LPD6803 outsideStrip = LPD6803(30, 8, 9);
//LPD6803 outsideStrip = LPD6803(9, 10, 11);

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
byte busy[1];
byte cmd[20];

int solenoid_1 = 30;
int solenoid_2 = 31;
int solenoid_3 = 32;
int solenoid_4 = 33;
int solenoid_5 = 34;
int solenoid_6 = 35;
int solenoid_7 = 36;
int solenoid_8 = 37;
int solenoid_9 = 38;
int solenoid_10 = 39;
int solenoid_11 = 40;
int solenoid_12 = 41;
int solenoid_13 = 42;
int solenoid_14 = 43;
int solenoid_15 = 44;
int solenoid_16 = 45;
        
typedef struct
{
  char* name;
  int numValves;
  int* valves;
  int* timings;
} Drink;

  /*
  40 - water
  42 - apple juice
  43 - orange juice
  45 - cranberry juice
  */

int orange_juice_valves[1] = {43};
int orange_juice_timings[1] = {10000};

Drink orange_juice = { "Orange Juice",              //name
                        1,                          //numValves
                        orange_juice_valves,        //valves
                        orange_juice_timings,       //timings
                      };
                        
                        
int apple_juice_valves[1] = {42};
int apple_juice_timings[1] = {10000};

Drink apple_juice = { "Apple Juice",               //name
                        1,                             //numValves
                        apple_juice_valves,            //valves
                        apple_juice_timings,           //timings
                      };
                      
int cranberry_juice_valves[1] = {45};
int cranberry_juice_timings[1] = {10000};

Drink cranberry_juice = { "Cranberry Juice",           //name
                        1,                             //numValves
                        cranberry_juice_valves,        //valves
                        cranberry_juice_timings,       //timings
                      };

int water_valves[1] = {40};
int water_timings[1] = {10000};

Drink water = { "Water",                     //name
                        1,                             //numValves
                        water_valves,                  //valves
                        water_timings,                 //timings
                      };

int apple_cranberry_juice_valves[2] = {42, 45};
int apple_cranberry_juice_timings[2] = {8000, 8000};

Drink apple_cranberry_juice = { "Apple Cranberry",               //name
                        2,                             //numValves
                        apple_cranberry_juice_valves,            //valves
                        apple_cranberry_juice_timings,           //timings
                      };
                      
int orange_cranberry_water_valves[3] = {43, 45, 40};
int orange_cranberry_water_timings[3] = {6000, 6000, 3000};

Drink orange_cranberry_water = { "OJ Cranberry H2O",               //name
                        3,                             //numValves
                        orange_cranberry_water_valves,            //valves
                        orange_cranberry_water_timings,           //timings
                      };
                      
int everything_valves[4] = {40, 42, 43, 45};
int everything_timings[4] = {4000, 4000, 4000, 4000};

Drink everything = { "Apl Crbry OJ H2O",               //name
                        2,                             //numValves
                        everything_valves,            //valves
                        everything_timings,           //timings
                      };

Drink drinks[7] = {orange_juice, apple_juice, cranberry_juice, water, apple_cranberry_juice, orange_cranberry_water, everything};

void setup()
{
  Serial.begin(57600);
  Serial.println("BLE Arduino Slave");
  
  Serial.println(TOTAL_PINS);
  
  /* Default all to digital input */
  for (int pin = 0; pin < TOTAL_PINS; pin++)
  {
    // Set pin to input with internal pull up
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);

    // Save pin mode and state
    pin_mode[pin] = INPUT;
    pin_state[pin] = LOW;
  }
  
  
    pinMode(solenoid_1, OUTPUT);
    pinMode(solenoid_2, OUTPUT);
    pinMode(solenoid_3, OUTPUT);
    pinMode(solenoid_4, OUTPUT);
    pinMode(solenoid_5, OUTPUT);
    pinMode(solenoid_6, OUTPUT);
    pinMode(solenoid_7, OUTPUT);
    pinMode(solenoid_8, OUTPUT);
    pinMode(solenoid_9, OUTPUT);
    pinMode(solenoid_10, OUTPUT);
    pinMode(solenoid_11, OUTPUT);
    pinMode(solenoid_12, OUTPUT);
    pinMode(solenoid_13, OUTPUT);
    pinMode(solenoid_14, OUTPUT);
    pinMode(solenoid_15, OUTPUT);
    pinMode(solenoid_16, OUTPUT);
    
    for(int k = 30; k < 46; k++)
      digitalWrite(k, LOW);

  lcd.begin(16, 2);
  lcd.print("      BarT");
  lcd.setCursor(2,1);
  lcd.print("Initialized");
  
  busy[0] = 'F';
  cmd[0] = 'E';  // for empty;
  
  //outsideStrip.setCPUmax(50); 
  //insideStrip.setCPUmax(50);

  // Start up the LED counter
  //outsideStrip.begin();
  //insideStrip.begin();
  
  //outsideStrip.show();
  //insideStrip.show();
              
  // Default pins set to 9 and 8 for REQN and RDYN
  // Set your REQN and RDYN here before ble_begin() if you need
  //ble_set_pins(3, 2);
  
  // Set your BLE Shield name here, max. length 10
  ble_set_name("BarT");

  // Init. and start BLE library.
  ble_begin();
  
}

static byte buf_len = 0;

void ble_write_string(byte *bytes, uint8_t len)
{
  if (buf_len + len > 20)
  {
    for (int j = 0; j < 15000; j++)
      ble_do_events();
    
    buf_len = 0;
  }
  
  for (int j = 0; j < len; j++)
  {
    ble_write(bytes[j]);
    buf_len++;
  }
    
  if (buf_len == 20)
  {
    for (int j = 0; j < 15000; j++)
      ble_do_events();
    
    buf_len = 0;
  }  
}
void sendCustomData(uint8_t *buf, uint8_t len)
{
  uint8_t data[20] = "Z";
  memcpy(&data[1], buf, len);
  ble_write_string(data, len+1);
}

int k = 0;

byte queryDone = false;
bool orderDone = false;
int drinkNum;

void loop()
{
  /*
  40 - water
  42 - apple juice
  43 - orange juice
  45 - cranberry juice
  */
 
  
  if (ble_connected() && true)
  {
    ble_write_string(busy, 1);
    buf_len = 0;
    delay(1000);
  
    // command already exists or new one coming in
    if(cmd[0] == 'E' && ble_available())
    {
      cmd[0] = ble_read();
    
      switch (cmd[0])
      {  
        case 'Z':
        {
          lcd.clear();
          orderDone = false;
          // format: 'Z', nameLen, name, drinkNum
          int nameLen = ble_read();
         
          lcd.setCursor(0,0);
          for(int i = 0; i < nameLen; i++)
            lcd.write((char)ble_read());
          
          lcd.setCursor(0,1);
          drinkNum = (char)ble_read();
          lcd.print(drinks[drinkNum].name);
        }
      }
    }
    
    // if there is a command, and a cup exists, and it hasn't
    //     just finished pouring, get drink
    if(cmd[0] != 'E' && analogRead(A0) <= 10 && !orderDone)
    {
      delay(3000);
      if(analogRead(A0) <= 10)
      {
        //we're busy!!
        busy[0] = 'B';
        ble_write_string(busy, 1);

        
        // pour drink order here
        for(int i=0; i < drinks[drinkNum].numValves; i++)
        {    
          digitalWrite(drinks[drinkNum].valves[i], HIGH);
          delay(drinks[drinkNum].timings[i]);
          digitalWrite(drinks[drinkNum].valves[i], LOW);
          delay(500);
        }
        orderDone = true;
        
        lcd.setCursor(0,1);
        lcd.print("     Done       ");
      }
    }
    
    // if the cup has been removed
    if(orderDone && analogRead(A0) > 10)
    {
      cmd[0] = 'E';
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.write("Waiting For");
      lcd.setCursor(5, 1);
      lcd.write("Order");
      
      // now I'm free again
      busy[0] = 'F';
      ble_write_string(busy, 1);
      delay(1000);
    }
    
    // send out any outstanding data
    ble_do_events();
    buf_len = 0;
    
    return;
  }else
  {
    //digitalWrite(13, LOW);
    delay(1000);
  }
    
  ble_do_events();
  buf_len = 0;
  
}
