#include "mbed.h"

#define SWITCH_BOUNCE_WAIT 75
#define SWITCH_HOLD_WAIT 225
//LEDs
BusOut redBus(p21, p22, p23); // Control all the red LEDs on a bus (Active LOW)
BusOut greenBus(p18, p19, p20); // Control the Green LEDs on a bus
BusOut blueBus(LED1, LED2, LED3, LED4); //Control all the blue LEDs on a bus

//Switches
DigitalIn brSwitch(p5, PullUp); //Active High
DigitalIn grSwitch(p10, PullUp); //Active High
DigitalIn gbSwitch(p30); //Active Low

//States of the LEDs
short redState = 3, blueState = 4, greenState = 3;
const short redMax = 3, blueMax = 4, greenMax = 3;
	
void decrementState(short &state, const short stateMax)
{
	//If the state can be decreased without going below 0, do so.
	//Otherwise, reset to the maximum value
	state = (state - 1) < 0 ? stateMax : state - 1;
}

void updateLEDs()
{
	int redOut = 0, greenOut = 0, blueOut = 0;
	
	//Calculate the red LED bus value
	for(int r = 0; r < redMax; ++r)
	{
		//If this device on the bus should be off
		if(r >= redState)
		{
			redOut = (redOut << 1) | 1; //Shift left and OR with 1, adds 1
		}
		//Otherwise turn it on!
		else
		{
			redOut = redOut << 1; //Shift left, adds 0
		}
	}
	
	//Calculate green LED bus value
	for(int g = 0; g < greenMax; ++g)
	{
		//If this device on the bus should be off
		if(g >= greenState)
		{
			greenOut = greenOut << 1; //Shift left, adds 0
		}
		//Otherwise turn it on!
		else
		{
			greenOut = (greenOut << 1) | 1; //Shift left and OR with 1, adds 1
		}
	}
	
	//Calculate blue LED bus value
	for(int b = 0; b < blueMax; ++b)
	{
		//If this device on the bus should be off
		if(b >= blueState)
		{
			blueOut = blueOut << 1; //Shift left, adds 0
		}
		//Otherwise turn it on!
		else
		{
			blueOut = (blueOut << 1) | 1; //Shift left and OR with 1, adds 1
		}
	}
	
	//Output to the buses
	redBus = redOut;
	greenBus = greenOut;
	blueBus = blueOut;
}
	
/**
 * Function that checks to see which switches are pressed.
 * If a certain switch is pressed, this fucntion waits to eliminate
 * switch bounce, and then decreases the state while the switch 
 * stays pressed.
 *
 * This also updates the LEDs throughout the program's lifetime
 */
void pollSwitches()
{
	//If the BR switch is pressed
	if(brSwitch == 0)
	{
		//Wait 50 ms to be sure
		wait_ms(SWITCH_BOUNCE_WAIT);
		
		//Wait some to be sure there isn't switch bounce
		while(brSwitch == 0)
		{
			//Decrease the count of ON LEDs
			decrementState(redState, redMax);
			decrementState(blueState, blueMax);
			
			//Update the LEDs
			updateLEDs();
			
			//Wait some so user can unpress button
			wait_ms(SWITCH_HOLD_WAIT);
		}
	}
	
	//If the GR switch is pressed
	if(grSwitch == 0)
	{
		//Wait some to be sure there isn't switch bounce
		wait_ms(SWITCH_BOUNCE_WAIT);
		
		//While the switch stays pressed
		while(grSwitch == 0)
		{
			//Decrease the count of ON LEDs
			decrementState(redState, redMax);
			decrementState(greenState, greenMax);
			
			//Update the LEDs
			updateLEDs();
			
			//Wait a bit so user can unpress button
			wait_ms(SWITCH_HOLD_WAIT);
		}
	}
	
	//If the BR switch is pressed
	if(gbSwitch == 1)
	{
		//Wait some to be sure there isn't switch bounce
		wait_ms(SWITCH_BOUNCE_WAIT);
		
		//While the switch stays pressed
		while(gbSwitch == 1)
		{
			//Decrease the count of ON LEDs
			decrementState(greenState, greenMax);
			decrementState(blueState, blueMax);
			
			//Update the LEDs	
			updateLEDs();
			
			//Wait some time so user can unpress button
			wait_ms(SWITCH_HOLD_WAIT);
		}
	}
}

int main()
{
	//Initialize all LEDs to ON. Start the game!
	updateLEDs();
	
	while(1)
	{
		/* Spend all the time in the program waiting for switches
			to be pressed so something can happen */
		pollSwitches();
	}
}