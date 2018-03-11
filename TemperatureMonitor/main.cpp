#include "mbed.h"

Serial pc(USBTX, USBRX);

//Setup temperature sensor
AnalogIn tVoltage(p19);

//Setup Buttons
DigitalIn currentSwitch(p21, PullDown);
DigitalIn recordSwitch(p23, PullDown);
DigitalIn resetSwitch(p25, PullDown);

//Setup 7-segment display
BusOut display(p5, p6, p7, p8, p9, p10, p11, p12);

//Manage state of program
enum TempMode {current, record, idle};
TempMode mode, nextMode;

//Detect if we need to switch state or not	
bool willSwitchState = false;
bool needsReset = false;

//Record High
float recordHighTemp = -1000.0;
float currentTemp = 0;

void displayText(char c);

int convertFloatToDigits(float num)
{
	float b = num > 0 ? 0.5 : -0.5;
	int a = abs(static_cast<int>((num * 10) + b));
	return a;
}

void displayDigit(int value, int digit, int depth)
{
	if(value > 0)
	{
		//Go to the more significant digit!
		displayDigit(value/10, value%10, depth+1);
	}
	
	//Show the decimal point
	if(depth == 0)
	{
		displayText('.');
	}
	
	if(depth >=0)
	{
		pc.printf("%d\r\n",digit);
		
		//Display the digit
		switch(digit)
		{
			case 0:
				display = 0b0111111;
				break;
			case 1:
				display = 0b0000110;
				break;
			case 2:
				display = 0b1011011;
				break;
			case 3:
				display = 0b1001111;
				break;
			case 4:
				display = 0b1100110;
				break;
			case 5:
				display = 0b1101101;
				break;
			case 6:
				display = 0b1111101;
				break;
			case 7:
				display = 0b0000111;
				break;
			case 8:
				display = 0b1111111;
				break;
			case 9:
				display = 0b1100111;
				break;
			default:
				break;
		}
		
		wait(0.75);
		display = 0;
		wait_ms(100);
	}
}

void displayText(char c)
{
	pc.printf("%c\r\n",c);
	switch(c)
	{
		case 'C':
			display = 0b0111001;
			break;
		case 'H':
			display = 0b1110110;
			break;
		case '-':
			display = 0b1000000;
			break;
		case '.':
			display = 0b10000000;
			break;
		default:
			break;
	}
	wait(0.75);
	display = 0;
	wait_ms(100);
}

float readTemperature()
{
	float totalTemp = 0;
	for(int i = 0; i < 1000; ++i)
	{
		float readTemp = ((tVoltage*3.3) - 0.5) / 0.01;
		totalTemp += readTemp;
	}
	
	totalTemp /= 1000;
	
	return totalTemp;
}

void pollSwitches()
{
	if(currentSwitch == 1)
	{
		wait(0.05);
		if(currentSwitch == 1)
		{
			nextMode = current;
		}
	}
	
	if(recordSwitch == 1)
	{
		wait(0.05);
		if(recordSwitch == 1)
		{
			nextMode = record;
		}
	}
	
	if(resetSwitch == 1)
	{
		wait(0.05);
		if(resetSwitch == 1)
		{
			needsReset = true;
		}
	}
}

int main() 
{
	//Start in the current temperature state
	mode = idle;
	nextMode = current;
	
	pc.baud(9600);
	
    while(1) 
    {
		//Display the current temperature
		if(mode == current)
		{
			pc.printf("C-Temp: %f\r\n", currentTemp);
			
			if(currentTemp < 0)
			{
				displayText('-');
			}
			displayDigit(convertFloatToDigits(currentTemp), -1, -1);
			displayText('C');
			
			mode = idle;
		}
		else if(mode == record) //Display the record temperature
		{
			pc.printf("H-Temp: %f\r\n", recordHighTemp);
			
			if(recordHighTemp < 0)
			{
				displayText('-');
			}
			displayDigit(convertFloatToDigits(recordHighTemp), -1, -1);
			displayText('H');
			
			mode = idle;
		}
		else
		{
			//Turn off the display to measure temperatures.
			display = 0;
			
			//Check the new temperature
			currentTemp = readTemperature();
			
			//Is this temperature a record
			if(currentTemp > recordHighTemp || needsReset)
			{
				recordHighTemp = currentTemp;
			}
			mode = nextMode;
		}
		
		pollSwitches();	
    }
}