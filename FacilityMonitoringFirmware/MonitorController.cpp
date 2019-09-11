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
		this->ModbusOutputValues[i] = this->OutputDefaults[i];
		pinMode(this->OutputPins[i], OUTPUT);
		delay(1);
		digitalWrite(this->OutputPins[i], this->OutputDefaults[i]);
	}
}

void MonitorController::ReadAnalog() {
	for (int i = 0; i < AnalogInputPins; i++) {
		float read = 0;
		for (int x = 0; x < AVG; x++) {
			float val = (float)analogRead(this->AnalogPins[i]);
			read += val;
		}
		read = read / AVG;
		this->AnalogValues[i] = (read / Step);
		delay(1);
	}
}

void MonitorController::ReadDigital() {
	for (int i = 0; i < DigitalPullUpPins; i++) {
		int value = digitalRead(this->InputPullUpPins[i]);
		if (value == LOW) {
			this->InputPullUpValues[i] = true; //switch(Warning)
		} else {
			this->InputPullUpValues[i] = false; // no Switch
		}
	}

	if (this->InputPullUpValues[KeySwitchIndex]) {
		this->hardwareMaintMode = true;
	} else {
		this->hardwareMaintMode = false;
	}

	for (int i = 0; i < Digital24VPins; i++) {
		int value = digitalRead(this->Input24VoltPins[i]);
		this->Input24VoltValues[i] = value;
	}
}

void MonitorController::UpdateModbus() {

	for (int i = 0; i < AnalogInputPins; i++) {
		this->modbus.R[i] = (int)(this->AnalogValues[i] * 1000);
	}

	for (int i = 0; i < DigitalPullUpPins; i++) {
		this->modbus.C[i] = this->InputPullUpValues[i];
	}

	int offset = DigitalPullUpPins;

	for (int i = 0; i < Digital24VPins; i++) {
		this->modbus.C[i + offset] = this->Input24VoltValues[i];
	}

	if (!modbus.C[CoilComIndex]) {
		offset = AnalogInputPins;
		for (int i = 0; i < DigitalOutputPins; i++) {
			this->modbus.R[i + offset] = this->ModbusOutputValues[i];
		}
	}
	offset = DigitalOutputPins + AnalogInputPins;
	this->modbus.R[offset] = this->State;
}

void MonitorController::CheckCom() {
	if (this->modbus.C[CoilComIndex]) {

		this->softwareMaintMode = this->modbus.C[SoftMaintModeIndex];
		this->alarm = this->modbus.C[AlarmIndex];
		this->warning = this->modbus.C[WarningIndex];
		for (int i = 0; i < SoftwareOuputPins; i++) {
			this->ModbusOutputValues[i] = modbus.R[i + InputRegIndex];
		}
		this->modbus.C[CoilComIndex] = false;
	}

}

void MonitorController::CheckState() {
	if (this->softwareMaintMode || this->hardwareMaintMode) {
		if (this->hardwareMaintMode && this->softwareMaintMode) {
			this->softwareMaintMode = false;
			this->modbus.C[HardwareMaintIndex] = true;
			this->modbus.C[SoftMaintModeIndex] = false;			
		} else if (this->hardwareMaintMode && !this->softwareMaintMode) {
			this->modbus.C[HardwareMaintIndex] = true;
		} else if (!this->hardwareMaintMode && this->softwareMaintMode) {
			this->modbus.C[HardwareMaintIndex] = false;
		}
		this->State = MAINTENCE;
	} else {
		this->modbus.C[HardwareMaintIndex] = false;
		if (this->alarm || this->warning) {
			if (this->alarm) {
				this->State = ALARM;
			} else if (this->warning) {
				this->State = WARNING;
			}
		} else {
			this->State = OKAY;
		}
	}
	this->SetLights();
}

void MonitorController::SetLights() {
	switch (this->State) {
		case OKAY: {
			this->DisplaySystemOkay();
			break;
		}
		case WARNING: {
			this->DisplaySystemWarning();
			break;
		}
		case ALARM: {
			this->DisplaySystemAlarm();
			break;
		}
		case MAINTENCE: {
			this->DisplayMaintenance();
			break;
		}
	}
}

void MonitorController::Init() {
	this->lastLoop = 0;
	this->lastPrint = 0;
	this->alarm = false;
	this->warning = false;
	this->hardwareMaintMode = false;
	this->softwareMaintMode = false;
	this->State = OKAY;
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

void MonitorController::Run() {
	modbus.Run();
	if (millis() >= (LoopTime + this->lastLoop)) {
		this->ReadDigital();
		this->ReadAnalog();
		this->CheckCom();
		this->CheckState();
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

void MonitorController::DisplaySystemOkay() {
	this->ModbusOutputValues[SystemOkayIndex] = LOW;
	this->ModbusOutputValues[SystemWarningIndex] = HIGH;
	this->ModbusOutputValues[SystemAlarmIndex] = HIGH;
	digitalWrite(this->OutputPins[SystemOkayIndex], LOW);
	digitalWrite(this->OutputPins[SystemWarningIndex], HIGH);
	digitalWrite(this->OutputPins[SystemAlarmIndex], HIGH);
}

void MonitorController::DisplaySystemWarning() {
	this->ModbusOutputValues[SystemOkayIndex] = HIGH;
	this->ModbusOutputValues[SystemWarningIndex] = LOW;
	this->ModbusOutputValues[SystemAlarmIndex] = HIGH;
	digitalWrite(this->OutputPins[SystemOkayIndex], HIGH);
	digitalWrite(this->OutputPins[SystemWarningIndex], LOW);
	digitalWrite(this->OutputPins[SystemAlarmIndex], HIGH);
}

void MonitorController::DisplaySystemAlarm() {
	this->ModbusOutputValues[SystemOkayIndex] = HIGH;
	this->ModbusOutputValues[SystemWarningIndex] = HIGH;
	this->ModbusOutputValues[SystemAlarmIndex] = LOW;
	digitalWrite(this->OutputPins[SystemOkayIndex], HIGH);
	digitalWrite(this->OutputPins[SystemWarningIndex], HIGH);
	digitalWrite(this->OutputPins[SystemAlarmIndex], LOW);
}

void MonitorController::DisplayMaintenance() {
	this->ModbusOutputValues[SystemOkayIndex] = LOW;
	this->ModbusOutputValues[SystemWarningIndex] = LOW;
	this->ModbusOutputValues[SystemAlarmIndex] = HIGH;
	digitalWrite(this->OutputPins[SystemOkayIndex], LOW);
	digitalWrite(this->OutputPins[SystemWarningIndex], LOW);
	digitalWrite(this->OutputPins[SystemAlarmIndex], HIGH);
}

void MonitorController::Print() {
	Serial.println("Digital Pull-up Pins");
	for (int i = 0; i < DigitalPullUpPins; i++) {
		Serial.print(" P"); Serial.print(i);
		if (InputPullUpValues[i]) {
			Serial.print(": High");
		} else {
			Serial.print(": Low");
		}
	}
	Serial.println();

	Serial.println("Digital Pull-up Pins");
	for (int i = 0; i < Digital24VPins; i++) {
		Serial.print(" P"); Serial.print(i);
		if (Input24VoltValues[i]) {
			Serial.print(": High");
		} else {
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

