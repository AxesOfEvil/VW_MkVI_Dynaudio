#include <SPI.h>
#include <mcp2515.h>
#include <avr/sleep.h>//this AVR library contains the methods that controls the sleep modes
#include <LowPower.h>
#include "CanMsg.h"

MCP2515 mcp2515(10); //Use Pins: CS=10, SCK=14, MISO=15, MOSI=16
#define wakePin 2
#define pwrPin 9
#define canIntPin 3
#define RXLED 17
#define Ser if(0) Serial1

enum {
  STATE_INIT = 0,
  STATE_POWERUP,
  STATE_POWERDOWN,
  STATE_UNKNOWN,
};

int led = 0;
int change = 0;

void set_led(bool state) {
  led = state;
  digitalWrite(RXLED, state ? LOW : HIGH);
}

void flash_led(int count, int led_delay=50){
  int orig_led = led;
  if (led) {
    set_led(0);
    delay(led_delay);
  }
  for (int i = 0; i < count; i++) {
    set_led(1);
    delay(led_delay);
    set_led(0);
    delay(led_delay);
  }
  if (orig_led) {
    set_led(1);
  }
}

void GoingToSleep(){
    Ser.println("going to sleep!");
    sleep_enable();//Enabling sleep mode
    //attachInterrupt(digitalPinToInterrupt(wakePin), wakeUp, LOW);//attaching a interrupt to pin d2
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
    sleep_cpu();//activating sleep mode
    Ser.println("just woke up!");//next line of code executed after the interrupt 
  }

void wakeUp(){
  Ser.println("Interrrupt Fired");//Print message to serial monitor
  sleep_disable();//Disable sleep mode
  detachInterrupt(digitalPinToInterrupt(wakePin)); //Removes the interrupt from pin 2;
}

void wake2(){
  sleep_disable();
  change = digitalRead(wakePin) ? 4 : 2;
  detachInterrupt(digitalPinToInterrupt(wakePin));
}
void setup() {  
  pinMode(pwrPin,OUTPUT);
  digitalWrite(pwrPin, HIGH);
  
  //while (!Serial);
  Ser.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_100KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  pinMode(wakePin,INPUT);  //PUSH/PULL
  pinMode(RXLED, OUTPUT);
  pinMode(canIntPin, INPUT_PULLUP);
  flash_led(3);
  Ser.println("Example: Write to CAN");
  //attachInterrupt(digitalPinToInterrupt(wakePin), wake2, CHANGE);
}


void ampPowerUp() {
  for (int i = 0; i < sizeof(canMsg_AmpEnable) / sizeof(can_frame); i++) {
    mcp2515.sendMessage(&canMsg_AmpEnable[i]);
    delay(50);  
  }
  set_led(1);
  Ser.println("PowerUp");
}

void ampPowerDown() {
  mcp2515.sendMessage(&canMsg_AmpDisable);
  set_led(0);
  Ser.println("PowerDown");
}

void loop() {
  static int state = STATE_INIT;
  Ser.println("loop");
  if (state == STATE_INIT) {
    delay(500);  // Ensure rail reaches 5V
    ampPowerUp();
    delay(1000);  // Debounce for paranoia
    state = STATE_POWERUP;
    return;
  }
  int updn = digitalRead(wakePin);
  Ser.print("Up/Down: ");
  Ser.println(updn);
  if (updn) {
    if (state == STATE_POWERDOWN) {
      digitalWrite(pwrPin, HIGH);
      ampPowerUp();
      state = STATE_POWERUP;
      delay(1000);
    }
  } else {
    if (state == STATE_POWERUP) {
      set_led(0);
      delay(10);
      set_led(1);
      ampPowerDown();
      state = STATE_POWERDOWN;
      delay(1000);
      digitalWrite(pwrPin, LOW);
    }
  }
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); 
  //LowPower.idle(SLEEP_1S, ADC_OFF, TIMER4_OFF, TIMER3_OFF, TIMER1_OFF, TIMER0_OFF, 
  //              SPI_OFF, USART1_OFF, TWI_OFF, USB_OFF);

}
