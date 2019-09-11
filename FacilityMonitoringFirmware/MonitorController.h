#pragma once

#include <Arduino.h>
#include <Ethernet.h>
#include <Mudbus.h>

#define DEBUG				0

#define Vref				5
#define Resistor			250
#define AVG					30
#define Step				204.6f
#define AnalogMax			1023


//Pins && Index
#define SS_SD_CARD			4
#define SS_ETHERNET			10
#define AnalogInputPins		16
#define DigitalPullUpPins	22
#define Digital24VPins		16
#define DigitalOutputPins	10
#define SoftwareOuputPins	7
#define HardwareOutputPins	3

#define SystemOkayIndex		7
#define SystemWarningIndex	8
#define SystemAlarmIndex	9

#define KeySwitchIndex		21

#define HardwareMaintIndex	38
#define CoilComIndex		39
#define SoftMaintModeIndex	40
#define WarningIndex		41
#define AlarmIndex			42
#define InputRegIndex		16 //AnalogInputPins

//Timing
#define LoopTime			50
#define PrintTime			5000 
#define ResetTime			604800000

enum ControllerState {OKAY,WARNING,ALARM,MAINTENCE};

class MonitorController
{
public:
	Mudbus modbus;
	void SetupPins();
	void ReadAnalog();
	void ReadDigital();
	void UpdateModbus();
	void CheckCom();
	void CheckState();
	void SetLights();

	void DisplaySystemOkay();
	void DisplaySystemWarning();
	void DisplaySystemAlarm();
	void DisplayMaintenance();

	void Print();
	void Init();
	void Run();
private:

	const int AnalogPins[AnalogInputPins] = { A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15 };
	const int OutputPins[DigitalOutputPins] = { 0,1,2,3,5,6,7,8,9,11 };
	const int InputPullUpPins[DigitalPullUpPins] = { 12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,39,31,32,33};
	const int Input24VoltPins[Digital24VPins] = { 34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49};
	
	bool hardwareMaintMode = false;
	bool softwareMaintMode = false;
	bool warning = false;
	bool alarm = false;

	ControllerState State;

	long lastPrint, lastLoop;
	float AnalogValues[AnalogInputPins] = { 0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f };
	float AnalogCurrentValues[AnalogInputPins] = { 0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f,0.000f};
	int OutputDefaults[DigitalOutputPins] = { LOW,LOW,LOW,LOW,LOW,LOW,HIGH,LOW,HIGH,HIGH};
	int OutputValues[DigitalOutputPins] = { 0 };
	int ModbusOutputValues[DigitalOutputPins] = { 0 };
	bool InputPullUpValues[DigitalPullUpPins] = { 0 };
	bool Input24VoltValues[Digital24VPins] = { 0 };

};
