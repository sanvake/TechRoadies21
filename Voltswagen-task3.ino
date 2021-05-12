/*
 Task 3- Voltswagen, Tech Roadies 2021
    Team Blitzkrieg

 This sketch prints "IMPETUS 21.0" to the LCD
 if the authentication is completed.

  The circuit:
 * LCD RS pin to digital pin 1
 * LCD Enable pin to digital pin 0
 * LCD D4 pin to digital pin 7
 * LCD D5 pin to digital pin 6
 * LCD D6 pin to digital pin 5
 * LCD D7 pin to digital pin 4
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * ends to +5V and ground
 * pin 9 to LCD VO pin (pin 3)
 * pins A1 - A5 connected to LEDs (can be scaled up if needed)
 * 2 push buttons connected to interrupt pins 2 and 3 and pulled down.
 
 The link for the TinkerCAD circuit.

 https://www.tinkercad.com/things/5oHcqZ9gkUu


  +--------+----------------+---------------+-------------------------------------------------------------------+
  | Sl no. |    Variable    |     Type      |                            Description                            |
  +--------+----------------+---------------+-------------------------------------------------------------------+
  |      1 | count          | volatile int  | for keeping count the number of times the count button is pressed |
  |      2 | flag           | volatile int  |  for keeping count of times the confirm button is pressed         |
  |      3 | prevcount      | int           | used for updating LCD                                             |
  |      4 | plus           | int           | pin no. for counter button                                        |
  |      5 | yes            | int           | pin no. for yes/OK button                                         |
  |      6 | auth           | int           | random values within a given range                                |
  |      7 | imp            | int           | random values within a given range                                |
  |      8 |  i             | int           | position of the last lit up led                                   |
  |      9 |  n             | int           | no. of LEDs                                                       |
  |     10 | cont           | int           | used to control contrast of LCD without a potentiometer           |
  |     11 | pin[]          | int(array)    | pins for LEDs                                                     |
  |     12 | wasreset       | bool          | used to check if vreset function was run                          |
  |     13 | previousMillis | unsigned long | takes the value of the last millis() call                         |
  |     14 | interval       | const long    | duration before next led turns on                                 |
  |     15 | currentMillis  | unsigned long | gets value from millis()                                          |
  +--------+----------------+---------------+-------------------------------------------------------------------+
*/




#include <LiquidCrystal.h>

volatile int count = 0;
volatile int flag = 0;
int prevcount = -1;
const int plus = 3;
const int yes = 2;
int auth, imp;
int i = 0, n = 5;
int cont = 10;
int pin[] = {A1, A2, A3, A4, A5};
bool wasreset = false;

unsigned long previousMillis = 0;
const long interval = 500;
unsigned long currentMillis = millis();
                              // initialize the library with the numbers of the interface pins
LiquidCrystal lcd(1, 0, 7, 6, 5, 4);

void setup()
{
  pinMode(9, OUTPUT);
  analogWrite(9, cont);
  randomSeed(analogRead(A0)); // random seed fo rng using environmental noise
  for (int i = 0; i < n; i++) // initializing led output based on number of LEDs:
  {
    pinMode(pin[i], OUTPUT);
  }
  lcd.begin(16, 2);
  pinMode(yes, INPUT_PULLUP); // interrupts for counting buttons at falling edges
  attachInterrupt(digitalPinToInterrupt(yes), button_pressed, FALLING);
  pinMode(plus, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(plus), increment, FALLING);
  lcd.clear();
  auth = random(0, 10);
  imp = random(1, n + 1); 
}

void increment()             // ISR for count button press
{ 
  count++;
}

void button_pressed()        // ISR for OK/yes button press
{   
  flag++;
}
void vreset()                // resets all values in case of "invalid" or "connection lost" and also randomizes auth and confirmation led values
{
  i = 0;                     
  count = 0;
  auth = random(0, 10);
  imp = random(1, n + 1);
  flag = 0;
  wasreset = true;  
  for (int j = 0; j < n; j++)
  {
    digitalWrite(pin[j], LOW);
  }
}

void loop()
{
  if (prevcount != count || wasreset)
  {                         // to refresh LCD during boot up and after invalid attempts
    wasreset = false;
    prevcount = count;
    lcd.clear();
    lcd.print("Authentication:");
    lcd.print(auth);
    lcd.setCursor(3, 1);
    lcd.print("Code: ");
    lcd.print(count);
  }

  if (flag > 0)
  {                         // check if confirm button was pressed
    if (count == auth)
    {                       // check if auth is same as button presses
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Success");
      currentMillis = millis();
      previousMillis = currentMillis;
      while (!(currentMillis - previousMillis >= 500))
      {
        currentMillis = millis();
      }
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Press: ");
      lcd.print(imp);
      while (true)
      {                     // after authentication to loop through leds until button pressed
        currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {                   // led cascading
          digitalWrite(pin[i], HIGH);
          ++i;

          previousMillis = currentMillis;
        }
        if (flag > 1)
        {                   // once button pressed, if success; end of program else reset
          if (i == imp)
          {
            lcd.setCursor(2, 0);
            lcd.print("IMPETUS 21.0");
            while (true);
          }
          else
          {
            lcd.setCursor(3, 0);
            lcd.print("Connection");
            lcd.setCursor(6, 1);
            lcd.print("Lost");
            currentMillis = millis();
            previousMillis = currentMillis;
            while (!(currentMillis - previousMillis >= 2000))
            {
              currentMillis = millis();
            }
            vreset();
            break;
          }
        }
        currentMillis = millis();
        if (i > 4 && (currentMillis - previousMillis >= interval))
        {                
          i = 0;
          for (int j = 0; j < n; j++)
          {
            digitalWrite(pin[j], LOW);
          }
          previousMillis = currentMillis;
        }
      }
    }
    else
    {                      // auth reset if invalid count is confirmed
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Invalid");
      currentMillis = millis();
      previousMillis = currentMillis;
      while (!(currentMillis - previousMillis >= 1500))
      {
        currentMillis = millis();
      }
      vreset();
    }
  }
}
