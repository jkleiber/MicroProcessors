#include "mbed.h"

#define SWITCH_BOUNCE_WAIT 50

//LEDs
PwmOut led1(LED1);
PwmOut led2(LED2);
PwmOut led3(LED3);
PwmOut led4(LED4);
PwmOut redLed(p22);

//Piezo
PwmOut piezo(p21);

//Buttons
DigitalIn pewButton(p19, PullDown);
DigitalIn alertButton(p20, PullDown);

enum ToyState{idle, pewpew, redalert};
ToyState state = idle;

void setIdle()
{
    //Turn everything off
    piezo = 0;  
    led1 = 0;
    led2 = 0;
    led3 = 0;
    led4 = 0;
    redLed = 0;
}

void setPewPew()
{
    float T;
    for(int f = 1760; f >= 880; f -= 20)
    {
        T = 1.0 / f;
        piezo.period(T);
        piezo = 0.5;
        
        if(f <= 1760 && f > 1540)
        {
            led1 = (1760 - f) / 220.;
        }
        else if(f <= 1540 && f > 1320)
        {
            led2 = (1540 - f) / 220.;
        }
        else if(f <= 1320 && f > 1100)
        {
            led3 = (1320 - f) / 220.;
        }
        else
        {
            led4 = (1100 - f) / 220.;
        }
        
        wait_us(11363);
    }
    
    //Turn LEDs Off
    led1 = 0;
    led2 = 0;
    led3 = 0;
    led4 = 0;
}

void setRedAlert()
{
    float T;
    
    for(int f = 440; f <= 880; f += 5)
    {
        T = 1.0 / f;
        piezo.period(T);
        piezo = 0.5;
        
        redLed = (f - 440) / 440.;
        wait_us(11363);
    }
    
    piezo = 0;
    redLed = 0;
    
    wait(0.5);
}

//Manages the outputs of the system
void stateMachine()
{
    switch(state)
    {
        case idle:
            setIdle();
            break;
        case pewpew:
            setPewPew();
            break;
        case redalert:
            setRedAlert();
            break;
        default:
            state = idle;
            break;
    }  
}

//Checks to see which buttons are pressed
void pollButtons()
{
    if(pewButton == 1)
    {
        wait_ms(SWITCH_BOUNCE_WAIT);
            
        while(pewButton == 1)
        {
            state = pewpew;
            stateMachine();
        }
    }
    
    if(alertButton == 1)
    {
        wait_ms(SWITCH_BOUNCE_WAIT);
        
        while(alertButton == 1)
        {
            state = redalert;
            stateMachine();
        }
    }
    
    //Reset to idle once the buttons are done being pressed
    state = idle;
    stateMachine();
}

int main() 
{
    float T = 1.0/1760;
    piezo.period(T);
    
    while(1) 
    {
        pollButtons();
    }
}
