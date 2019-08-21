#pragma once
void(*resetFunc) (void) = 0; //declare reset function @ address 0
template <class T> int EEPROM_write(int addr, const T& value)
{
	const byte* p = (const byte*)(const void*)& value;
	int newAddr;
	for (newAddr = 0; newAddr < sizeof(value); newAddr++)
	{
		EEPROM.write(addr++, *p++);
	}
	return newAddr;
}//End write any value/type

template <class T> int EEPROM_read(int addr, T& value)
{
	byte* p = (byte*)(void*)& value;
	int newAddr;
	for (newAddr = 0; newAddr < sizeof(value); newAddr++)
		* p++ = EEPROM.read(addr++);
	return newAddr;
}//End read any value/type

float convert(int raw1, int raw2)
{
	int raw[2];
	raw[1] = raw1;
	raw[0] = raw2;
	long result = 0;
	memcpy(&result, &raw, sizeof(long));
	return (float)result;
}//End convert

/**
 * convertToFloat
 *  Converts 2 integers to float, read in from mbus
 */
float convertToFloat(int raw1, int raw2)
{
	unsigned int raw[2];
	raw[1] = raw1;
	raw[0] = raw2;
	float result;
	memcpy(&result, &raw, sizeof(float));
	return result;
}//End convert to float

/*
 * extract16bit(long)
 *   converting 32bit long into 2 16bit integers(cannot be negative)
 */
unsigned int* extract16bit(long x)
{
	static unsigned int result[2];
	result[0] = (unsigned int)(x >> 16);
	result[1] = (unsigned int)(x & 0x0000FFFF);
	return result;
}//convert long into 2 16bit registers