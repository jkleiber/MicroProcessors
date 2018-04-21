#include "mbed.h"

//Serial for display and debugging
Serial pc(USBTX, USBRX);

//Some enums for state management
enum PlayerState { LEFT, RIGHT, NONE };
enum GameState { BEGIN, IN_PROGRESS, OVER };

//Game timer
Timer stopwatch;

//Game state manager
GameState gameState = BEGIN;

//Award manager
volatile int playerWin = 0;

//Player 1
InterruptIn player1Left(p28);
InterruptIn player1Right(p23);
Timeout player1Bounce;
PlayerState player1State = NONE;
PlayerState player1Pressed = NONE;
volatile int player1Steps = 0;

//Player 2
InterruptIn player2Left(p15);
InterruptIn player2Right(p18);
Timeout player2Bounce;
PlayerState player2State = NONE;
PlayerState player2Pressed = NONE;
volatile int player2Steps = 0;

//Player 1 Functions
void player1Step()
{
	player1Steps++;
	player1State = player1Pressed;
	player1Pressed = NONE;
	
	if(player1Steps >= 40)
	{
		stopwatch.stop();
		playerWin = 1;
		gameState = OVER;
	}
}

void player1LeftPress()
{
	if(player1State != LEFT && player1Pressed == NONE && gameState == IN_PROGRESS)
	{
		player1Bounce.attach(player1Step, 0.025);
		player1Pressed = LEFT;
	}
}

void player1RightPress()
{
	if(player1State != RIGHT && player1Pressed == NONE && gameState == IN_PROGRESS)
	{
		player1Bounce.attach(player1Step, 0.025);
		player1Pressed = RIGHT;
	}
}


//Player 2 Functions
void player2Step()
{
	player2Steps++;
	player2State = player2Pressed;
	player2Pressed = NONE;
	
	if(player2Steps >= 40)
	{
		stopwatch.stop();
		playerWin = 2;
		gameState = OVER;
	}
}

void player2LeftPress()
{
	if(player2State != LEFT && player2Pressed == NONE && gameState == IN_PROGRESS)
	{
		player2Bounce.attach(player2Step, 0.025);
		player2Pressed = LEFT;
	}
}

void player2RightPress()
{
	if(player2State != RIGHT && player2Pressed == NONE && gameState == IN_PROGRESS)
	{
		player2Bounce.attach(player2Step, 0.025);
		player2Pressed = RIGHT;
	}
}

int main()
{
	//Initialize Serial
	pc.baud(9600);
	
	//Attach player 1 and 2 interrupts
	player1Left.rise(player1LeftPress);
	player1Left.mode(PullDown);
	
	player1Right.rise(player1RightPress);
	player1Right.mode(PullDown);
	
	player2Left.fall(player2LeftPress);
	player2Left.mode(PullUp);
	
	player2Right.fall(player2RightPress);
	player2Right.mode(PullUp);
	
	//Notify the players that the race is about to start
	pc.printf("\r\nRunners to the blocks!\r\n");
	wait(1);
	pc.printf("Take your marks!\r\n");
	wait(1);
	pc.printf("GO!\r\n");
	stopwatch.start();
	
	gameState = IN_PROGRESS;
	
	while(1)
	{
		if(gameState == OVER)
		{
			pc.printf("Player 1: %d \tPlayer 2: %d \r\n", player1Steps, player2Steps);
			pc.printf("Player %d wins!!! It took Player %d %f seconds to finish the race!", playerWin, playerWin, stopwatch.read());
			
			while(1){}
		}
		else
		{
			pc.printf("Player 1: %d \tPlayer 2: %d \r\n", player1Steps, player2Steps);
			wait(0.25);
		}
	}
}