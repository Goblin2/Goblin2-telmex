#include <SoftwareSerial.h>                           
//#include <OneWire.h>
//#include <DallasTemperature.h>

SoftwareSerial RS485(8,7);                                                  //(RX,TX)   asigna el serial por software para RS-485
SoftwareSerial SIM5320(6, 4);                                                   // RX, TX
//#define ONE_WIRE_BUS 10                                                         //PIN DS18B20


char    BUFFER_USART;                                                           //BUFFER RX 
String  token ="gqeGtoC8soQ2fyLj2iJTQEIsloKjRdLllg5Wtzzemhjyv13dKJmUrFRp1FNM";                                //TOKEN UBIDOTS
String  id    ="5771b9b27625420dd490348e";                                      //ID CORROSION  
String  id1   ="5839055976254207457c062f";                                      // ID BATERIA

char DATO_CORR[15];                    // Obtiene toda la cadena de 15 caracteres del string 
char PULG[3];                          // guarda solo 2 caracteres de los 15 acarcteres (del string de corrocion) dado por el sensor 
int x=0;                               // cuenta las posiciones de la cadena de 15 caracteres para obtener una por una
float MPY=0;                           // variable regresa para la convcersion de char a entero
float MPY1=0;                          // variable regresa el valor de la ecuacion (de PULAGGADA a MICRAS) 

char DATO_BATT[30];                    // Obtiene toda la cadena de 17 caracteres del string de bateria 
char BATT[5];                          // guarda solo 2 caracteres de los 15 acarcteres (del string de corrocion) dado por el sensor 3
float VBATT=0;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 int     X=0;                                                                        
 int     I;
 int     D;
 int     P;                                          //REGISTROS DEL SERIAL
 int     Z;
 int     COPY;
 int     COPY_TIME;
 int     ZZ;
 char    BUFFER_USART2[150];
 String  BAT_V;


void setup() { 
  
//////////////////////////////////////////////////////////////////////////////  
  SREG    =0x80;                                    //INT GLOBAL SOLO ACTIVA LA DEL UART
  UCSR0B  =0x98;                                    //ACTIVA TX Y RX A SI COM LA INT DEL RX
  UCSR0C  =0x06;                                    //CONFIGURAMOS A 8BITS DE DATOS SERIAL
  UBRR0H  =0x00;                                    //VALOR QUE SE LE CARGA AL REGISTRO HIGH 0
  UBRR0L  =0x67;                                    //VALOR QUE SE LE CARGA AL REGISTRO LOW 0x67 QUE ES IGUAL A 103 PARA 9600
  
//////////////////////////////////////////////////////////////////////////////  
   pinMode(5, OUTPUT);                              //PIN PARA EL ENCENDIDO DEL SIM5320
   pinMode(0, INPUT);                               //
   SIM5320_ON();                                    //ENCIENDO EL SIM
   RS485.begin(2400);                               // velocidad de sensor corrosion
   SIM5320.begin(9600);
   delay(10000);
   SETUP_SIM5320();                                 //INICIALIZA EL SIM
}


void loop() {
  
  voltaje_bateria();
  sensor_corrosion();
  WEB();                                           //ENVIO JSON AL UBIDOTS
  //delay(1000);                                    //TIEMPO DE MUESTRAS
  
}
///////////////////////////////////////////////////////////////////////////
ISR (USART_RX_vect)
{  
    X++;
    BUFFER_USART = UDR0;                // LEE EL BUFFER RX                    
    
    if(COPY!=0&&X>I+2&&X<D+4){
    Z++;
    BUFFER_USART2[Z]= BUFFER_USART;  
                       
                                   }  
  
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void voltaje_bateria()
{
  COPIA_BUFFER(18,22);
  SIM5320.println("AT+CBC");          //VOLTAJE BATERIA
  delay(500);
  
  BAT_V=BUFFER_USART2;
  LIMPIA_BUFFER();
    
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sensor_corrosion()
{
   
   RS485.listen();
   RS485.write(01);                // envia el valor 1 en binario por el SERIAL (direccion del sensor- seleccionada en el bit swich de sensor)
   RS485.write("\n");              // envia ENTER para obtener el valor del RS-485  
   
    delay(1000);  
    while(RS485.available()>0){           // detecta la variable de regreso por el sensor  
     
     for(int x=0;x<15;x++){               // asigna la posicion de la cadena dada por el sensor (del 0 al 15 "15 caracteres")
     
      DATO_CORR[x]=RS485.read();          // guarda en variable DATO_CORR la cadena que regresa el sensor 
       PULG[0]=DATO_CORR[5];              // guarda en la posicion 0 de la cadena PULG - la posicion 5 de la cadena DATO_CORR
       PULG[1]=DATO_CORR[6];              // guarda en la posicion 1 de la cadena PULG - la posicion 6 de la cadena DATO_CORR
       
    }
    
    MPY=atol(PULG);                            // convierte el char (PULG)- en entero(MPY)
    MPY1=((MPY/1000)*254);                 // hace la conversion de PULGADAS a MICRAS    
    }
     
}   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WEB()                                  
{
  SIM5320.listen();
  int LONG_JSON = 15;
  int SEND = 238;
    
  //sensors.requestTemperatures(); 
  //float temp = sensors.getTempCByIndex(0); 

  SIM5320.println(F("AT+CIPOPEN=0,\"TCP\",\"things.ubidots.com\",80"));  //
  delay(4000);
  
  SIM5320.print("AT+CIPSEND=0,");  //
  delay(50);
  SIM5320.println(SEND);  //
  delay(600);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  SIM5320.print(F("POST /api/v1.6/variables/"));
  delay(10);
  SIM5320.print(id);
  delay(10);
  SIM5320.println(F("/values  HTTP/1.1"));
  delay(10);
  SIM5320.println(F("Content-Type: application/json"));
  delay(10);
  SIM5320.print(F("Content-Length:"));
  delay(10);
  SIM5320.println(LONG_JSON);
  delay(10);
  SIM5320.print(F("X-Auth-Token: "));
  delay(10);
  SIM5320.println(token);
  delay(10);
  SIM5320.println(F("Host: things.ubidots.com"));
  delay(10);
  SIM5320.println("");
  ///////////////////////////////////////////////////////////////
  SIM5320.print("{\"value\":");
  delay(10);
  SIM5320.print(MPY1,2);
  delay(50);
  SIM5320.println("}");
  delay(50);
  SIM5320.write(0x1A);
  delay(3000);
  
    SIM5320.print("AT+CIPSEND=0,");  //
  delay(50);
  SIM5320.println(SEND);  //
  delay(600);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  SIM5320.print(F("POST /api/v1.6/variables/"));
  delay(10);
  SIM5320.print(id1);
  delay(10);
  SIM5320.println(F("/values  HTTP/1.1"));
  delay(10);
  SIM5320.println(F("Content-Type: application/json"));
  delay(10);
  SIM5320.print(F("Content-Length:"));
  delay(10);
  SIM5320.println(LONG_JSON);
  delay(10);
  SIM5320.print(F("X-Auth-Token: "));
  delay(10);
  SIM5320.println(token);
  delay(10);
  SIM5320.println(F("Host: things.ubidots.com"));
  delay(10);
  SIM5320.println("");
  ///////////////////////////////////////////////////////////////
  SIM5320.print("{\"value\":");
  delay(10);
  SIM5320.print(BAT_V);
  delay(50);
  SIM5320.println("}");
  delay(50);
  SIM5320.write(0x1A);
  delay(3000);
  
  SIM5320.println(F("AT+CIPCLOSE=0"));
  delay(2000);

}

void SIM5320_ON()                                  //
{
  digitalWrite(5, HIGH);
  delay(2000);                                                //ENCIENDE EL SIM5320
  digitalWrite(5, LOW);
  delay(2000);
}
/////////////////////////////////////////////////////////////////////////////
void SETUP_SIM5320()
{

  SIM5320.listen();
  SIM5320.println(F("AT+CFUN=1"));        //SIM5320 TRABAJANDO CON TODAS LAS FUNCIONALIDADES
  delay(250);

  SIM5320.println(F("AT+CNMP=2"));        //MODO AUTOMATICO
  delay(250);

  SIM5320.println(F("AT+CVAUXV=61"));     //VOLTAJE DE SALIDA 3.0V
  delay(250);

  SIM5320.println(F("AT+CNBP=0x0000000004000000"));  //3G 850MHZ TELCEL 0x0000000004000000 hspda     2G 1900MHZ TELCEL AT+CNBP=0x0000000000200000 edge
  delay(500);

  //SIM5320.println(F("AT+IPREX=9600"));
  //delay(4000);

  SIM5320.println(F("AT+CSQ"));            //NIVEL DE LA SEÑAL
  delay(250);

  SIM5320.println(F("AT+CREG?"));         
  delay(250);

  SIM5320.println(F("AT+COPS?"));
  delay(250);
 
  SIM5320.println(F("AT+CGSOCKCONT=1,\"IP\",\"internet.itelcel.com\""));
  delay(250);

  SIM5320.println(F("AT+CNSMOD?"));        //MUESTRA EN QUE RED ESTA TRABAJANDO
  delay(250);

  SIM5320.println(F("AT+CSOCKSETPN=1"));
  delay(250);

  SIM5320.println(F("AT+CIPMODE=0"));
  delay(250);

  SIM5320.println(F("AT+CNBP?"));
  delay(250);

  SIM5320.println(F("AT+NETOPEN"));
  delay(8000);

  SIM5320.println(F("AT+IPADDR"));          //IP
  delay(500);

  SIM5320.println(F("AT+CGSN"));            //IMEI
  delay(500);
}

////////////////////////////////////////////////////////////
void COPIA_BUFFER(int P,int W)                       //VARIABLES DE RX
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

void LIMPIA_BUFFER()                          // FUNCION QUE LIMPIA EL BUFFER
{
  COPY=0;
    X=0;
      Z=0;
        I=0;
          D=0;
}
