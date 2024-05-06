#include <msp430.h>

#include "libTimer.h"

#include "led.h"





#define SW1 BIT0

#define SW2 BIT1

#define SW3 BIT2

#define SW4 BIT3

#define SWITCHES (SW1 | SW2 | SW3 | SW4)



int secondCount =0;

int state = 0;

int currLed = 0;

int redCount =0;

int blinking = 0;





int main(void) {



  //This gets the leds and turns them off

  P1DIR |= LEDS;

  P1OUT &= ~(LED_GREEN | LED_RED);



  //Setting up clocks

  configureClocks();

  enableWDTInterrupts();



  //BUZZER

  buzzer_init();



  //Sets up buttons

  P2REN |= SWITCHES; //resistors for switches

  P2IE |= SWITCHES; //enables interrupts from swicthes

  P2OUT |= SWITCHES; //pull ups for switches

  P2DIR &= ~SWITCHES;  // set switches bits for input



  or_sr(0x18);

}



void buzzer_init(){



  timerAUpmode();

  P2SEL2 &= ~(BIT6 | BIT7);

  P2SEL &= ~BIT7;

  P2SEL |= BIT6;

  P2DIR = BIT6;



}



void buzzer_set_period(short cycles){



  CCR0 = cycles;

  CCR1 = cycles >> 3;

}



void switch_interrupt_handler()

{

  char p2val = P2IN;



  P2IES |= (p2val & SWITCHES);

  P2IES &= (p2val | ~SWITCHES);



  if ((p2val & SW1) == 0) {

    state = 2;

    secondCount = 0;

  }



  if ((p2val & SW2) == 0) {

    state = 0;

    secondCount = 0;

  }



  if ((p2val & SW3) == 0) {

    state = 1;

    secondCount = 0;

  }



  if ((p2val & SW4) == 0) {

    state = 3;

    secondCount = 0;

  }

}



void __interrupt_vec(PORT2_VECTOR) Port_2(){



  if (P2IFG & SWITCHES){

    P2IFG &= ~SWITCHES;

    switch_interrupt_handler();

  }

}



void tunes(state)

{



  float C4 = 260.00;

  float D4 = 280.00;

  float E4 = 330.00;

  float G4 = 300.00;



}





void __interrupt_vec(WDT_VECTOR) WDT()

{



  secondCount += 1;



  switch (state){

  case 0:



    buzzer_set_period(75);

    if (secondCount >= 25){

      P1OUT ^= (LED_GREEN | LED_RED);

      secondCount = 0;

    }

    break;



  case 1:



    buzzer_set_period(150);

    if (secondCount >= 75){

      P1OUT ^= (LED_GREEN | LED_RED);

      secondCount = 0;

    }

    break;



  case 2:



    buzzer_set_period(300);

    if (secondCount >= 50) {

      if (currLed){

	P1OUT = (P1OUT & ~LED_RED) | LED_GREEN;

	currLed = 0;

      } else {

	P1OUT = (P1OUT & ~LED_GREEN) | LED_RED;

	currLed = 1;

      }

    }

    break;

  case 3:

    buzzer_set_period(400);

    if (secondCount >= 25){

      if (redCount <= 10) {

	P1OUT ^= LED_GREEN;

	redCount += 1;

      } else {

	P1OUT ^= LED_RED;

	redCount = 0;

      }

      secondCount = 0;

    }

    break;

  }

}
