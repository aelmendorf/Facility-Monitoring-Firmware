#pragma once
#include <Arduino.h>
#include <Ethernet.h>
#include <Mudbus.h>

#define DEBUG				1

#define SS_SD_CARD			4
#define SS_ETHERNET			10
#define Vref				5
#define Resistor			250
#define AVG					30
#define Step				204.6f
#define AnalogMax			1023


#define AnalogPins			16
#define Digital24VPins		16
#define PullUpDigitalPins	22
#define DigitalOutputPins	10
#define LoopTime			100
#define PrintTime			5000    

class MonitorController
{
public:
	Mudbus modbus;
	void CalibrateAnalog();
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
private:
	int sensorValue = 0;         // the sensor value
	int sensorMin = 1023;        // minimum sensor value
	int sensorMax = 0;           // maximum sensor value
	bool calMode;


	const int AnalogInputPins[AnalogPins] = { A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15 };
	const int OutputPins[DigitalOutputPins] = { 0,1,2,3,5,6,7,8,9,11 };
	const int InputPullUpPins[PullUpDigitalPins] = { 12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,39,31,32,33 };
	const int Input24VoltPins[Digital24VPins] = { 34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49 };
	 

	long lastPrint, lastLoop;
	float AnalogValues[AnalogPins] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int AnalogPinMinValues[AnalogPins] = { 1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023 };
	int AnalogPinMaxValues[AnalogPins] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	float AnalogActualValues[AnalogPins] = {3.70f,0.00f,0.00f,0.00f,0.00f,0.00f,0.00f,0.00f,0.00f,0.00f,0.00f,0.00f,0.00f,0.00f,0.00f,0.00f };

	bool OutputValues[DigitalOutputPins] = { 0 };
	bool InputPullUpValues[PullUpDigitalPins] = { 0 };
	bool Input24VoltValues[Digital24VPins] = { 0 };

};

