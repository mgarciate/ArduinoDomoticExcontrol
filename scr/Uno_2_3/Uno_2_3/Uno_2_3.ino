// BOF preprocessor bug prevent - insert me on top of your arduino-code
// From: http://www.a-control.de/arduino-fehler/?lang=en
//#if 1
//__asm volatile ("nop");
//#endif


// Enumera tipos de circuitos.
//#define Reserva       	1
//#define Ado_Digital   	2
//#define Ado_3Etapas   	3
//#define Ado_Retroaviso 	4
//#define Enchufe		8
//#define EnchufeRF	9
//#define	Riego		14
//#define Riego_Temporizado 15
//#define Valvula		16
//#define ConsignaTemp	30
//#define Frio		20
//#define Calor		25
//#define Radiante	26
//#define	Persiana	35
//#define Toldo		36
//#define	Puerta		40
//#define	Ventilador	44
//#define	Piloto  	52
#include   "Excontrol_def.h"

/**************************************************************************
  #Librerias estandar shield ETHERNET. UNO Y MEGA.
***************************************************************************/

  #include <SPI.h>
  #include <Ethernet.h>
  #include <EthernetUdp.h>

/**************************************************************************
  #      SOLO MEGA
  #Librerias estandar shield WIFI.....
**************************************************************************/
//  #include <SPI.h>
//  #include <WiFi.h>
//  #include <WiFiUdp.h>
//  #define WIFI_SHIELD;
  
/**************************************************************************
  #     OBLIGATORIO UNO, NANO ATMEGA 328 
  #     EN CASO MEGA USA MEMORIA EXTERNA.  
  #Librerias estandar modulo Reloj.......  
  
  #define moduleDS3231 o #define moduleDS1307
***************************************************************************/
  #include <Wire.h>
  #define moduleDS1307  

/**************************************************************************
  #      SOLO MEGA
  #Librerias estandar Eeprom interna.
***************************************************************************/

//  #include <Wire.h>
//  #include <EEPROM.h>
//  #define EEPROM_INTERNA

/**************************************************************************
  #      SOLO MEGA
  #Activar historicos en SD y las librerias necesarias. 
***************************************************************************/
// #include <SD.h>
// #define Historical_SD 


/**************************************************************************
  #Herramientas de depuracion.
***************************************************************************/

//Enable watch dog
//habilita perro guardian
//#define ENABLE_WATCH_DOG;
//#define DEBUG_MODE




//CONFIGURACION EQUIPOS INTALADOS, TERMOSTATOS, ENCHUFES RADIOFRECUENCIA 433MHZ, INFARROJOS.
//EQUIPMENT CONFIGURATION , THERMOSTAT, RADIO 433MHZ, INFARROJOS.



//#define LED_IR 
//#define IR_RECIVE 
//#define THERMOSTAT_DS18B20_NUMBER
//#define RECEIVER_433
//#define TRANSMITER_433
//#define ELECTRIC_OUTLET_433
//#define RETROAVISOS //Habilita funcionamiento por retroavisos


/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/

//
#ifdef RECEIVER_433
  #include <RCSwitch.h>
#endif

#ifdef TRANSMITER_433 
 #ifndef RECEIVER_433
    #include <RCSwitch.h>
  #endif
#endif 

#ifdef ELECTRIC_OUTLET_433 
  #if !defined (RECEIVER_433)  || !defined (TRANSMITER_433)
    #include <RCSwitch.h>
  #endif 
#endif 

#ifdef LED_IR  
  #include <IRremote.h>
  IRsend irsend;
#endif 

#ifdef IR_RECIVE
 #ifndef LED_IR
    #include <IRremote.h>
  #endif
  IRrecv irrecv(19);//El 19 corresponde con el pin de arduino, cambiar para utilizar otro
  decode_results results;
#endif 

#ifdef THERMOSTAT_DS18B20_NUMBER 
  #include <OneWire.h>
  #include <DallasTemperature.h>
  #define TEMPERATURE_PRECISION 9
  #define ONE_WIRE_BUS 3           //Pin one wire donde estan conectadas las sondas
  DeviceAddress Ds18B20Addres[THERMOSTAT_DS18B20_NUMBER] ={{0x28,0x36,0x0F,0xB5,0x05,0x00,0x00,0x0B},{0x28,0xBF,0x99,0xB5,0x05,0x00,0x00,0x5C}};
  OneWire oneWire(ONE_WIRE_BUS);  // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
  DallasTemperature sensorTemp(&oneWire);// Pass our oneWire reference to Dallas Temperature.   
  boolean ThermostatCool[THERMOSTAT_DS18B20_NUMBER];
  boolean ThermostatHeat[THERMOSTAT_DS18B20_NUMBER];
  float Temperature[]={22.2,22.4,22.6};
#endif 

#ifdef RECEIVER_433
  RCSwitch mySwitch = RCSwitch();
#endif
#if defined (TRANSMITER_433)  && !defined (RECEIVER_433)
  RCSwitch mySwitch = RCSwitch();  
#endif 

#ifdef ELECTRIC_OUTLET_433
  #if !defined (TRANSMITER_433)  && !defined (RECEIVER_433)
    RCSwitch mySwitch = RCSwitch();
  #endif
#endif 



//Activa o desactiva cambio hora automatico invierno verano
//Set or Restet Daylight saving time o DST
//El modo dts esta configurado para europa
//Dts mode is set to europe
//Para otros paises configure funcion CargaHora()
//For other countries configure  function  CargaHora()
const boolean Enable_DaylightSavingTime  = true; 

/**************************************************************************
    CONFIGURACION DE PINES Y CIRCUITOS.
    
***************************************************************************/
// POLARIZAION PINES ENTRADA
  #define INTERNAL_RESISTOR; 

// MODO ACTIVACION RELES.
  #define On HIGH
  #define Off LOW

//Entradas con conmutador
//Swicth Inputs
byte PinSwicthInput[]={5};

//Define pines de Entradas
//Inputs pin
byte PinInput[]={2,3,4};

//Define pines de Salidas
//Outputs pin
byte PinOutput[]={6,7,8,9};

//Numero de Persianas
//Number of blind
const byte NumeroPersianas = 1;

// Circuitos 
const byte Circuit_Type[] ={Ado_3Etapas, Ado_Digital, Enchufe, Persiana};


/***********************************************************************************************************************/
/***********************************************************************************************************************/
//VARIABLES PARA CONTROL ESTADO CIRUCITOS
//EL RETORNO DE ESTADO SE RECOGE POR ENTRADAS DIGITALES.
/***********************************************************************************************************************/
/***********************************************************************************************************************/  

#ifdef RETROAVISOS 
  byte PinEstadoCircuito[]={27};
  #define RETROAVISO_NUMBER ( sizeof(PinEstadoCircuito))
  const byte Retroavisos_Number=RETROAVISO_NUMBER;
  unsigned long LastTimeEstadoRetroaviso[Retroavisos_Number];  
#endif


#include "v2.h"


//CONFIGURACION DE RED
//Direccion IP ES MUY PROBABLE QUE TENGAS QUE AJUSTARLO A TU RED LOCAL
//Cambia la ip por una dentro de tu rango, tambien debes actualizarla en tu aplicacion android
//Tambien puedes cambiar el puerto, por defecto 5000

// NETWORK CONFIGURATION
// IP Address, ADJUST TO YOUR LOCAL NETWORK
//If you change the IP address will have to adjust in android application
//If you change the Local Port address will have to adjust in android application
//WIRELESS CONFIGURATION
#ifdef WIFI_SHIELD enum Net_Security {OPEN,WEP,WPA};
  Net_Security Net_Type = WEP;
  char ssid[] = "YOUR-SSID"; //  your network SSID (name) 
  char pass[] = "YOUR-PASSWORD";    // your network password (use for WPA, or use as key for WEP)
  int keyIndex = 0;            // your network key Index number (needed only for WEP)
  
  unsigned int TimConexion;
  int status = WL_IDLE_STATUS;     // the Wifi radio's status
//  char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; 
  WiFiClient client;
  WiFiUDP Udp;
#else
  // buffers para recepcion y envio de datos
//  char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; 
  EthernetClient client;
  // Instanacia Ethernet UDP Para enviar y recibir paqueteP
  EthernetUDP Udp;
   
#endif


byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0x26, 0x20};
IPAddress ip(192,168,2,49);
unsigned int localPort = 5000;
const String Mail ="";
const char* Key ="";
const boolean SecureConnection=false;

//#include "root/Common_functions.h"

#include "Common_functions.h"

/******************************************************************************************************************************/
void UserSetup() {
    
  

}

void UserLoop(){


}

void LoopNewSecond(){
  //Este evento se produce cada segundo.
  //This event occurs every second.

}

void Loop30Sg(){
  //Este evento se produce cada 30sg.
  //This event occurs every 30SG.
}

void NewMinute(){
  //Este evento se produce cada cada minuto.
  //This event occurs every minute.

}
/******************************************************************************************************************************/
 /***********************************************************************************************************************/
/***********************************************************************************************************************/
//EVENTOS CONTROL ENTRADAS SALIDAS
//INPUT OUTPUT CONTROL EVENTS

//CUATRO EVENTOS PARA ENTRADAS DIGITALES
//CONMUTADOR CAMBIA VALOR
//PULSACION CORTA
//PULSACION LARGA, MAYOR DE 0.5 SEGUNDOS
//FINAL PULSACION LARGA

// FOUR EVENTS FOR DIGITAL INPUTS
//VALUE CHANGE SWITCH
// PRESS SHORT
// PRESS LONG, OVER 0.5 SECONDS
// LONG PRESS END.

/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
void SwicthStateChange(byte NumberInput){
//AUTO GENERATED CODE
/*************************************************************/
	//CIRCUITO NUMERO 1
	//Conmutador
  if (NumberInput==0){
    if (circuits[2].Value==1){
      circuits[2].Value=0;
    }
    else{
      circuits[2].Value=1;
    }
  }

/*************************************************************/
//END GENERATED CODE
/*************************************************************/

    /*****************************************************************/
  //Este evento se produce cuando un conmutador cambia posicion
  // This event occurs with swicth change state.
  //dos parametros
  //two parameters
  //Number input--numero de entrada
  //Value, if input is HIGH value = HIGH, If value=low then Value=Low
  //Value, si la entrada esta en nivel alto el valor es HIGH si es bajo el valor es LOW
  //Tambien puede acceder desde cualquier punto del codigo al valor de la entrada con SwicthState[]
  //You can know input state anywhere using SwictState[]
  
  /*****************************************************************/


}
void ShortInput(byte NumberInput){
/*************************************************************/
//Este evento se produce con una pulsación corta..
//This event occurs with a short press.
//AUTO GENERATED CODE
/*************************************************************/

	//CIRCUITO NUMERO 0
	//Circuito 
	if (NumberInput==0){
            
		switch (circuits[0].Value){
		case 0:
		     circuits[0].Value=1;break;
		case 1: 
		     circuits[0].Value=2; break;
		case 2: 
		     circuits[0].Value=3; break;
		case 3: 
		     circuits[0].Value=0; break;}
        }

	//CIRCUITO NUMERO 1
	//Circuito 
    if (NumberInput==1){
       if (circuits[1].Value==1){
         circuits[1].Value=0;
       }else{
         circuits[1].Value=1;
       }
     }
/*************************************************************/
//END GENERATED CODE
/*************************************************************/
  #ifdef DEBUG_MODE   
   Serial.print("Short Input End ");
   Serial.print(NumberInput);
   Serial.print(" pin ");
   Serial.println(PinInput[NumberInput]);    
  #endif
}
void LongInputEnd(byte NumberInput){
/*************************************************************/
//Este evento se produce con una pulsación corta..
//This event occurs with a short press.
//AUTO GENERATED CODE
/*************************************************************/

/*************************************************************/
//END GENERATED CODE
/*************************************************************/
  #ifdef DEBUG_MODE   
    Serial.print("Long Input End ");
    Serial.print(NumberInput);
    Serial.print(" pin ");
    Serial.println(PinInput[NumberInput]); 
  #endif
  
  /*****************************************************************/
  //FINAL DE PULSACION LARGA
  //LONG PRESS END, EVENT
  // This event occurs with end a long press.
  /*****************************************************************/
  
}
void LongInput(byte NumberInput){
/*************************************************************/
//Este evento se produce con una pulsación corta..
//This event occurs with a short press.
//AUTO GENERATED CODE
/*************************************************************/

	
/*************************************************************/
//END GENERATED CODE
/*************************************************************/
  #ifdef DEBUG_MODE   
    Serial.print("Long Input Start ");
    Serial.print(NumberInput);
    Serial.print(" pin ");
    Serial.println(PinInput[NumberInput]);     
  #endif
 
  /*****************************************************************/
  //EVENTO PRODUCINO AL INICIO DE UNA PULSACION LARGA
  // LONG PRESS START
  // This event occurs with start a long press.
  /*****************************************************************/    
}

void OutControl(){
/*************************************************************/
//Activamos los reles de control. Activate control relays.
//AUTO GENERATED CODE
/*************************************************************/

	//CIRCUITO NUMERO 0
	//Circuito 
	//Out 1
	digitalWrite(PinOutput[0],circuits[0].Out1_Value);
	//Out 2
	digitalWrite(PinOutput[1],circuits[0].Out2_Value);

	//CIRCUITO NUMERO 1
	//Conmutador
	//Out 1
	digitalWrite(PinOutput[2],circuits[1].Out1_Value);

/*************************************************************/
//END GENERATED CODE
/*************************************************************/
}

char* RunCommand(byte CommandNumber){
  //Este evento se produce cuando se ejecuta un comando desde el app
  //This event occurs when a command is executed from the app
   #ifdef DEBUG_MODE   
    Serial.print("Command Nª");Serial.println(CommandNumber);     
  #endif
  
  //Enabled this line for send infrared 
   #ifdef LED_IR
     SendIr(CommandNumber);
   #endif
  
 return "COMPLETED";
}
void CommonOrders(byte CommandNumber){
  //Este evento se produce cuando se ejecuta un comando desde el app
  //This event occurs when a command is executed from the app
  
  #ifdef DEBUG_MODE   
    Serial.println(CommandNumber);Serial.println(CommandNumber);             
  #endif
}
char* ReadSensor(byte NumeroSensor)
{
  //Monitor sensores
  //monitor sensors
  //El parametro numero de sensor representa se corresponde con el sensor configurado en la app
  //The number represents sensor parameter corresponds to the sensor set in the app
  if (NumeroSensor==100){
    char val[10];    
    //dtostrf(Temperature[0],4,2,val);
    return val;
  }

  return "RESERVA"; //No borrar, dont delete this line
}
String GetAlarmsName(byte Number){
/*************************************************************/
//AUTO GENERATED CODE
/*************************************************************/

	if (Number==0){return "Reserva";}
	if (Number==1){return "Reserva";}
	if (Number==2){return "Reserva";}
	if (Number==3){return "Reserva";}
	if (Number==4){return "Reserva";}
	if (Number==5){return "Reserva";}
	if (Number==6){return "Reserva";}
	if (Number==7){return "Reserva";}
	if (Number==8){return "Reserva";}
	if (Number==9){return "Reserva";}
/*************************************************************/
//END AUTO GENERATED CODE
/*************************************************************/

    return "RESERVA";
}
