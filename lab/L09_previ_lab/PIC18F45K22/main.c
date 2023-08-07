/* Main.c file generated by New Project wizard
 *
 * Created:   2020
 * Processor: PIC18F45K22
 * Compiler:  MPLAB XC8
 */

#include <xc.h>
#define _XTAL_FREQ 	8000000
#define EST_RUNNING 	0
#define EST_INIT 	1
#define EST_STOPPED 	0

#include <string.h>
#include "config.h"
#include "GLCD.h"

const unsigned int decimes_init = 102;	//valor inicial

int hores;				//indica el valor de les hores del temporitzador
int minuts;				//indica el valor dels minuts del temporitzador
int segons;				//indica el valor dels segons del temporitzador
int decimes;				//indica el valor de les d�cimes del temporitzador

unsigned int RUNNING = EST_RUNNING; 	//0 si no est� en RUNNING i 1 si ho est� 
unsigned int INIT = EST_INIT;		//0 si no est� en INIT i 1 si ho est� 
unsigned int STOPPED = EST_STOPPED; 	//0 si no est� en STOPPED i 1 si ho est� 

char inici_cicle = '1';			//char que actua com a bole� per controlar l'entrada/sortida del RSI
char funcionamentc = '0';  		//char que actua com a bole� i controla el funcionament del programa
char c1 = '0';				//char que actua com a bole� i controla el flanc de pujada del bot� associat al RC0

//funci� que converteix en hores, minuts i segons, el valor en decimes inicial
void config_temps() {
   hores = (decimes_init / 36000);	
   minuts = (decimes_init/600) - (hores*60);
   segons = (decimes_init/10) - (hores*3600) - (minuts*60);
   decimes = decimes_init - (hores*36000) - (minuts*600) - (segons*10);
}

//interrupci� d'alta prioritat associat per al timer 0
void interrupt high_priority tic(void) {
   if(INTCONbits.TMR0IF == 1 && INTCONbits.TMR0IE == 1) {
      inici_cicle = '1';
      /*
      El timer0 ha de tenir el valor 0XF3CB, que en decimal es 62411. I surt 
      de la f�rmula T(rsi) = n* T(cicle_clock) * PRESCALER  => 0.1s = n * 1/(8000000/4) * 64 => n = 3125
      llavors com que el timer0 �s de 16 bits, 2^16 - 3125 = 62411 i en hexadecimal es 0xF3CB 
      */
      TMR0H = 0xF3;
      TMR0L = 0xCB;
      INTCONbits.TMR0IF = 0;
   }
}

//funci� que mostra el valor del comptador de temps en el format hh:mm:ss.d
void mostrar_numero() {	
   if (decimes < 0) {	//quan queden menys de una d�cima de segon, torna a 9 d�cimes de sesgons
      decimes = 9;
      if (segons >= 0) --segons;//si queden segons, es resta
      else if (minuts >= 0) {	//si no, es resten minuts
	 --minuts;
	 segons = 59;
      }
      else if (hores >= 0) {	//si no, es resten hores
	 --hores;
	 minuts = 59;
      }
   }
   if (segons < 0) {	//quan queden menys de un segon, passa a 9 d�cimes de sesgons
      decimes = 9;
      if (minuts >= 0) {//si quedaven minuts, es resten
	 --minuts;
	 segons = 59;
      }
      else if (hores >= 0) {//si no es resten hores
	 --hores;
	 minuts = 59;
      }
   }
   if (minuts < 0) {	//quan queden menys de un minut, passa a 59 segons
      segons = 59;
      if (hores >= 0) {	//si encara quedaven hores, es resten
	 --hores;
	 minuts = 59;
      }
   }
   if (hores < 0) {	//quan queden menys de una hora, passa a 59 minuts
      hores = 0;
      minuts = 59;
   }
   if (hores < 10) {	//si hores nom�s t� un valor (menor que 10), afegeixo un valor 0 al seu esquerre
      writeNum(6, 0, 0);
      writeNum(6, 1, hores);
   }
   if (minuts < 10) {	//si minuts nom�s t� un valor (menor que 10), afegeixo un valor 0 al seu esquerre
      writeNum(6, 3, 0);
      writeNum(6, 4, minuts);
   }
   if (segons < 10) {	//si segons nom�s t� un valor (menor que 10), afegeixo un valor 0 al seu esquerre
      writeNum(6, 6, 0);
      writeNum(6, 7, segons);
   }
   if (segons > 9) {	//si segons t� dos valors (major que 9), el mostro per pantalla directament
      writeNum(6, 6, segons);
   }
   if (minuts > 9) {	//si minuts t� dos valors (major que 9), el mostro per pantalla directament
      writeNum(6, 3, minuts);
   }
   if (hores > 9) {	//si hores t� dos valors (major que 9), el mostro per pantalla directament
      writeNum(6, 0, hores);
   }
   writeNum(6, 9, decimes);
   putchGLCD(6, 2, ':');
   putchGLCD(6, 5, ':');
   putchGLCD(6, 8, '.');
}

//cont� totes les configuracions necess�ries per al bon funcionament del programa
void config() {
   T0CONbits.TMR0ON = 1;	//Enables timer 0
   T0CONbits.T08BIT = 0;	//Timer 0 is configured as a 16-bit timer
   T0CONbits.T0CS = 0;		//Instruction cycle clock
   T0CONbits.T0SE = 1;		//Increment on rising edge transition on T0CKL
   T0CONbits.PSA = 0;		//Timer0 prescaler is assigned
   T0CONbits.T0PS2 = 1;
   T0CONbits.T0PS1 = 0;		//1:64 prescaler value
   T0CONbits.T0PS0 = 1;
   INTCONbits.GIE = 1;		//Enables all interrupts
   INTCONbits.PEIE = 1; 	//Enables peripheral interrupts 
   INTCONbits.TMR0IE = 1;	//Enables the timer 0 overflow interrupt
   INTCON2bits.TMR0IP = 1;	//Timer 0 overflow interrupt high priority
   /*
      El timer0 s'ha d'inicialitzar amb el valor 0XF3CB, que en decimal es 62411. I surt 
      de la f�rmula T(rsi) = n* T(cicle_clock) * PRESCALER  => 0.1s = n * 1/(8000000/4) * 64 => n = 3125
      llavors com que el timer0 �s de 16 bits, 2^16 - 3125 = 62411 i en hexadecimal es 0xF3CB 
   */
   TMR0H = 0xF3;
   TMR0L = 0xCB;
   ANSELC = 0;			//PORTC configurat com Digital
   ANSELD = 0;                  //PORTD configurat com Digital
   ANSELB = 0;                  //PORTB configurat com Digital
   PORTC = 0x00;		//Donem uns valors inicials als ports C, D i B
   PORTD = 0x00; 		  
   PORTB = 0x00;
   TRISC = 0xFF;		//PORTC configurat com a entrada
   TRISD = 0x00;		//PORTD configurat com a sortida
   TRISB = 0x00;		//PORTB configurat com a sortida
   GLCDinit();		   	//Inicialitzem la pantalla
   clearGLCD(0,7,0,127);      	//Esborrem pantalla
   setStartLine(0);           	//Definim linia d'inici
   mostrar_numero();
}

//Funci� que controla els flancs ascendents del bit RC0 i executa el seu corresponent codi per al bon funcionament del progama
void funcionament_programa() {
   if (PORTCbits.RC0 == 1 && c1 == '0') {	//prenem bot� RC0
      if (INIT == 1) {	//si estaba en el mode INIT, passa a estar al mode RUNNING
	 INIT = 0;
	 RUNNING = 1;
      }
      else if (RUNNING == 1) {
	 RUNNING = 0;	//si estaba en el mode RUNNING, passa a estar al mode STOPPED
	 STOPPED = 1;
      }
      else if (STOPPED == 1) {
	 STOPPED = 0;	//si estaba en el mode STOPPED, passa a estar al mode INIT
	 INIT = 1;
      }
      funcionamentc = '1';
      c1 = '1';
      __delay_ms(5);		//per controlar els rebots
   }
   if (funcionamentc == '1' && INIT == 1) {	//mode INIT
      config_temps();			//resetegem l'hora, els minuts, els segons i les decimes posant-los al valor inicial
      mostrar_numero();			//mostrem els valors
   }
   if (funcionamentc == '1' && RUNNING == 1) {//mode RUNNING
      if (decimes == 0 && segons == 0 && minuts == 0 && hores == 0) { //si tots tenen 0, llavors es queden al 0
	 mostrar_numero();
      }
      else {
	 --decimes;				 //decrementem una decima al comptador
	 mostrar_numero();      		 //mostrem els valors
      }
   }
   if (funcionamentc == '1' && STOPPED == 1) {//mode STOPPED
      mostrar_numero();			 //mostrem els valors actuals
   }
   if (PORTCbits.RC0 == 0) {
      c1 = '0';
   }
}

void main(void) {
   config_temps();
   config();
   while (1) {
      while(inici_cicle == '0');	//espera activa, surt quan li arriba el enable flag al timer0 (que s'executa cada 0.1 segons)
      inici_cicle = '0';		//el tornem a posar a '0'
      funcionament_programa();		//comen�a a executar-se el programa
   }
}