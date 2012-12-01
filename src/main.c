/*
 * zeldachest
 * MSP430-based code to play PWM sound when a switch is opened
 *
 * Copyright (c) 2012, Chris Sammis (http://csammisrun.net)
 * Source released under the MIT License (http://www.opensource.org/licenses/mit-license.php)
 *
 * Pin assignments used:
 *
 * P1.3: Switch
 * P1.6: PWM output to speaker
 *
 */

#include <msp430.h>

// da . . da da do daaaaaaaaaaaaaaaa

#define SMCLK_FREQ 125000

unsigned short freqs[] = { 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440 };
#define NOTE_A220   0
#define NOTE_Bb     1
#define NOTE_B      2
#define NOTE_C      3
#define NOTE_Cs     4
#define NOTE_D      5
#define NOTE_Eb     6
#define NOTE_E      7
#define NOTE_F      8
#define NOTE_Fs     9
#define NOTE_G      10
#define NOTE_Ab     11
#define NOTE_A440   12


// Playing state
unsigned char sound_playing;
unsigned char note = 0;

void play()
{
    sound_playing = 1;
    TACCR1 = 150; // Duty cycle (speaker volume)

    TACCR0 = SMCLK_FREQ / freqs[note];
    
    if (note == 12)
    {
        note = 0;
    }
    else
    {
        note++;
    }

    //TACCR1 = 0; // Speaker off
    sound_playing = 0;
}

#define PWMOUT BIT6
#define SWITCH BIT3

#define Interrupt(x) void __attribute__((interrupt(x)))

void debounce_switch();

int main()
{
    WDTCTL = (WDTPW + WDTHOLD);

    // Configure timers - DC0 to 1MHz and SMCLK to 1/8th that (125KHz)
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    BCSCTL2 = DIVS_3;

    P1DIR |= PWMOUT;
    P1SEL |= PWMOUT;

    // Configure the switch
    P1IE   = SWITCH;
    P1OUT |= SWITCH;
    P1REN  = SWITCH;

    // PWM setup
    TACTL = TASSEL_2 | MC_1;
    TACCTL1 = OUTMOD_7;
    TACCR1 = 0;
    
    __bis_SR_register(CPUOFF | GIE); // Switch to LPM0 and enable interrupts

    return 0;
}

Interrupt(PORT1_VECTOR) p1_isr()
{
    if (P1IFG & SWITCH)
    {
        debounce_switch();
        if (!sound_playing)
        {
            play();
        }
    }
}

void debounce_switch()
{
    P1IE  &= ~SWITCH;
    P1IFG &= ~SWITCH;

    __asm__
    (
        "mov #1000, r2  \n\t"
        "dec r2         \n\t"
        "jnz $-2        \n\t"
    );

    P1IFG &= ~SWITCH;
    P1IE  |=  SWITCH;
}



//eof

