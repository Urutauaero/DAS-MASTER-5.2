//SISTEMA DE MOVIMENTO
#include "I2Cdev.h" // Tipo de comunicação na qual a I2C original não suporta, assim foi desenvolvida para ma comunição mais robustas 
#include "MPU6050_6Axis_MotionApps20.h" // Biblioteca do módulo de acelômetro e giroscópio --> https://www.filipeflop.com/produto/acelerometro-e-giroscopio-3-eixos-6-dof-mpu-6050/

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  #include "Wire.h"
#endif

MPU6050 mpu; //removi a parte mpu(0x68)
#define OUTPUT_READABLE_YAWPITCHROLL

long previousMillis = 0;
long redLedInterval = 100;  

bool dmpReady = false;
uint8_t mpuIntStatus;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];

Quaternion q; // [w, x, y, z]
VectorInt16 aa; // [x, y, z]
VectorInt16 aaReal; // [x, y, z]
VectorInt16 aaWorld; // [x, y, z]
VectorFloat gravity; // [x, y, z]
int ax, ay, az;
float lsb = 16384.0;

float euler[3]; // [psi, theta, phi]
float ypr[3]; // [yaw, pitch, roll]
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

volatile bool mpuInterrupt = false;  
void dmpDataReady(){
  mpuInterrupt = true;
}

// ---------------------------------------------------------------------------------
// ------------------- INÍCIO DA FUNÇÃO DE CONFIGURAÇÃO (SETUP)---------------------
// ---------------------------------------------------------------------------------

void MotionSystem_setup(){
  Serial.println("\n\n>>> init: MotionSystem_setup --------------\n");
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
   // Wire.begin();  duas linhas modificadas
   // Wire.setClock(400000);
  // DEFINIÇÃO DA FREQUÊNCIA DE CLOCK ----------------------------------------------
  TWBR = 24;
    
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif
  
  // INICIALIZAÇÃO DO MPU6050 ------------------------------------------------------
  mpu.initialize();
  pinMode(2, INPUT);
  
  // VERIFICAÇÃO DA CONEXÃO COM O MPU6050 ------------------------------------------
  Serial.println(mpu.testConnection() ? F("Conexão bem sucedida") : F("Falha na conexão"));

/*    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again  */  //parte modificada baseado no i2cdev

      
  // INICIALIZAÇÃO E CARREGAMENTO DO DMP (DIGITAL MOTION PROCESSOR) ----------------
  devStatus = mpu.dmpInitialize();
  
  // INSERÇÃO DE VALORES DE OFFSET DO SENSOR ---------------------------------------
  mpu.setXGyroOffset(3);
  mpu.setYGyroOffset(12);
  mpu.setZGyroOffset(8);
  mpu.setZAccelOffset(937);

  /* Os valores de offset foram estabelecidos rodando outro código no Arduino, disponível
   * no GitHub. O código faz medições com o sensor MPU6050, e retorna os valores de offset
   * aproximados. 
   */
  
  // CONDIÇÃO PARA GARANTIR O SUCESSO DA ETAPA DE CONFIGURAÇÃO ---------------------
  
  if (devStatus == 0){
    Serial.println("\n       ## devStatus == 0 --------------\n");
    // HABILITA O DIGITAL MOTION PROCESSOR -----------------------------------------
    mpu.setDMPEnabled(true);
    
    // HABILITA A DETECÇÃO DE INTERRUPÇÕES PARA O DMP ------------------------------
    attachInterrupt(2, dmpDataReady, RISING);  //removido - > digitalPinToInterrupt(2);
    mpuIntStatus = mpu.getIntStatus();
    
    // FINALIZA A HABILITAÇÃO DO DMP -----------------------------------------------
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;
    
    // VERIFICA O TAMANHO DO PACOTE FIFO -------------------------------------------
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
  
  else{
    // CONDIÇÃO EXECUTADA COM A OCORRÊNCIA DE ERROS --------------------------------
    
    // CÓDIGO 1 = ERRO NO CARREGAMENTO DA MEMÓRIA INICIAL
    // CÓDIGO 2 = ERRO NA CONFIGURAÇÃO DO DMP
    
    Serial.print(F("FALHA NA INICIALIZAÇÃO DO DMP - CÓDIGO DO ERRO ("));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }

  //void MotionSystem_loop();
  Serial.println("Tudo certo com MPU6050!");
  Serial.println("\n\n>>> end: MotionSystem_setup --------------\n");
}

// ---------------------------------------------------------------------------------
// ---------------------- INÍCIO DA FUNÇÃO PRINCIPAL (LOOP) ------------------------
// --------------------------------------------------------------------------------- 

 void MotionSystem_loop(){  
  unsigned long currentMillis = millis();
  
  // CONDIÇÃO PARA ENCERRAR O CÓDIGO CASO HAJA ALGUM ERRO DE DMP -------------------
  if (!dmpReady) return;

  mpu.getAcceleration(&ax, &ay, &az);
  
  // CONDIÇÃO PARA AGUARDAR O MPU INTERRUPT ----------------------------------------
  while (!mpuInterrupt && fifoCount < packetSize){  }
  
  // RESET DO MPU INTERRUPT E LEITURA DO BYTE INT_STATUS ---------------------------
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  
  // LEITURA DA CONTAGEM DO FIRST IN FIRST OUT (FIFO) ------------------------------
  fifoCount = mpu.getFIFOCount();
  
  // CHECAGEM DE OVERFLOW ----------------------------------------------------------
  if ((mpuIntStatus & 0x10) || fifoCount == 1024){
  mpu.resetFIFO();
  }
  else
    if (mpuIntStatus & 0x02){
      while (fifoCount < packetSize)
        fifoCount = mpu.getFIFOCount();
          
      // LEITURA DO PACOTE DO FIRST IN FIRST OUT (FIFO)-----------------------------
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      fifoCount -= packetSize;
      
    
// ---------------------------------------------------------------------------------
// ------- CONVERSÃO DE RAD PARA GRAUS E ENVIO DE COMANDOS AOS SERVO MOTORES -------
// ---------------------------------------------------------------------------------

      #ifdef OUTPUT_READABLE_YAWPITCHROLL
  
      // CHAMADA DE FUNÇÕES PARA LEITURA DE DADOS ----------------------------------
      
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);      
      
      gyroXrateint = long(int(ypr[1] * -180/M_PI));
      gyroYrateint = long(int(ypr[2] * -180/M_PI));

      GyroX = String(gyroXrateint);
      GyroY = String(gyroYrateint);
            
      gForceXint = (double(ax/lsb));
      gForceYint = (double(ay/lsb));
      gForceZint = (double(az/lsb));
      
      #endif    
     }
 }
