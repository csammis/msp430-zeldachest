/*
 * zeldachest
 * MSP430-based code to play PWM sound when a switch is opened
 *
 * Copyright (c) 2012, Chris Sammis (http://csammisrun.net)
 * Source released under the MIT License (http://www.opensource.org/licenses/mit-license.php)
 *
 */

#include <msp430.h>

int main()
{
    WDTCTL = (WDTPW + WDTHOLD);
    
    __bis_SR_register(CPUOFF | GIE); // Switch to LPM0 and enable interrupts

    return 0;
}

//eof

