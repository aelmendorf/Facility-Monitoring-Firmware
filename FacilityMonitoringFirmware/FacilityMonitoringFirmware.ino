/*
 Name:		FacilityMonitoringFirmware.ino
 Created:	8/14/2019 12:15:59 PM
 Author:	Andrew Elmendorf
*/

#include <Ethernet.h>
#include <Mudbus.h>
#include "MonitorController.h"

IPAddress ip(172, 21, 100, 30);
IPAddress subnet(255, 255, 0, 0);
//byte macAddress[6] = { 0x90, 0xA2, 0xDA, 0x00, 0xC4, 0x5A };
byte macAddress[6] = { 0x90, 0xA2, 0xDA, 0x00, 0xC4, 0x6A };
MonitorController Controller;

void setup() {
	Controller.Init();
	Ethernet.begin(macAddress, ip, subnet);
}

void loop() {
	Controller.Run();
}



