/*
 * main.c
 *
 *  Created on: 2018 Mar 05 16:48:01
 *  Author: michealdunne14
 */




#include <DAVE.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)

/**

 * @brief main() - Application entry point
 *
 * <b>Details of function</b><br>
 * This routine is the application entry point. It is invoked by the device startup code. It is responsible for
 * invoking the APP initialization dispatcher routine - DAVE_Init() and hosting the place-holder for user application
 * code.
 */

//Timer to count the time on the LEDs
uint32_t timer = 0;
// Brightness for the PWM LED
uint32_t brightness;

//Puts all LEDS in to Code
typedef enum LEDs{
	RED,
	GREEN,
	YELLOW,
} led;

//Sets current State to RED.
led currentState = RED;
//Sets that the pedestrian button is not pressed.
bool pedestrian = false;

//counts up to a certain time
void counterIRQ(){
	TIMER_ClearEvent( &TimerNumber);
	timer++;
}

//Button is pressed handler
void ButtonIRQHandler(void)
{
	if(PIN_INTERRUPT_GetPinValue(&PedestrianButton) == 1){
		pedestrian = true;
		PWM_SetDutyCycle( &PedestrianLED, brightness*100 );
		
		if(currentState == YELLOW){
				PWM_SetDutyCycle( &PedestrianLED, brightness*50 );
		}
	}
	else{
		PWM_SetDutyCycle( &PedestrianLED, brightness*0 );
		pedestrian = false;
	}
}

//Counts and turns on the right LEDs.
void TimerIRQ(void){
//Turns on RED Light if current State is RED and the Pedestrian button is not pressed.
	if((currentState == RED) && (pedestrian == false)){
		if(timer <= 30){
			currentState = RED;
			counterIRQ();
		}
		else{
		timer = 0;
		currentState = GREEN;
		}
	}
	//Turns on RED Light if current State is RED and the Pedestrian button is pressed.
	else if((currentState == RED) && (pedestrian == true)){
		if(timer <= 90)
		{
		currentState = RED;
		counterIRQ();
		}
		else{
		timer = 0;
		pedestrian = false;
		currentState = GREEN;
		}
	}
	//Turns on Green Light if current State is Green
	else if(currentState == GREEN){
		if(timer <= 40){
			currentState = GREEN;
			counterIRQ();
		}
		else{
		timer = 0;
		currentState = YELLOW;
		}
	}
	//Turns on Yellow Light if current State is Yellow
	else if(currentState == YELLOW){
		if(timer <= 10){
		counterIRQ();
		currentState = YELLOW;		
			}
		else{
			timer = 0;
			currentState = RED;
		}
		}
	}

	//Main Startup
int main(void)
{
  DAVE_STATUS_t status;
	long int loop;
  status = DAVE_Init();           /* Initialization of DAVE APPs  */

  if(status != DAVE_STATUS_SUCCESS)
  {
    /* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
    XMC_DEBUG("DAVE APPs initialization failed\n");

    while(1U)
    {

    }
  }

  /* Placeholder for user application code. The while loop below can be replaced with user application code. */
  while(1U)
  {
		//Switch statement to make use of the LEDS
		switch(currentState){
			case RED:
				DIGITAL_IO_SetOutputHigh(&RedLED);
				DIGITAL_IO_SetOutputLow(&GreenLED);
				DIGITAL_IO_SetOutputLow(&YellowLED);
				break;
			case YELLOW:
				DIGITAL_IO_SetOutputHigh(&YellowLED);
				DIGITAL_IO_SetOutputLow(&GreenLED);
				DIGITAL_IO_SetOutputLow(&RedLED);
				break;
			case GREEN:
				DIGITAL_IO_SetOutputHigh(&GreenLED);
				DIGITAL_IO_SetOutputLow(&RedLED);
				DIGITAL_IO_SetOutputLow(&YellowLED);
				break;
  }
		//Loop to check if button is pressed
		for ( loop = 9999999; loop > 0; loop-- );
	}
}
