/*
 Name:		FacilityMonitoringFirmware.ino
 Created:	8/14/2019 12:15:59 PM
 Author:	AElmendo
*/

#include <Ethernet.h>
#include <Mudbus.h>
#define DEBUG 1

#define SS_SD_CARD      4
#define SS_ETHERNET     10
#define Vref        5
#define Resistor      250
#define AVG         5
#define Step        204.6f


#define AnalogPins      16
#define Digital24VPins    16
#define PullUpDigitalPins 22
#define DigitalOutputPins 10
#define LoopTime      100
#define PrintTime     2000    

void SetupAnalog();
void Setup24VDigital();
void SetupPullUpDigital();
void SetupOutputDigital();
void ReadAnalog();
void ReadDigital();
void UpdateModbus();
void Print();
void Init();
void Run();

IPAddress ip(172, 20, 1, 62);
IPAddress subnet(255, 255, 255, 0);
byte macAddress[6] = { 0x90, 0xA2, 0xDA, 0x00, 0xC4, 0x5A };
Mudbus modbus;

const int AnalogInputPins[AnalogPins] = { A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15 };
const int OutputPins[DigitalOutputPins] = { 0,1,2,3,5,6,7,8,9,11 };
const int InputPullUpPins[PullUpDigitalPins] = { 12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,39,31,32,33 };
const int Input24VoltPins[Digital24VPins] = { 34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49 };

long lastPrint, lastLoop;
int AnalogValues[AnalogPins] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
bool OutputValues[DigitalOutputPins] = { 0 };
bool InputPullUpValues[PullUpDigitalPins] = { 0 };
bool Input24VoltValues[Digital24VPins] = { 0 };

void setup() {
	// put your setup code here, to run once:
	Init();
}

void loop() {
	Run();
}

void Init()
{
	lastLoop = 0;
	lastPrint = 0;
	pinMode(SS_SD_CARD, OUTPUT);
	pinMode(SS_ETHERNET, OUTPUT);
	digitalWrite(SS_ETHERNET, LOW);  // Ethernet ACTIVE
	digitalWrite(SS_ETHERNET, HIGH);  // SD Card Deactivated

	SetupAnalog();
	Setup24VDigital();
	SetupPullUpDigital();
	SetupOutputDigital();

	Ethernet.begin(macAddress, ip, subnet);

#if DEBUG
	Serial.begin(38400);
#endif
}

void Run()
{
	if (millis() >= (LoopTime + lastLoop)) {
		ReadDigital();
		ReadAnalog();
		UpdateModbus();
		lastLoop += LoopTime;
	}
#if DEBUG
	if (millis() >= (PrintTime + lastPrint)) {
		Print();
		lastPrint += PrintTime;
	}

#endif // DEBUG
}

void SetupAnalog()
{
	for (int i = 0; i < AnalogPins; i++) {
		pinMode(AnalogInputPins[i], INPUT);
	}
}

void Setup24VDigital()
{
	for (int i = 0; i < Digital24VPins; i++) {
		pinMode(Input24VoltPins[i], INPUT);
	}
}

void SetupPullUpDigital()
{
	for (int i = 0; i < PullUpDigitalPins; i++) {
		pinMode(InputPullUpPins[i], INPUT_PULLUP);
	}
}

void SetupOutputDigital()
{
	for (int i = 0; i < DigitalOutputPins; i++) {
		pinMode(OutputPins[i], OUTPUT);
	}
}

void ReadAnalog()
{
	for (int i = 0; i < AnalogPins; i++) {
		int read = 0;
		for (int x = 0; x < AVG; x++) {
			read += analogRead(AnalogInputPins[i]);
			delayMicroseconds(500);
		}
		read = read / AVG;
		AnalogValues[i] = read;
	}
}

void ReadDigital()
{
	for (int i = 0; i < PullUpDigitalPins; i++) {
		int value = digitalRead(InputPullUpPins[i]);
		if (value = HIGH) {
			InputPullUpValues[i] = 1; //no switch
		}
		else if (value = LOW) {
			InputPullUpValues[i] = 0; // switch triggered(alarm)
		}
	}

	for (int i = 0; i < Digital24VPins; i++) {
		int value = digitalRead(Input24VoltPins[i]);
		if (value = HIGH) {
			Input24VoltValues[i] = 1;
		}
		else {
			Input24VoltValues[i] = 0;
		}
	}
}

void UpdateModbus()
{
	for (int i = 0; i < PullUpDigitalPins; i++) {
		modbus.C[i] = InputPullUpValues[i];
	}

	for (int i = 0; i < Digital24VPins; i++) {
		modbus.C[i + PullUpDigitalPins] = Input24VoltValues[i];
	}

	for (int i = 0; i < AnalogPins; i++) {
		modbus.R[i] = AnalogValues[i];
	}
}

void Print()
{
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
}
