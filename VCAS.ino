// VCAS
// Velocidade da aeronave
#include "SDP6x.h"

float difPressure;

void vcas_setup(){
  Serial.println("\n\n>>> init: vcas_setup --------------\n");
  difPressure = 0.0;
  Serial.println("\n\n>>> end: vcas_setup --------------\n");
}

void vcas_loop(){
  PVcas = SDP6x.GetPressureDiff();
  vcas = 0;

  if (PVcas > 0){
    vcas = sqrt ((2.0 * PVcas) / 1.225);
  }
}
