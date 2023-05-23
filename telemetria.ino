//Sistema de Telemetria
//SD
#include <SD.h>
#include <SPI.h>
#include "SdFat.h"

const int chipSelect = 22; //modificar no das

File cartao_SD;

bool SD_flag = 0;

bool heading_flag = 0, IMU_flag = 0, HP_flag = 0, VCAS_flag = 0, medusa_flag = 0, RTC_flag = 0;

//SD

void telemetria_setup(){
  ("\n\n>>> telemetria_setup --------------\n");
  String cabecalho1 = F("Tempo \tRPM \tWOW  \tHP \tVCAS \tLAT    \tLON  \tROLL  \tPIT \tHEAD \tnZ  \tTemp \tPABS");
  String cabecalho2 = F("s     \trpm \tbit  \tft \tm/s  \tdeg    \tdeg  \tdeg   \tdeg \tdeg  \tg   \tºC   \thPa");

  Serial.println(F("#########################################################################################################################################################################################################################"));

  void print_logo();

  Serial.println(F("#########################################################################################################################################################################################################################"));

  Serial.println(F("DAS 19 - 22 Urutau Aerodesign"));
  Serial.print(dasVersion); Serial.print(" - "); Serial.println(versionName);

  void printTime();

  Serial.println(F("#########################################################################################################################################################################################################################"));
  Serial.println(F("Inicializando sistema..."));

  //SD

  if (!SD.begin(chipSelect)){
    Serial.println(F("Status SD:  ERRO, NÃO ENCONTRADO. POR FAVOR, VERIFIQUE A PORTA SD"));
    void SD_ERROR();
  }  
  else{
    SD_flag = 1;
  }
    if (SD.exists("urutau.txt")){
      Serial.println(F("Status SD:  OK! O ARQUIVO 'urutau.txt' EXISTE NO CARTÃO SD"));
    }
    else{
      Serial.println(F("Status SD:  OK! O ARQUIVO 'urutau.txt' NÃO EXISTE NO CARTÃO SD. CRIANDO ARQUIVO..."));
      cartao_SD = SD.open("urutau.txt", FILE_WRITE);
      cartao_SD.flush();
      cartao_SD.println(cabecalho1);
      cartao_SD.println(cabecalho2);
      cartao_SD.close();
    }
  
  void status_check();

  Serial.println(F("#########################################################################################################################################################################################################################"));

  Serial.println(cabecalho1);
  Serial.println(cabecalho2);
}

void get_data(){
  if(medusa_flag == 1)   { medusa_loop();       }  
  if(IMU_flag == 1)      { MotionSystem_loop(); }
  if(heading_flag == 1)  { heading_loop();      }
  if(VCAS_flag == 1)     { vcas_loop();         }
  if(HP_flag == 1)       { hp_loop();           }

//declaração das variáveis que serão impressas no cartão sd
  data_string = /* TEMPO:   */      String(tempo)  + String(",") + String(cs) + String(F(" \t "))
  
                /* RPM:     */    + String(rpm)    + String(F("\t "))
                                  
                /* WOW:     */    + String(wow)    + String(F(" \t "))

                /* HP:      */    + String(hp, 2)  + String(F(" \t "))
                                  
                /* VCAS:    */    + String(vcas)   + String(F(" \t "))
                                  
                /* GPS:     */    + String(latfloat, 6)   + String(F(" \t "))
                                  + String(longfloat, 6)  + String(F(" \t "))
                //                + String(altfloat, 2)   + String(F(" \t "))
                                  
                /* CONTROL:       + String(control_1) + String(F(" \t "))
                                  + String(control_2) + String(F(" \t "))
                                  + String(control_3) + String(F(" \t "))
                                  + String(control_4) + String(F(" \t "))*/
                  
                /* MOTION S:*/    + String(GyroX) + String(F(" \t "))
                                  + String(GyroY) + String(F(" \t "))
                                  
                /* HEADING: */    + String(GyroZ) + String(F(" \t "))
                                  
                /* FORÇA G: */    + String(gForceZint, 2) + String(F(" \t "))
                                  
                /* ºC:      */    + String(temp) + String(F(" \t "))
                                  
                                                                   
                /* PRESS. AB*/    + String(pbmp, 2);  
}

void serial_write(){
  #ifdef DEBUG
    Serial.println(data_string); //variaveis plotadas na serial que demostra no sd
  #else

    unsigned long tempo_serial = tempo * 10; //(tempo) 
    uint16_t hp_serial = hp * 10;
    uint16_t vcas_serial = vcas * 100;
    uint16_t sonar_serial = sonarDistance * 100;

    data_string_serial[0] = 255;
    data_string_serial[1] = 255;
    data_string_serial[2] = 255;
    data_string_serial[3] = 255;

    // TEMPO ------------------------------------------------
    data_string_serial[4] = tempo_serial >> 16;
    data_string_serial[5] = tempo_serial >> 8;
    data_string_serial[6] = tempo_serial;

    // RPS/RPM ----------------------------------------------
    data_string_serial[7] = rps >> 8;
    data_string_serial[8] = rps;

    // WOW --------------------------------------------------
    data_string_serial[9] = wow;

    // ALTITUDE-PRESSÃO -------------------------------------
    data_string_serial[10] = hp_serial >> 8;
    data_string_serial[11] = hp_serial;

    // VCAS -------------------------------------------------
    data_string_serial[12] = vcas_serial >> 8;
    data_string_serial[13] = vcas_serial;

    // GPS --------------------------------------------------
    data_string_serial[14] = latuint >> 24;
    data_string_serial[15] = latuint >> 16;
    data_string_serial[16] = latuint >> 8;
    data_string_serial[17] = latuint;
    
    data_string_serial[18] = longuint >> 24;
    data_string_serial[19] = longuint >> 16;
    data_string_serial[20] = longuint >> 8;
    data_string_serial[21] = longuint;
    
    data_string_serial[22] = altint >> 8;
    data_string_serial[23] = altint;

    //DEFLEXÃO -----------------------------------------------
    data_string_serial[24] = servo1uint >> 8;
    data_string_serial[25] = servo1uint;
    
    data_string_serial[26] = servo2uint >> 8;
    data_string_serial[27] = servo2uint;
    
    data_string_serial[28] = servo3uint >> 8;
    data_string_serial[29] = servo3uint;
    
    data_string_serial[30] = servo4uint >> 8;
    data_string_serial[31] = servo4uint;

    // MOTION SYSTEM --------------------------------------------------
    data_string_serial[32] = gyroXrateint >> 8;
    data_string_serial[33] = gyroXrateint;
    
    data_string_serial[34] = gyroYrateint >> 8;
    data_string_serial[35] = gyroYrateint;

    // HEADING --------------------------------------------------------
    data_string_serial[36] = magheadint >> 8;
    data_string_serial[37] = magheadint;

    // FATOR DE CARGA -------------------------------------------------
    data_string_serial[38] = gForceZint;
    data_string_serial[39] = gForceZint;

    // TEMPERATURA ----------------------------------------------------
    data_string_serial[40] = temp;

    // PRESSAO ABSOLUTA -----------------------------------------------
    data_string_serial[41] = pbmp;

    // DISTANCIA A PARTIR DO SONAR ------------------------------------
    data_string_serial[42] = sonarDistance >> 8;
    data_string_serial[43] = sonarDistance;

    for (uint8_t i = 0; i < 43; i++){
      Serial.write(data_string_serial[i]);      
    }

  #endif
}

void SD_write(){
  if (SD_flag == 1){
    cartao_SD = SD.open("urutau.txt", FILE_WRITE);
    cartao_SD.print(data_string);
    cartao_SD.print("\n");
    cartao_SD.close();
  }
}

void status_check(){

  // MEDUSA --------------------------------------------------------------------------
  Wire.beginTransmission (0x08);

  if (Wire.endTransmission () == 0){
    Serial.println (F("Status medusa: OK!"));
    medusa_flag = 1;
  }
  else{
    Serial.println (F("Status medusa: ERRO, NÃO ENCONTRADO! PLACA DANIFICADA! (?)"));
  }

  //RTC ------------------------------------------------------------------------------
  Wire.beginTransmission (0x68);

  if (Wire.endTransmission () == 0){
    Serial.println (F("Status RTC:  OK!"));
    RTC_flag = 1;
    
  }
  else{
    Serial.println (F("Status RTC:  ERRO, NÃO ENCONTRADO! PLACA DANIFICADA! (?) >>> FALHA CRÍTICA <<<"));
  }

  //HP -------------------------------------------------------------------------------
  Wire.beginTransmission (0x77);

  if (Wire.endTransmission () == 0){
    Serial.println (F("Status HP: OK!"));
    HP_flag = 1;
  }
  else{
    Serial.println (F("Status HP: ERRO, NÃO ENCONTRADO!"));
  }

  //VCAS -----------------------------------------------------------------------------
  Wire.beginTransmission (0x40);

  if (Wire.endTransmission () == 0){
    Serial.println (F("Status VCAS: OK!"));
    VCAS_flag = 1;
  }
  else{
    Serial.println (F("Status VCAS: ERRO, NÃO ENCONTRADO!"));
  }

  //IMU ------------------------------------------------------------------------------
  Wire.beginTransmission (0x69);

  if (Wire.endTransmission () == 0){
    Serial.println (F("Status IMU: OK!"));
    IMU_flag = 1;
  }
  else{
    Serial.println (F("Status IMU: ERRO, NÃO ENCONTRADO!"));
  }

  //Heading --------------------------------------------------------------------------
  Wire.beginTransmission (0x0D);

  if (Wire.endTransmission () == 0){
    Serial.println (F("Status Heading: OK!"));
    heading_flag = 1;
  }
  else{
    Serial.println (F("Status Heading: ERRO, NÃO ENCONTRADO!"));
  }  
}

void printTime(){} 

void print_logo(){ 
  Serial.println("  _   _          _                    _ _ _ ");
  Serial.println(" | | | |_ __ _  | |_ __ _   _   _    / \\ ___ _ __ ___ __| | ___ ___(_) __ _ _ __ ");
  Serial.println(" | | | | '__| | | | __/ _` | | | |  / _ \\ / _ \\ '__/ _ \\ / _` |/ _ \\/ __| |/ _` | '_ \\");
  Serial.println(" | | | | '__| | | | __/ _` | | | |  / _ \\ / _ \\ '__/ _ \\ / _` |/ _ \\/ __| |/ _` | '_ \\");
  Serial.println(" \\___/|_|  \\__,_|  \\__\ \__,_|\\__,_| /_/ \\_\\___|_| \\___/ \\__,_|\\___||___/_|\\__, |_| |_|");
  Serial.println(" |___/ ");
}

void SD_ERROR(){
  while(true){
    Serial.println("SD ERROR...");
    delay(100);
    Serial.println("SD ERROR...");
    delay(100);
    Serial.println("SD ERROR...");
    delay(100);
  }
}
