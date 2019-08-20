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
	const int AnalogInputPins[AnalogPins] = { A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15 };
	const int OutputPins[DigitalOutputPins] = { 0,1,2,3,5,6,7,8,9,11 };
	const int InputPullUpPins[PullUpDigitalPins] = { 12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,39,31,32,33 };
	const int Input24VoltPins[Digital24VPins] = { 34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49 };
	//const float SlopeValues[AnalogPins] = { 1.00476,1.00577,1.004007,1.00488,1.00448,1.00454,.99849,0.000,1.00448,1.00455,1.00450,1.00451,1.00456,1.00414,1.00456,0 };
	//const float OffsetValues[AnalogPins] = { 0.00998,0.00920,.008976,0.00919,0.00744,0.00791,-.00462,0.0000,0.00787,0.00763,0.00761,0.00761,0.00761,0.00833,0.00782,0.0000 };
	//const float RValues[AnalogPins] = { 250.81,250.474,246.2776,210.223,250.58,240.204,332.018,0.000,250.902,250.918,250.684,250.808,251.002,250.576,250.821,0.000 };
	 
	bool Alarm = false;
	long lastPrint, lastLoop;
	float AnalogValues[AnalogPins] = { 0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f };
	float AnalogCurrentValues[AnalogPins] = { 0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f};

	bool OutputValues[DigitalOutputPins] = { 0 };
	bool InputPullUpValues[PullUpDigitalPins] = { 0 };
	bool Input24VoltValues[Digital24VPins] = { 0 };

};

