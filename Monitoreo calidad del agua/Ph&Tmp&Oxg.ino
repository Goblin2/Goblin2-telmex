/////////////////////////////////////////////////////////////////
// TELMEXHUB, MICROSOFT.
// EJEMPLO PARA SUBIR DATOS DE VARIABLES 
// A UN SERVIDOR WEB CON UN GOBLIN 2.
// EN ESTE CASO SUBIREMOS DATOS DE TEMPERATURA 
// Y PH A LA PLATAFORMA UBIDOTS.
// -JORGE LUIS, HECTOR, RAFAEL CORTES.
// -TEAM. VERSE TECHNOLOGY.
/////////////////////////////////////////////////////////////////

#include <EEPROM.h>                       //LIBRERIA EEPROM
#include <OneWire.h>                      //LIBRERIA ONEWIRE
#include <DallasTemperature.h>            //LIBRERIA SENSOR DE TEMPERATURA
#include <SoftwareSerial.h>               //LIBRERIA SOFTWARE SERIAL
#include <avr/wdt.h>                      //LIBRERIA avr/wdt
#include <avr/sleep.h>                    //LIBRERIA avr/sleep
#include <avr/power.h>                    //LIBRERIA avr/power.h

SoftwareSerial SIM5320(6, 4); // RX, TX

OneWire TMP_DALLAS(2);                    //PIN SENSOR DE TEMPERATURA
DallasTemperature sensors(&TMP_DALLAS);   //OBTENER TEMPERTAURA

///////////////////////////DECLARO MIS REGISTROS CON SUS DATOS CONFIGURADOS////////////////////  
byte  INIT_EEPROM; 
long int    Puxitron=0; 
int    INT_TIMER16;
char   WEB_TIEMPO[6];
String TIEMPO_INT;
String IMEI;
String IP="192.168.1.1";
String PH;
String TMP="36.00";
String OXI="36.00";
String BAT_V;
int long acumulador=0;
int long SUMA=0;
float    PROM=0;                                            //ADC REGISTROS Y PROMEDIO
float    INTER=0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 int     X=0;                                                                        
 int     I;
 int     D;
 int     P;                                                  //REGISTROS DEL SERIAL
 int     Z;
 int     COPY;
 int     COPY_TIME;
 int     ZZ;
 char    BUFFER_USART;
 char    BUFFER_USART2[150];
 char    TIME_BUFFER[22];
 int     FLAG;
 int     FREQ_CHECK;
///////////////////////////////////////////////////////////////////////////
ISR (USART_RX_vect)
{
   //while(!(UCSR0A&(1<<UDRE0))){
    //                              };     //SI TERMINO DE RECIVIR LO ESCRIBE EN TX PARA ENVIAR
    
    X++;
    BUFFER_USART = UDR0;                   // LEE EL BUFFER RX                    
    
    if(COPY!=0&&X>I+2&&X<D+4){
    Z++;
    BUFFER_USART2[Z]= BUFFER_USART;  
                       
                                   } 
    
    if(FREQ_CHECK!=0&&BUFFER_USART == '{'){
      FLAG=1;
                                                   }
    
                    if(COPY_TIME!=0&&FLAG!=0){
                      ZZ++;
                          TIME_BUFFER[ZZ]= BUFFER_USART;  
                                          if(TIME_BUFFER[ZZ] == '}'){
                                            COPY_TIME=0;
                                              FLAG=0;
                                                ZZ=-1;
                                                 FREQ_CHECK=0;                       
                                                                       }
                                                                               } 

    //UDR0 = BUFFER_USART;                // ESCRIBE EN EL BUFFER TX  

} 


void setup() { 

  delay(1000);
  sensors.begin();                                  //INICIA EL SENSOR DALLAS TEMPERATURA
  sensors.setResolution(TEMP_10_BIT);               //10BITS DE RESOLUCION EN LA TEMPERATURA
//////////////////////////////////////////////////////////////////////////////  
  SREG    =0x80;                                    //INT GLOBAL SOLO ACTIVA LA DEL UART
  UCSR0B  =0x98;                                    //ACTIVA TX Y RX A SI COM LA INT DEL RX
  UCSR0C  =0x06;                                    //CONFIGURAMOS A 8BITS DE DATOS SERIAL
  UBRR0H  =0x00;                                    //VALOR QUE SE LE CARGA AL REGISTRO HIGH 0
  UBRR0L  =0x67;                                    //VALOR QUE SE LE CARGA AL REGISTRO LOW 0x67 QUE ES IGUAL A 103 PARA 9600
//////////////////////////////////////////////////////////////////////////////
 SIM5320.begin(9600);
  pinMode(14, INPUT);                               // SESONR PH A0
  pinMode(5, OUTPUT);                                 
  analogReference(DEFAULT);
////////////////////////////////////////
  digitalWrite(5, HIGH);                            //ENCIENDE EL SIM5320
  delay(2000);//                                    
  digitalWrite(5, LOW);
  delay(2000);
    
  delay(15000);
  SETUP_SIM5320();
}





void loop() {

PH_ADC();                                    //FUNCION PARA LEER LA VARIABLE DEL SENSOR DE PH
WEB();                                       //FUNCION PARA SUBIR EL DATO A LA PLATAFORMA

}

void WEB()                                   //FUNCION WEN. ESTA FUNCION SUBE LOS DATOS A LA PLATAFORMA.  
{
   
int x=0;
byte EEPROM_DEFAULT;
int JSON_3DATOS=0;
int JSON_BASE1=84;
int JSON_BASE2=90;
int JSON_BASE3=90;
int SEND=137;

  sensors.requestTemperatures();        //Prepara el sensor para la lectura

  COPIA_BUFFER(18,22);
  SIM5320.println("AT+CBC");            //VOLTAJE BATERIA
  delay(500);
  LIMPIA_BUFFER();
  BAT_V=BUFFER_USART2;
  
  SIM5320.println("AT+CSQ");             //CALIDAD DE CONSULTA DE SEÑAL
  delay(500);

  while (x<6) {
  COPIA_BUFFER(1,90);
  SIM5320.println("AT+CIPOPEN=0,\"TCP\",\"vtechph.herokuapp.com\",80");  //vtechph.herokuapp.com
  delay(3000);
  LIMPIA_BUFFER();                                         
  x++;                                               

                                                           if (x == 5) {              
                                                              SIM_ON(); 
                                                                 delay(2000); 
                                                            
                                                                                                            }//IF  
  
                                                                                                         



                                                            if (strcasestr(BUFFER_USART2, "+CIPOPEN: 0,0")) { 
                                                                  break;  
                                                                                                              }//IF
                                                                                                             
                                                                      
                                                                      
        
                                                                      
                                                                      
                                                                      
                                                                      } //WHILE
                                                            

  
  
   
   JSON_3DATOS=JSON_BASE3+IMEI.length()+IP.length()+OXI.length()+BAT_V.length()+JSON_BASE2+IMEI.length()+IP.length()+TMP.length()+BAT_V.length()+JSON_BASE1+IMEI.length()+IP.length()+PH.length()+BAT_V.length()+4;                         //CALCULA LA LONGITUD DEL JSON
   SEND+=JSON_3DATOS;                              //CALCULA LA LONGITUD DE LO QUE VA ENVIAR DESPUES DEL PROMPT(SEND)
   
   SIM5320.print("AT+CIPSEND=0,");  //
   delay(100);
   SIM5320.println(SEND);  //
   delay(1000);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
   SIM5320.println(F("POST /vtechinfos/ HTTP/1.1"));
   delay(50);
   SIM5320.println(F("Content-Type: application/json"));
   delay(50);
   SIM5320.println(F("Host: vtechph.herokuapp.com"));
   delay(50);
   SIM5320.println(F("Connection: keep-alive"));
   delay(50);
   SIM5320.print(F("Content-Length:"));   
   delay(50);
   SIM5320.println(JSON_3DATOS);   
   delay(50);
   SIM5320.println("");
///////////////////////////////////////////////////////////////   
   SIM5320.print(F("[{\"imei\":\""));
   delay(50);
   SIM5320.print(IMEI);
   delay(50);
   SIM5320.print(F("\",\"ip\":\""));
   delay(50);
   SIM5320.print(IP);                                                           //JSON PH
   delay(50);
   SIM5320.print(F("\",\"kpi\":\"ph\",\"kpivalue\":\""));
   delay(50);
   SIM5320.print(PH);
   delay(50);
   SIM5320.print(F("\",\"batterystate\":\""));
    delay(50);
   SIM5320.print(BAT_V);
   delay(50);
   SIM5320.print(F("\",\"kpiunitmeasure\":\"ph\"},"));
   delay(50);
//////////////////////////////////////////////////////////////
   SIM5320.print(F("{\"imei\":\""));
   delay(50);
   SIM5320.print(IMEI);
   delay(50);
   SIM5320.print(F("\",\"ip\":\""));
   delay(50);
   SIM5320.print(IP);                                                           //JSON TEMPERATURA
   delay(50);
   SIM5320.print(F("\",\"kpi\":\"tmp\",\"kpivalue\":\""));
   delay(50);
   SIM5320.print(sensors.getTempCByIndex(0),2);
   delay(50);
   SIM5320.print(F("\",\"batterystate\":\""));
    delay(50);
   SIM5320.print(BAT_V);
   delay(50);
   SIM5320.print(F("\",\"kpiunitmeasure\":\"Celsius\"},"));
   delay(50);
//////////////////////////////////////////////////////////////  
   SIM5320.print(F("{\"imei\":\""));
   delay(50);
   SIM5320.print(IMEI);
   delay(50);
   SIM5320.print(F("\",\"ip\":\""));
   delay(50);
   SIM5320.print(IP);                                                           //JSON OXI
   delay(50);
   SIM5320.print(F("\",\"kpi\":\"oxi\",\"kpivalue\":\""));
   delay(50);
   SIM5320.print(OXI);
   delay(50);
   SIM5320.print(F("\",\"batterystate\":\""));
   delay(50);
   SIM5320.print(BAT_V);
   delay(50);
   SIM5320.println(F("\",\"kpiunitmeasure\":\"Oxigeno\"}]"));
   delay(50);
   SIM5320.write(0x1A);
   TIME_JSON();
   delay(10000);   
//////////////////////////////////////////////////////////////  
   SIM5320.println(F("AT+CIPCLOSE=0"));                                          
   delay(5000); 


     }//IF PRINCIPAL
void SIM_ON()                                  //  
{
  digitalWrite(5, HIGH);
  delay(2000);//                                     //ENCIENDE EL SIM5320
  digitalWrite(5, LOW);
  delay(2000);
 } 
////////////////////
void PH_ADC() {
char PH_CHAR[10];                          //BUFFER DONDE SE GUARDA LA CONVERCION DEL FLOAT 
PROM=0;
SUMA=0;
INTER=0;
     
for (int x=0;x<4000;x++) {
           
      acumulador=analogRead(A0);  
      if(acumulador>480){
        //bitSet(acumulador,0);
        //bitSet(acumulador,1);
                               }
       SUMA+=acumulador;
                                                  } 

PROM=SUMA/4000; 
PROM=PROM*0.0049866;             //promedio es el voltaje

INTER=((PROM-1.980)*(4-10)/(3.00-1.980))+10;
 
   if(INTER>10.00){
              INTER=10.0000;
                  }

   if(INTER<4.00){
              INTER=4.0000;
                  }               
     
dtostrf(INTER,5,2,PH_CHAR);                  //CONVIERTE DE FLOTANTE A CHAR

PH=PH_CHAR;
//Serial.print(INTER,4);
//Serial.println(" PH");
//PROM=0;
//SUMA=0;
//INTER=0;
//delay(1000);
//Serial.println(" ");
}


////////////////////////////////////////////////////////////
void COPIA_BUFFER(int P,int W)                     //FUNCION QUE LIMPIA TODO EL STRING QUE GUARDA LAS VARIABLES DE RX
{
  //K=0;
  COPY=1;                                 
    X=0;
      Z=-1;
      for (int i=0; i<99;i++)
        {BUFFER_USART2[i]=NULL;}                  //NULL 00
          I=P; 
            D=W;
 //return K;
}

void LIMPIA_BUFFER()                              //LIMPIA BUFFER
{
  COPY=0;
    X=0;
      Z=0;
        I=0;
          D=0;
}

void TIME_JSON()                                 
{
  COPY_TIME=1;
      ZZ=-1;
       FREQ_CHECK=1;

}
/////////////////////////////////////////////////////////////////////////////
void SETUP_SIM5320()                       // FUNCION QUE LIMPIA TODO EL STRING QUE GUARDA LAS VARIABLES DE RX
{
   
  SIM5320.println(F("AT+CFUN=1"));    //
  delay(250);

  SIM5320.println(F("AT+CVAUXV=61"));    //GPS APGADO PARA NO GASTAR PILA
  delay(250);  
  
  SIM5320.println(F("AT+CGPS=0"));    //GPS APGADO PARA NO GASTAR PILA
  delay(500);
 
  SIM5320.println(F("AT+CNBP=0x0000000004000000"));  //3G EN 850MHZ EN TELCEL
  delay(500);
  
  SIM5320.println(F("AT+CNMP=2"));  //
  delay(250);
  
  //SIM5320.println(F("AT+IPREX=9600"));  //
  //delay(4000);

  //SIM5320.println(F("AT+CIPTIMEOUT=30000,30000,30000"));  //PREGUNTO LA CALIDAD DE LA SEÑAL 
  //delay(500);

  delay(10000);
  SIM5320.println(F("AT+CSQ"));  //PREGUNTO LA CALIDAD DE LA SEÑAL 
  delay(250);

  SIM5320.println(F("AT+CREG?"));  //REGISTRO DE RED, ME TIENE QUE DAR UN 1
  delay(250);
  
  SIM5320.println(F("AT+COPS?"));  //SELECCION DE OPERADOR
  delay(250);
  //////////////////////////////////
  //SIM5320.println(F("AT+CGSOCKCONT=1,\"IP\",\"internet.itelcel.com\""));  //DEFINO EL ENCHUFE QUE EN ESTE CASO ES TELCEL
  //delay(4000);

  SIM5320.println(F("AT+CGSOCKCONT=1,\"IP\",\"internet.itelcel.com\""));  //DEFINO EL ENCHUFE QUE EN ESTE CASO ES TELCEL
  delay(250);

  SIM5320.println(F("AT+CNSMOD?")); //MUESTRA EL TIPO DE RED EN EL QUE ENTRO EL SISTEMA
  delay(250);

  SIM5320.println(F("AT+CSOCKSETPN=1")); //ACTIVA EL PDP
  delay(250);

  SIM5320.println(F("AT+CIPMODE=0"));  //MODO TRANSPARENTE, EN ESTE CASO ES MODO NO TRANSPARENTE
  delay(250);

  SIM5320.println(F("AT+CNBP?"));  //ESTADO DE BANDA DE PREFERENCIA
  delay(250);

  SIM5320.println(F("AT+NETOPEN")); //ACTIVE PDP CONTEXT PROFILE NUMBER
  delay(8000);

  SIM5320.println(F("AT+IPADDR"));  //CONSULTA LA DIRECCION IP LOCAL
  delay(500);


 COPIA_BUFFER(8,22);
 SIM5320.println(F("AT+CGSN"));  //IMEI
 delay(500);
 LIMPIA_BUFFER();
 IMEI=BUFFER_USART2;

}

