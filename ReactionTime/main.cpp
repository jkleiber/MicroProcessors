#include "mbed.h"

//SPI variables
SPI spi(p11, p12, p13);
DigitalOut sevenSegmentSelect(p14);
DigitalOut switchSelect(p15);

//Random number seeding
Timer timer;

//Round time control
Timeout roundTimeout;
float timeLimit = 1.0;

//Round states
enum RoundState {IDLE, PLAY, END};
RoundState roundState = IDLE;
bool roundWin = false;

//Current goal letter
char currentLetter;

char generateLetter(float timerSeed)
{
	srand(static_cast<unsigned int>(timerSeed));
	
	int r = rand()%4;
	
	char c = 'A' + r;
	
	return c;
}

uint8_t mapLetterToInt(char c)
{
	switch(c)
	{
		case 'A':
			return 0b11101110;//0b0110111;
		case 'B':
			return 0b11111110;//0b1111111;
		case 'C':
			return 0b10011100;//0b0111001;
		case 'D':
			return 0b11111100;//0b0111111;
		case 'L':
			return 0b00011100;//0b0111000;
		default:
			return 0;
	}
}

void writeToSevenSegment(uint8_t value)
{
	//Choose proper mode
	spi.format(8, 0);
	
	//Select this slave
	sevenSegmentSelect = 0;
	
	//Write value
	spi.write(value);
	
	//Stop selecting this slave
	sevenSegmentSelect = 1;
}

uint8_t readSwitches()
{
	//Choose proper format
	spi.format(8, 2);
	
	//Select the slave
	switchSelect = 1;
	
	//Read Data
	uint8_t data;
	data = spi.write(0);
	
	//Stop selecting the slave
	switchSelect = 0;
	
	return data;
}

void roundEnd()
{
	if(roundWin)
	{
		roundState = IDLE;
		timeLimit *= 0.8;
	}
	else
	{
		roundState = END;
	}
}

int main()
{	
	//Start the timer
	timer.start();
	
	//Set the slaves to inactive
	sevenSegmentSelect = 1;
	switchSelect = 0;
	
	int switchesPress;
	
	while(1)
	{
		switch(roundState)
		{
			case IDLE:
				writeToSevenSegment(0);
				wait(1);
				roundState = PLAY;
				currentLetter = generateLetter(timer.read());
				writeToSevenSegment(mapLetterToInt(currentLetter));
				roundTimeout.attach(roundEnd, timeLimit);
				roundWin = false;
				break;
			case PLAY:
				switchesPress = readSwitches();

				//If multiple switches are pressed, game over
				if(switchesPress != 0 && switchesPress != 1 && switchesPress != 2 && switchesPress != 4 && switchesPress != 8)
				{
					roundTimeout.detach();
					roundState = END;
				}
				//If the correct switch was pressed
				else if(switchesPress != 0 && (switchesPress >> (currentLetter - 'A')) & 1)
				{
					roundWin = true;
					roundTimeout.detach();
					roundState = IDLE;
					timeLimit *= 0.8;
				}
				//If the incorrect switch was pressed
				else if(switchesPress != 0)
				{
					roundTimeout.detach();
					roundState = END;
				}
				
				break;
			case END:
				timeLimit = 1.0;
				writeToSevenSegment(mapLetterToInt('L'));
				while(1){}
				break;
			default:
				roundState = IDLE;
				break;
		}
	}
}