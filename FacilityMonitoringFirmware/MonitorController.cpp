#include "MonitorController.h"

void MonitorController::CalibrateAnalog()
{
	for (int i = 0; i < AnalogPins; i++) {
		for (int x = 0; x < 500; x++) {
			int value = analogRead(this->AnalogInputPins[i]);
			// record the maximum sensor value
			if (value > AnalogPinMaxValues[i]) {
				this->AnalogPinMaxValues[i] = value;
			}

			// record the minimum sensor value
			if (value < AnalogPinMinValues[i]) {
				this->AnalogPinMinValues[i] = value;
			}
		}
	}
}

void MonitorController::SetupAnalog()
{
	for (int i = 0; i < AnalogPins; i++) {
		pinMode(this->AnalogInputPins[i], INPUT);
	}
}

void MonitorController::Setup24VDigital()
{
	for (int i = 0; i < Digital24VPins; i++) {
		pinMode(this->Input24VoltPins[i], INPUT);
	}
}

void MonitorController::SetupPullUpDigital()
{
	for (int i = 0; i < PullUpDigitalPins; i++) {
		pinMode(this->InputPullUpPins[i], INPUT_PULLUP);
	}
}

void MonitorController::SetupOutputDigital()
{
	for (int i = 0; i < DigitalOutputPins; i++) {
		pinMode(this->OutputPins[i], OUTPUT);
	}
}

void MonitorController::ReadAnalog()
{
	for (int i = 0; i < AnalogPins; i++) {
		float read = 0;
		for (int x = 0; x < AVG; x++) {
			float val = (float)analogRead(this->AnalogInputPins[i]);
			read += val;
		}
		read = read / AVG;
		this->AnalogValues[i]=(read/Step);
		delay(10);
	}
}

void MonitorController::ReadDigital()
{
	for (int i = 0; i < PullUpDigitalPins; i++) {
		int value = digitalRead(this->InputPullUpPins[i]);
		if (value > 0) {
			this->InputPullUpValues[i] = 1; //no switch
		}else {
			this->InputPullUpValues[i] = 0; // switch triggered(alarm)
		}
	}

	for (int i = 0; i < Digital24VPins; i++) {
		int value = digitalRead(this->Input24VoltPins[i]);
		if (value > 0) {
			this->Input24VoltValues[i] = 1;
		}else {
			this->Input24VoltValues[i] = 0;
		}
	}
}

void MonitorController::UpdateModbus()
{
	for (int i = 0; i < AnalogPins; i++) {
		this->modbus.R[i] = (int)(this->AnalogValues[i]*1000);
	}


	for (int i = 0; i < PullUpDigitalPins; i++) {
		this->modbus.C[i] = this->InputPullUpValues[i];
	}

	int offset = PullUpDigitalPins;

	for (int i = 0; i < Digital24VPins; i++) {
		this->modbus.C[i + PullUpDigitalPins] = this->Input24VoltValues[i];
	}

	offset += Digital24VPins;

	for (int i = 0; i < DigitalOutputPins; i++) {
		this->modbus.C[i + offset] = this->OutputValues[i];
	}
}

void MonitorController::Init()
{
	this->lastLoop = 0;
	this->lastPrint = 0;
	pinMode(SS_SD_CARD, OUTPUT);
	pinMode(SS_ETHERNET, OUTPUT);
	digitalWrite(SS_ETHERNET, LOW);  // Ethernet ACTIVE
	digitalWrite(SS_ETHERNET, HIGH);  // SD Card Deactivated

	this->SetupAnalog();
	this->Setup24VDigital();
	this->SetupPullUpDigital();
	this->SetupOutputDigital();
#if DEBUG
	Serial.begin(38400);
	this->Print();
#endif
}

void MonitorController::Run()
{
	modbus.Run();
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
	Serial.print("Min: "); Serial.print(sensorMin);
	Serial.print(" Max: "); Serial.print(sensorMax);
	Serial.println();
	Serial.println("Digital Pull-up Pins");
	for (int i = 0; i < PullUpDigitalPins; i++) {
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
	for (int i = 0; i < AnalogPins; i++) {
		Serial.print(" P"); Serial.print(i);
		Serial.print(": "); Serial.print(AnalogValues[i]);
	}
	Serial.println();

	//Serial.println("Analog Min Cal");
	//for (int i = 0; i < AnalogPins; i++) {
	//	Serial.print(" Min"); Serial.print(i);
	//	Serial.print(": "); Serial.print(this->AnalogPinMinValues[i]);
	//}
	//Serial.println();

	//Serial.println("Analog Max Cal");
	//for (int i = 0; i < AnalogPins; i++) {
	//	Serial.print(" Max"); Serial.print(i);
	//	Serial.print(": "); Serial.print(this->AnalogPinMaxValues[i]);
	//}
	//Serial.println();
}

