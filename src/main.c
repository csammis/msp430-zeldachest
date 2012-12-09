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

#define Interrupt(x) void __attribute__((interrupt(x)))

void hard_delay()
{
    __asm__
    (
        "mov #1000, r2  \n\t"
        "dec r2         \n\t"
        "jnz $-2        \n\t"
    );
}

#define SMCLK_FREQ 125000

// Half-steps from A220 to A440
unsigned short frequencies[] = { 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440 };
#define REST        0xFF
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

// da . . da da do daaaaaaaaaaaaaaaa
unsigned short notes[]    = { NOTE_G, REST, REST, NOTE_G, NOTE_G, NOTE_Fs, NOTE_G };
unsigned short duration[] = { 1,      1,    1,    1,      1,      1,       6 };
#define NUM_NOTES 7

// Technically this is the PWM duty cycle, not a switch. ON = 1 just doesn't deafen me.
#define SPEAKER_ON 1
#define SPEAKER_OFF 0

// Playing state
unsigned char sound_playing;
unsigned char curr_note;
unsigned short curr_duration;
unsigned char time_to_switch;

void play()
{
    sound_playing = 1;
    curr_note = -1;
    time_to_switch = 1;

    // Start the WDT in interval mode based off of SMCLK (125Khz) / 32768 = about 4Hz
    WDTCTL = (WDTPW + WDTTMSEL + WDTCNTCL + WDTIS0);
    IFG1 &= ~WDTIFG;
    IE1  |=  WDTIE;
}

Interrupt(WDT_VECTOR) wdt_isr()
{
    if (time_to_switch)
    {
        TACCR1 = SPEAKER_OFF;

        // Get the next note
        curr_note++;
        if (curr_note >= NUM_NOTES)
        {
            // Disable WDT interval interrupts and stop the timer
            IE1 &= ~WDTIE;
            WDTCTL = (WDTPW + WDTHOLD);
            sound_playing = 0;
        }
        else
        {
            unsigned short note = notes[curr_note];
            curr_duration = duration[curr_note];

            // Delay a bit for articulation
            hard_delay();

            if (note != REST)
            {
                TACCR0 = SMCLK_FREQ / frequencies[note];
                TACCR1 = SPEAKER_ON;
            }

            time_to_switch = 0;
        }
    }
    else
    {
        if (curr_duration > 1)
        {
            time_to_switch = 0;
            curr_duration--;
        }
        else
        {
            time_to_switch = 1;
        }
    }

    IFG1 &= ~WDTIFG;
}

#define PWMOUT BIT6
#define SWITCH BIT3

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

    hard_delay();

    P1IFG &= ~SWITCH;
    P1IE  |=  SWITCH;
}



//eof

