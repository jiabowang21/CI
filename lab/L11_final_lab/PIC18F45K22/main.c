/* Main.c file generated by New Project wizard
 *
 * Created:   2020
 * Processor: PIC18F45K22
 * Compiler:  MPLAB XC8
 */

#include <xc.h>
#define _XTAL_FREQ 8000000  

#include <string.h>
#include "config.h"
#include "GLCD.h"

int pos[10];

void dibuixar_linia() {
   for (int i = 0; i < 128; ++i) {
      SetDot(32, i); 
   }
}

void configGLCD(){
   ANSELD = 0x00;			//PORTB configurat com a digital
   ANSELB = 0x00;			//PORTD configurat com a digital
   PORTD = 0x00; 		   	//Valors inicials al PORTD
   PORTB = 0x00;  			//Valors inicials al PORTB
   TRISD = 0x00;		   	//PORTD configurat com a sortida
   TRISB = 0x00;			//PORTB configurat com a sortida
   GLCDinit();		   		//Inicialitzem la pantalla	
   clearGLCD(0,7,0,127);      		//Esborrem la pantalla
   setStartLine(0);           		//Definim la linia d'inici
}

void configPIC() {
   TRISA = 0xFF; 			//PORTA configurat com a entrada
   ANSELA = 0xFF;			//PORTA configurat com a anal�gic
   ADCON2bits.ADCS0 = 1;		//A/D Conversion Clock = Fosc/8
   ADCON2bits.ADCS1 = 0;		//A/D Conversion Clock = Fosc/8
   ADCON2bits.ADCS2 = 0;		//A/D Conversion Clock = Fosc/8
   ADCON1bits.PVCFG0 = 0;		//A/D connected to internal signal AVdd
   ADCON1bits.PVCFG1 = 0;		//A/D connected to internal signal AVdd
   ADCON1bits.NVCFG0 = 0;		//A/D connected to internal signal AVss
   ADCON1bits.NVCFG1 = 0;		//A/D connected to internal signal AVss
   ADCON0bits.CHS0 = 0; 		//AN0 as analog channel
   ADCON0bits.CHS1 = 0;			//AN0 as analog channel
   ADCON0bits.CHS2 = 0;			//AN0 as analog channel
   ADCON0bits.CHS3 = 0;			//AN0 as analog channel
   ADCON0bits.CHS4 = 0;			//AN0 as analog channel	 
   ADCON2bits.ACQT0 = 0;		//Tacq = 4 Tad
   ADCON2bits.ACQT1 = 1;		//Tacq = 4 Tad
   ADCON2bits.ACQT2 = 0;		//Tacq = 4 Tad
   ADCON2bits.ADFM = 1;			//Right justified
   ADCON0bits.ADON = 1;			//Enable ADC
}

void main(void) {
   configGLCD();
   configPIC();
   dibuixar_linia();
   int valor;
   int percentatge;
   int i = 0;
   int percentatge2 = 0;
   int valor2;
   int iteracion = 0;
   char inici = '1';
   int i2 = 0;
   while (1) {
      if (i == 127) {
	 i = 0;
	 clearGLCD(0,3,0,127);
	 clearGLCD(5,7,0,127);
      }
      __delay_ms(100);
      ADCON0bits.GO = 1; 		// Demanem una conversi�
      while(ADCON0bits.GO){} 		// Espera activa a que ens confirmi la lectura
      valor = ADRESH << 8;
      valor += ADRESL;
      percentatge = valor/32;
      SetDot(31-percentatge, i);
      if (i2 == 10) i2 = 0;
      pos[i2] = valor;
      if (iteracion == 9 && inici == '1') {
	 valor2 = (pos[0] + pos[1] + pos[2] + pos[3] + pos[4] + pos[5] + pos[6] + pos[7] + pos[8] + pos[9]) /10;
	 iteracion = 0;
	 inici = '0';
      }
      if (inici == '0') {
	 valor2 = (pos[0] + pos[1] + pos[2] + pos[3] + pos[4] + pos[5] + pos[6] + pos[7] + pos[8] + pos[9]) /10;
      }
      percentatge2 = valor2/32;
      if (i > 3) SetDot(63-percentatge2, i);
      ++iteracion;
      ++i;
      ++i2;
   }
}