#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Stepper.h>
#include <IRremote.h>

#define TFT_DC 9
#define TFT_CS 10
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define PIR_PIN 8
int pirState = LOW;             
int val = 0; 

#define ledpin_R 0
#define ledpin_G 1
#define ledpin_B 2

int stepsPerRevolution = 2000;
int stepsPerReverseRevolution = 500;
const int A_min = 3;
const int A_plus = 4;
const int B_plus = 5;
const int B_min = 6;
Stepper myStepper(stepsPerRevolution, A_min, A_plus, B_plus, B_min);

#define pin_IR 7

void alarma();
void display_motion(const char* texxt, uint16_t culoare);
void display_menu();
void setRGBColor(bool red, bool green, bool blue);
void translateIR();



void setup() {
  tft.begin();
  tft.setRotation(1);
  pinMode(PIR_PIN, INPUT);
  pinMode(ledpin_R, OUTPUT);
  pinMode(ledpin_G, OUTPUT);
  pinMode(ledpin_B, OUTPUT);
  setRGBColor(0, 1, 0);
  myStepper.setSpeed(10); 
  IrReceiver.begin(pin_IR, ENABLE_LED_FEEDBACK);  
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.println("Ecranul a fost pornit");
  tft.println("LED-ul a fost pornit");
  tft.println("Senzorul este in functiune"); 
  tft.print("\nMeniu optiuni: ");
  tft.setTextColor(ILI9341_ORANGE);
  tft.print("MENU\n");  
}

void loop() {
  if (IrReceiver.decode()) {
    translateIR();
    IrReceiver.resume();  
  }
  alarma();
}

void alarma(){
  val = digitalRead(PIR_PIN);  
  if (val == HIGH) {          
    if (pirState == LOW) {
      display_motion("Motion Detected \nStepper-ul a fost pornit", ILI9341_RED);
      setRGBColor(1, 0, 0);
      myStepper.step(-stepsPerRevolution);
      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH) {
      display_motion("No motion detected", ILI9341_BLUE);
      setRGBColor(0, 0, 1);
      myStepper.step(stepsPerReverseRevolution);
      pirState = LOW;
    }
  }
}

void display_motion(const char* texxt, uint16_t culoare) {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(culoare);
  tft.setTextSize(2);
  tft.println(texxt);
}

void setRGBColor(bool red, bool green, bool blue) {
  digitalWrite(ledpin_R, red);
  digitalWrite(ledpin_G, green);
  digitalWrite(ledpin_B, blue);
}

void translateIR(){
  switch (IrReceiver.decodedIRData.command) {
    case 162:
      tft.fillScreen(ILI9341_BLACK);
      setup();
      break;
    case 226:
      display_menu();
      break;
    case 34:
      display_menu();
      display_motion("Stepper-ul se afla in in mod manual", ILI9341_CYAN);
      setRGBColor(0, 1, 1);
      myStepper.step(-stepsPerRevolution);
      break;
    case 194:
      setRGBColor(0, 1, 1);
      myStepper.step(stepsPerRevolution);
      break;
   case 104:
      setRGBColor(0, 1, 1);
      display_motion("Setari Stepper: ", ILI9341_YELLOW);
      tft.setTextColor(ILI9341_YELLOW);
      tft.setTextSize(2);
      tft.println("4. Schimbare viteza de rotatie");
      tft.println("5. Schimbare nr. rotatii pozitive");
      tft.println("C. Revenire la setari standard");
      tft.println("Vit = 10; Rot. poz = 2000");
      break;
    case 176:
      myStepper.setSpeed(10);
      stepsPerRevolution = 2000;
      stepsPerReverseRevolution = 500;
      break;
    case 16:
      tft.println("Optiunu viteza de rotatie");
      tft.println("<<. Viteza rot. = 5");
      tft.println(">>. Viteza rot. = 20");
      break;
    case 224:
      myStepper.setSpeed(5);
      break;
    case 144:
      myStepper.setSpeed(20);
      break;
    case 56:
      tft.println("Optiunu nr. rotatii");
      tft.println("+. Numar rotatii = 5000");
      tft.println("-. Viteza rot. = 500");
      break;
    case 2:
      stepsPerRevolution = 5000;
      break;
    case 152:
      stepsPerRevolution = 500;
      break;
  }
}

void display_menu(){
  tft.fillScreen(ILI9341_BLACK);
  setRGBColor(1, 1, 0);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(4);
  tft.println("MENIU");
  tft.setTextSize(2);
  tft.println("TEST. Activare manuala stepper (BACK pt. rotatie inversa)");
  tft.println("0. Optiuni stepper");
}