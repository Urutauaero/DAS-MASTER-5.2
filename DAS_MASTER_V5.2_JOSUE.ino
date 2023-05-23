// Edição feita por mim (Josué Souza)
// Atualização do método de telemetria

/* TO DO:
 *    - Já está integrado
 *    - Falta verficar como os dados serão trasnmitidos
 *    - Após isso fechou
 *    - Analisar a lógica de como os dados são inserdos
 */
#include <Zanshin_BME680.h>
/*
    SISTEMA DE AQUISIÇÃO DE DADOS - DAS 2019 - MASTER
    URUTAU AERODESIGN - MANAUS, 02/06/2019
    
    WALDISMAR DE OLIVEIRA SAMPAIO FILHO
    JACOBUS LAURENS DE JAGER
*/

#define dasVersion "v3.0.1"          //O define é utilizado para pré definir os parâmetros que serão usados durante o programa 
#define versionName "Versão alpha de 2022"   

#define DEBUG // Antes de iniciar o programa ou teste já eé definido um "Debug" que visa encontrar erros pré-existentes no código

#pragma GCC optimize ("-Ofast") // Pré-processamento da manipulação do otimizaddor 
#include <Wire.h> // Iniciando a biblioteca do I2C

#define baudrate 115200 // Definindo a taxa de tranferência da serial 

void MotionSystem_setup(); 
void heading_setup();     
void hp_setup();          
void telemetria_setup();  
void vcas_setup();        

float xv, yv, zv;
float heading;
float headingDegrees;

void get_data();
void serial_write();
void SD_write();

// ###########################################################################
// VARIÁVEIS GLOBAIS #########################################################
// ###########################################################################

String GyroX, GyroY, GyroZ;
String AccelX, AccelY, AccelZ;

String data_string, IDV_string;

int16_t gpsDay = 0, gpsMonth = 0, gpsYear = 0;
int16_t gpsHour = 0, gpsMinute = 0, gpsSecond = 0;

byte data_string_serial[45];

int rps;
int altint;
int wow, rpm, control_1, control_2, control_3, control_4;
int sonarDistance;
int teste1, teste2;

float hp, pbmp, vcas, PVcas;
float longfloat, latfloat, altfloat;
float maghead;
float gForceX, gForceY, gForceZ;
float gyroXrate, gyroYrate, gyroZrate;

long magheadint;
long gyroXrateint;
long gyroYrateint;
double gForceXint;
double gForceYint;
double gForceZint;

uint32_t latuint, longuint;

uint16_t servo1uint, servo2uint, servo3uint, servo4uint;

uint32_t tempo;
double temp;

double tempoM, segundo;

int i = 0, j = 0;
uint8_t cs; // Variável de armazenamento tipo byte

//RTC ###################################################################################

#include <DS3231.h> // Inicializando a bilbioteca do módulo RTC DS3231
RTClib rtc; // Comunição através da biblioteca wire.h e definir um 'nome' a esta biblioteca  (SDA, SCL)
DateTime now = rtc.now();

void temporizador_rtc(){// Início da função RTC
  tempoM = now.minute() * 60;
  segundo = now.second();   
}

//###########################################################################################

void setup(){
  Serial.begin(baudrate); //  Início das comunicações feitas no arduino
  Wire.begin();           

  Serial.println("\n\nBEGIN: rtc.begin --------------\n");
  //rtc.Begin(); // Iniciando a comunicação com o RTC

  Serial.println("\n\nCALLING: MotionSystem_setup --------------\n");
  MotionSystem_setup(); // Iniciando o sistema de movimento

  Serial.println("\n\nCALLING: heading_setup --------------\n");
  heading_setup(); 

  Serial.println("\n\nCALLING: hp_setup --------------\n");
  hp_setup(); // Captação dos dados de pressão para definir a altura

  Serial.println("\n\nCALLING: vcas_setup --------------\n");
  vcas_setup(); // 
  
  Serial.println("\n\nCALLING: telemetria_setup --------------\n");
  telemetria_setup();
 // temporizador_rtc();     
}

void loop(){ // Função que estará escrevendo no cartão sd
  tempo = tempoM + segundo;
  get_data();
  serial_write();
  SD_write();
}
