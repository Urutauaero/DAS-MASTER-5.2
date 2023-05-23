//Pressure / Humidity
// Sistema que informa a temperatura e altura do local
//#include "BME680.h" // Modulo do sensor de umidade, temperatura e pressão

#define SEALEVELPRESSURE_HPA (1013.25)

BME680_Class BME680;

float calcAltitude(float pressureHP, const float seaLevel = 1013.25) // Fórmula do calculo de altitude feita através dos dadso fornecidos pelo sensor
{
  float Altitude = 44330.0 * (1.0 - pow(pressureHP / seaLevel, 0.1903));
  return (Altitude); // Retorno do cálculo feito e a altura aproximada (ou até exata)
}

void hp_setup(){
  Serial.println("\n\n>>> init: hp_setup --------------\n");

  if(!BME680.begin(I2C_STANDARD_MODE)){                                                   //
        Serial.println("Problema ao encontrar o barômetro. Verifique as ligações."); //
  }
  else{
      BME680.setOversampling(TemperatureSensor, Oversample2);
      BME680.setOversampling(HumiditySensor, SensorOff);
      BME680.setOversampling(PressureSensor, Oversample2);
      BME680.setIIRFilter(IIR8);
      BME680.setGas(0, 0);
  }
  Serial.println("Tudo certo com o BME!");
  Serial.println("\n\n>>> end: hp_setup --------------\n");
}

void hp_loop(){
  static int32_t temperature, humidity, pressure, gas;
  BME680.getSensorData(temperature, humidity, pressure, gas);

  int temp_x100 = temperature;
  temp = temp_x100 / 100.0;
  pbmp = pressure / 100.0;

  hp = calcAltitude(pbmp, SEALEVELPRESSURE_HPA);
}
