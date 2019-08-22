#include "MonitorController.h"


void MonitorController::SetupPins() {

	//enable ethernet board
	pinMode(SS_SD_CARD, OUTPUT);
	pinMode(SS_ETHERNET, OUTPUT);
	digitalWrite(SS_ETHERNET, LOW);  // Ethernet ACTIVE
	digitalWrite(SS_ETHERNET, HIGH);  // SD Card Deactivated

	for (int i = 0; i < AnalogInputPins; i++) {
		pinMode(this->AnalogPins[i], INPUT);
	}

	for (int i = 0; i < Digital24VPins; i++) {
		pinMode(this->Input24VoltPins[i], INPUT);
	}

	for (int i = 0; i < DigitalPullUpPins; i++) {
		pinMode(this->InputPullUpPins[i], INPUT_PULLUP);
	}

	for (int i = 0; i < DigitalOutputPins; i++) {
		pinMode(this->OutputPins[i], OUTPUT);
		delay(1);
		digitalWrite(this->OutputPins[i],this->OutputDefaults[i]);
	}
}

void MonitorController::ReadAnalog()
{
	for (int i = 0; i < AnalogInputPins; i++) {
		float read = 0;
		for (int x = 0; x < AVG; x++) {
			float val = (float)analogRead(this->AnalogPins[i]);
			read += val;
		}
		read = read / AVG;
		this->AnalogValues[i]=(read/Step);
		delay(1);
	}
}

void MonitorController::ReadDigital()
{
	for (int i = 0; i < DigitalPullUpPins; i++) {
		int value = digitalRead(this->InputPullUpPins[i]);
		if (value==LOW) {
			this->InputPullUpValues[i] = 1; //switch(Warning)
		}else {
			this->InputPullUpValues[i] = 0; // no Switch
		}
	}

	for (int i = 0; i < Digital24VPins; i++) {
		int value = digitalRead(this->Input24VoltPins[i]);
		if (value > 0) {
			this->Input24VoltValues[i] = 1; //switch(Warning)  
		}else {
			this->Input24VoltValues[i] = 0; //no switch
		}
	}
}

void MonitorController::UpdateModbus()
{
	for (int i = 0; i < AnalogInputPins; i++) {
		this->modbus.R[i] = (int)(this->AnalogValues[i]*1000);
	}

	for (int i = 0; i < DigitalPullUpPins; i++) {
		this->modbus.C[i] = this->InputPullUpValues[i];
	}

	int offset = DigitalPullUpPins;

	for (int i = 0; i < Digital24VPins; i++) {
		this->modbus.C[i + offset] = this->Input24VoltValues[i];
	}
}

void MonitorController::CheckModbusInput()
{	
	if (this->modbus.C[CoilComIndex]) {
		for (int i = 0; i < DigitalOutputPins; i++) {
			if (this->OutputValues[i] != modbus.R[i + InputRegIndex]) {
				this->OutputValues[i] = modbus.R[i + InputRegIndex];
				digitalWrite(this->OutputPins[i], this->OutputValues[i]);
			}
		}
		this->modbus.C[CoilComIndex] = false;
	}
}

void MonitorController::Init()
{
	this->lastLoop = 0;
	this->lastPrint = 0;
	this->SetupPins();
	for (int i = 0; i < MB_N_C; i++) {
		this->modbus.C[i] = false;
	}

	for (int i = 0; i < MB_N_R; i++) {
		this->modbus.R[i] = 0;
	}

#if DEBUG
	Serial.begin(38400);
	this->Print();
#endif
}

void MonitorController::Run()
{
	modbus.Run();
	this->CheckModbusInput();
	if (millis() >= (LoopTime + this->lastLoop)) {
		this->ReadDigital();
		this->ReadAnalog();
		this->UpdateModbus();		
		this->lastLoop += LoopTime;
	}
#if DEBUG
	if (millis() >= (PrintTime + this->lastPrint)) {
		this->Print();
		this->lastPrint += PrintTime;
	}
#endif // DEBUG
}

void MonitorController::Print()
{
	Serial.println("Digital Pull-up Pins");
	for (int i = 0; i < DigitalPullUpPins; i++) {
		Serial.print(" P"); Serial.print(i);
		if (InputPullUpValues[i]) {
			Serial.print(": High");
		}
		else {
			Serial.print(": Low");
		}
	}
	Serial.println();

	Serial.println("Digital Pull-up Pins");
	for (int i = 0; i < Digital24VPins; i++) {
		Serial.print(" P"); Serial.print(i);
		if (Input24VoltValues[i]) {
			Serial.print(": High");
		}
		else {
			Serial.print(": Low");
		}
	}
	Serial.println();

	Serial.println("Analog Pins");
	for (int i = 0; i < AnalogInputPins; i++) {
		Serial.print(" P"); Serial.print(i);
		Serial.print(": "); Serial.print(AnalogValues[i]);
	}
	Serial.println();
}

