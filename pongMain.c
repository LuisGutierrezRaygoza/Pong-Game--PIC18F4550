/*
 * File:   pong.c
 * Author: frayg
 *
 * Created on April 23, 2024, 7:53 PM
 */
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define _XTAL_FREQ 8000000

 
#pragma config FOSC = INTOSC_EC    //internal oscillator, CLKOUT on RA6, EC used by USB
//#pragma config FOSC = INTOSCIO_EC	// Internal oscillator, port function on RA6, EC used by USB
//#pragma configFOSC = ECIO_EC	// External clock, port function on RA6, EC used by USB
// #pragma configFOSC = EC_EC		// External clock, CLKOUT on RA6, EC used by USB
#pragma config PLLDIV = 1   //// No prescale 
// #pragma config FOSC = HSPLL_HS  //10MHz x 4 gives 40MHz clock
#pragma config FCMEN = OFF   //no fail safe clock monitor 
#pragma config IESO = OFF   //oscillator switchover disabled 
#pragma config PWRT = ON           //oscillator power up timer enabled (release version only) 
#pragma config BOR = OFF      //hardware brown out reset 
#pragma config WDT = OFF   //watchdog timer disabled 
#pragma config MCLRE = ON    //MCLR pin enabled 
#pragma config LPT1OSC = ON  //timer1 low power operation 
#pragma config PBADEN = OFF   //portB 0to 4 digital - not analogue 
#pragma config LVP = OFF     //low voltage programming disabled 
#pragma config CCP2MX = OFF  //portc1 = CCP2 
#pragma config XINST = OFF         //do not allow PIC18 extended instructions 
#pragma config  STVREN = ON         //stack overflow will cause reset 
#pragma config CP0 = OFF      //code protection block 0 
#pragma config CP1 = OFF 
#pragma config CP2 = OFF 
#pragma config CP3 = OFF 
#pragma config CPB = OFF      //code protection boot block 
#pragma config CPD = OFF      //code protection data EEPROM 
#pragma config WRT0 = OFF      //write protection block 0 
#pragma config WRT1 = OFF 
#pragma config WRT2 = OFF 
#pragma config WRT3 = OFF 
#pragma config WRTB = OFF      //write protection boot block 
#pragma config WRTC = OFF      //write protection configuration register 
#pragma config WRTD = OFF      //write protection data EEPROM 
#pragma config EBTR0 = OFF      //table read protection block 0 
#pragma config EBTR1 = OFF 
#pragma config EBTR2 = OFF 
#pragma config EBTR3 = OFF 
#pragma config EBTRB = OFF      //table read protection boot block

#define filas LATB
#define columnas LATD 
#define btn1izq PORTCbits.RC1
#define btn1der PORTCbits.RC0
#define btn2izq PORTCbits.RC6
#define btn2der PORTCbits.RC7
struct punto {   
  int posicionX,posicionY;
  int direccionX, direccionY; //y = -1 arriba, y = 0 abajo,x0irsederecho,-1izq
  int velocidad;
  int velocidadMin;
};

struct jugador{
    int posicion,puntos;
};
struct MODO{
    int PVP, dificultad, velocidad;
};


int barridoColumnas[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
int barridoFilas[8] = {0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};
int barridoPaleta[7] = {0x03,0x06,0x0C,0x18,0x30,0x60,0xC0};
int i,j,k;
int puntos2[8] = {0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80};
int puntos1[8] = {0xFF,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x01};
int uno[5] = {0xBB,0xB9,0x81,0xBF,0xBF};
int dos[4] = {0x9B,0xAD,0xB5,0xBB};
int barridoPVP[8] = {0xE1,0xF9,0xF7,0xEF,0xF7,0xE1,0xF9,0xFF};
int barridoPVC[8] = {0xE1,0xF9,0xF7,0xEF,0xF7,0xE1,0xED,0xFF};
int barridoEasy[8] = {0x4B,0x44,0x04,0xFF,0xE8,0x18,0xE0,0xFF};
int barridoMedium[8] = {0xF0,0xFD,0xF0,0x3F,0x0B,0xF4,0xF4,0xFF};
int barridoHard[8] = {0x00,0xCD,0x10,0xFF,0x01,0x6A,0x91,0xFF};
int barridoHell[8] = {0x10,0x7D,0x70,0xFF,0x10,0x72,0x976,0xFF};

struct punto pelota ;
struct jugador jugador1, jugador2;
int ganadorAnterior = 2; 
struct MODO modo;

void inicio(){
    jugador1.posicion = 3;
    jugador2.posicion = 3;
    pelota.posicionX =3;
    pelota.posicionY = 4;
    pelota.direccionX = 0;
    if(ganadorAnterior == 1){
        pelota.direccionY = -1;
    }
    else{
        pelota.direccionY = 1;
    }
    for(i = 0;i < 70;i++){  //cada pantalla dura alrededor de 3ms, 300 un segundo
        filas = barridoFilas[0];
        columnas = barridoPaleta[jugador2.posicion];
        __delay_ms(1);
        filas = barridoFilas[pelota.posicionY];
        columnas = barridoColumnas[pelota.posicionX];
        __delay_ms(1);
        filas = barridoFilas[7];
        columnas = barridoPaleta[jugador1.posicion];
        __delay_ms(1);
    }
        
    
};

void imprimir(){
    filas = barridoFilas[0];
    columnas = barridoPaleta[jugador2.posicion];
    __delay_us(700);
    filas = barridoFilas[pelota.posicionY];
    columnas = barridoColumnas[pelota.posicionX];
    __delay_us(700);
    filas = barridoFilas[7];
    columnas = barridoPaleta[jugador1.posicion];
    __delay_us(700);
};

void juego(){
    int finPartida = 1,contador1 = 0,contador2 = 0, contador = 0;
    int perder = 0;
    while(finPartida){
        imprimir();
        contador++;
        contador1++;
        contador2++;
        
        
        if(contador1 > 4){
            if(btn1izq == 1){
                contador1 = 0;
                if(jugador1.posicion != 0){
                    jugador1.posicion--;
                }
            }
            if(btn1der == 1){
                contador1 = 0;
                if(jugador1.posicion != 6){
                    jugador1.posicion++;
                }
            }
        }
        
        
        if(contador2 > 4 && (modo.PVP == 1)){
            if(btn2izq == 1){
                contador2 = 0;
                if(jugador2.posicion != 0){
                    jugador2.posicion--;
                }
            }
            if(btn2der == 1){
                contador2 = 0;
                if(jugador2.posicion != 6){
                    jugador2.posicion++;
                }
            }
        }
        else if((contador2 > modo.velocidad) && (modo.PVP == -1)){
            contador2 = 0;
            if(jugador2.posicion > pelota.posicionX)
                jugador2.posicion--;
            else if((jugador2.posicion + 1) < pelota.posicionX)
                    jugador2.posicion++;
            
        }
        
        
        if(contador == pelota.velocidad){
            contador = 0;
            //inicio esquina seperior
            if((pelota.posicionX == 0  && pelota.posicionY == 1 && pelota.direccionX == -1 && pelota.direccionY == -1) 
                    || (pelota.posicionX == 7  && pelota.posicionY == 1 && pelota.direccionX == 1 && pelota.direccionY == -1)){
                    
                
                if((pelota.posicionX == jugador2.posicion) || (pelota.posicionX == (jugador2.posicion + 1))){
                    pelota.direccionX *= -1;
                    pelota.direccionY *= -1;
                    if(pelota.velocidad > pelota.velocidadMin)
                        pelota.velocidad--;
                }
                else{
                    finPartida = 0;
                }
                
            }//fin esquinas superior
            //inicio esquina inferior
            else if((pelota.posicionX == 0  && pelota.posicionY == 6 && pelota.direccionX == -1 && pelota.direccionY == 1) 
                    || (pelota.posicionX == 7  && pelota.posicionY == 6 && pelota.direccionX == 1 && pelota.direccionY == 1)){
                if((pelota.posicionX == jugador1.posicion) || (pelota.posicionX == (jugador1.posicion + 1))){
                    pelota.direccionX *= -1;
                    pelota.direccionY *= -1;
                    if(pelota.velocidad > pelota.velocidadMin)
                        pelota.velocidad--;
                }
                else{
                    finPartida = 0;
                }
                
            }//fin esquina inferior
            //inicioPaddle2NoEsquina 
            else if(pelota.posicionY == 1 && pelota.direccionY == -1){
                pelota.direccionY *= -1;
                if(pelota.posicionX == jugador2.posicion){
                    if(pelota.velocidad > pelota.velocidadMin)
                        pelota.velocidad--;
                    if(pelota.direccionX != -1){
                        pelota.direccionX --;
                    }
                }
                else if (pelota.posicionX == (jugador2.posicion + 1)){
                    if(pelota.velocidad > pelota.velocidadMin)
                        pelota.velocidad--;
                    if(pelota.direccionX != 1){
                        pelota.direccionX ++;
                    }
                    
                }
                else{
                    finPartida = 0;
                    ganadorAnterior = 1;
                }
            }//finPaddle2No esquina
            
            //iniciopaddle1
            else if(pelota.posicionY == 6 && pelota.direccionY == 1){
                pelota.direccionY*=-1;
                if(pelota.posicionX == jugador1.posicion){
                    if(pelota.velocidad > pelota.velocidadMin)
                        pelota.velocidad--;
                    if(pelota.direccionX != -1){
                        pelota.direccionX --;
                    }
                }
                else if (pelota.posicionX == (jugador1.posicion + 1)){
                    if(pelota.velocidad > pelota.velocidadMin)
                        pelota.velocidad--;
                    if(pelota.direccionX != 1){
                        pelota.direccionX ++;
                    }
                    
                }
                else{
                    finPartida = 0;
                    ganadorAnterior = 2;
                }
                
            }//finpaddle1
            
             //inicio pared
            else if((pelota.posicionX == 0 && pelota.direccionX == -1) || (pelota.posicionX == 7 && pelota.direccionX == 1)){
                pelota.direccionX *= -1;    
            }//fin pared
            
            
            pelota.posicionX += pelota.direccionX;
            pelota.posicionY += pelota.direccionY;
            
            
        
        }       
        
        
    }
};

void puntos(){
    
    for(i =0;i<100;i++){
        columnas = barridoColumnas[0];
        filas = puntos1[jugador1.puntos];
        __delay_ms(1);
        columnas = barridoColumnas[7];
        filas = puntos2[jugador2.puntos];
        __delay_ms(1);
        
        
    }
};
void ganador1(){
    for(i = 0; i<3;i++){
        for(j = 0;j<50;j++){
            for(k = 2;k<6 ;k++){
                columnas = barridoColumnas[k];
                filas = uno[k - 2];
                __delay_ms(1);
            }
            
        }
        columnas = 0x00;
            filas = 0xFF;
            __delay_ms(100);
    }
};
void ganador2(){
    for(i = 0; i<3;i++){
        for(j = 0;j<50;j++){
            for(k = 2;k<6 ;k++){
                columnas = barridoColumnas[k];
                filas = dos[k -2];
                __delay_ms(1);
            }
            
            
        }
        columnas = 0x00;
            filas = 0xFF;
            __delay_ms(100);
    }
};
void imprimirPVP(){
    for(i = 0;i<10;i++){
        for(k = 0;k<8;k++){
            columnas = barridoColumnas[k];
            filas = barridoPVP[k];
            __delay_us(250);  
        }

    }
};
void imprimirPVC(){
    for(i = 0;i<10;i++){
        for(k = 0;k<8;k++){
            columnas = barridoColumnas[k];
            filas = barridoPVC[k];
            __delay_us(250);  
        }

    }
};
void imprimirEasy(){
    for(i = 0;i<10;i++){
        for(k = 0;k<8;k++){
            columnas = barridoColumnas[k];
            filas = barridoEasy[k];
            __delay_us(250);  
        }

    }
};
void imprimirMedium(){
    for(i = 0;i<10;i++){
        for(k = 0;k<8;k++){
            columnas = barridoColumnas[k];
            filas = barridoMedium[k];
            __delay_us(250);  
        }

    }
};
void imprimirHard(){
   for(i = 0;i<10;i++){
        for(k = 0;k<8;k++){
            columnas = barridoColumnas[k];
            filas = barridoHard[k];
            __delay_us(250);  
        }

    }
};
void imprimirHell(){
   for(i = 0;i<10;i++){
        for(k = 0;k<8;k++){
            columnas = barridoColumnas[k];
            filas = barridoHell[k];
            __delay_us(250);  
        }

    }
};
void seleccion(){
    int selec = 1;
    int opcion =1;
    while(selec){
        if(opcion == 1)
            imprimirPVP();
        else
            imprimirPVC();
        
        if(btn1izq == 1){
           opcion*=-1; 
        }
        else if(btn1der == 1)
            selec = 0;
    }
    modo.PVP = opcion;
    selec = 1;
    opcion = 1;
    __delay_ms(100); 
    while(selec && (modo.PVP == -1)){
        if(opcion == 1)
            imprimirEasy();
        else if(opcion == 2)
            imprimirMedium();
        else if(opcion == 3)
            imprimirHard();
        else if(opcion == 4)
            imprimirHell();
        if(btn1izq == 1){
            if(opcion == 4)
                opcion =1;
            else
                opcion++;
        }
        else if(btn1der == 1)
            selec = 0;
    }
    modo.dificultad = opcion;
};
void main(void) {

TRISD = 0b00000000; //COLUMNAS
LATD = 0b00000000;
TRISB = 0b00000000; //FILAS
LATB = 0b00000000;
TRISCbits.TRISC0 = 1;
TRISCbits.TRISC1 = 1;
TRISCbits.TRISC6 = 1;
TRISCbits.TRISC7 = 1;

pelota.velocidad = 25;
pelota.velocidadMin = 5;
seleccion();
while (1)
{
    
    if(modo.dificultad == 1)
        modo.velocidad = 16;
    else if(modo.dificultad == 2)
        modo.velocidad = 12;
    else if(modo.dificultad == 3)
        modo.velocidad = 8;
    else if(modo.dificultad == 4)
        modo.velocidad = 6;
    inicio();
    pelota.velocidad = 20;
    juego();
    if(ganadorAnterior == 1)
        jugador1.puntos++;
    if(ganadorAnterior == 2)
        jugador2.puntos++;
    if(jugador1.puntos == 8){
        ganador1();
        jugador1.puntos = 0;
        jugador2.puntos = 0;
        seleccion();
    }
        
    else if(jugador2.puntos == 8){
        ganador2();
        jugador1.puntos = 0;
        jugador2.puntos = 0;
        seleccion();
    }
    else{
        puntos();
    }
        
    
}
    return;
}