#include <Arduino.h>

#include <stdio.h>
#include <math.h>
#include <avr/io.h>
#include "hd44780/HD44780.hpp"


void ADC_Init(void);

char str[32];
int adc_value; //Variable to store the value read from the ADC converter (0 to 1023)
int tempC;
int tempF;

int main(void){
	// INIT FUNCTIONS
	ADC_Init();
	LCD_Initalize();


	// LED CONTROLLER
	DDRB |= ((1 << DDB2) | (1 << DDB3) | (1 << DDB4)| (1 << DDB5)); // Direction of port line (1 - output)
	PORTB |= (1<<PORTB2) | (1<<PORTB3) | (1<<PORTB4)| (1 << DDB5); // Output register (1 - do not light)

	// BUTTON CONTROLLER
	DDRD &= ~(1<<DDD0) | ~(1<<DDD1)| ~(1<<DDD2); // Direction of port line (0 - input)

	// for loop
	for(;;){
		// Input channel selections, PORT PC1;
		ADMUX |= (1<<MUX0);
		ADMUX &= ~(1<<MUX1) | ~(1<<MUX2) | ~(1<<MUX3); // MUX - input Channel Selections (0000 - ADC0)
		ADCSRA |= (1<<ADSC); //Start converting

		adc_value = ADCW;    //Read the ADC value

		// We calculate the temperature, in Celsius, with the formula
		tempC = (5.0 * adc_value * 100.0)/1024.0;

		// First button for Celsius
		if (!(PIND & 1<<PIND3)){
			LCD_Clear();
			sprintf(str, "Celsius: %d", tempC);
			LCD_GoTo(0,0);
			LCD_WriteText(str);
			_delay_ms(1000);
		}

		// Second button for Fahrenheit
		else if (!(PIND & 1<<PIND2)){
			tempF = (tempC*1.8)+32;

			LCD_Clear();
			sprintf(str, "Fahrenheit: %d", tempF);
			LCD_GoTo(0,0);
			LCD_WriteText(str);
			_delay_ms(1000);
		}

		// Show the temperature in the LEDS
		else if (!(PIND & 1<<PIND1)){
			if (tempC < 24){
				PORTB = PORTB*2;
				_delay_ms(1000);
				PORTB |= (1<<PORTB2) | (1<<PORTB3) | (1<<PORTB4)| (1 << DDB5);
			}
			else if (tempC == 24){
				PORTB = PORTB*3;
				_delay_ms(1000);
				PORTB |= (1<<PORTB2) | (1<<PORTB3) | (1<<PORTB4)| (1 << DDB5);
			}

			else if (tempC > 24){
				PORTB = PORTB*4;
				_delay_ms(1000);
				PORTB |= (1<<PORTB2) | (1<<PORTB3) | (1<<PORTB4)| (1 << DDB5);
			}

		}

		// Ask if we want to see the temperature in Celsius or Fahrenheit
		else {
			sprintf(str, "PIND3 Celsius");
			LCD_GoTo(0,0);
			LCD_WriteText(str);
			sprintf(str, "PIND2 Fahrenheit");
			LCD_GoTo(0,1);
			LCD_WriteText(str);
		}

	}
}

void ADC_Init(void){
	ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));    //Prescaler at 128 so we have an 125Khz clock source
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);                //Avcc(+5v) as voltage reference
	ADCSRB &= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0));    //ADC in free-running mode
	ADCSRA |= (1<<ADATE);                //Signal source, in this case is the free-running
	ADCSRA |= (1<<ADEN);                //Power up the ADC
}
