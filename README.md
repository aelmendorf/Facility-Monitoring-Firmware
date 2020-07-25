# Facility-Monitoring-Firmware
Generic Facility Monitor Box Firmware with Modbus TCP Protocol

Designed around being scalable and ease of use.  Basic idea is facility technicians can add,remove,change sensors by simply plugging them in 
then modifying through facility monitoring website(seperate repo https://github.com/aelmendorf/Facility-Monitoring).

Available Channels: These were chosen based on industry standard gas sensor and switch specifications
* 10 5v,3amp outputs
* 24 24v,<1amp discrete inputs
* 16 5v pullup discrete inputs
* 16 analog(current 0-20mA) inputs

Task:
* Read digital and analog inputs then update modbus register.  
* Recieve commands from server then update outputs accordingly

Monitoring Box images below:

![Alt Text](https://drive.google.com/uc?export=view&id=1x6pShu-5J5-v-ukDgh3S6b1UDACBadIJ)

![Alt Text](https://drive.google.com/uc?export=view&id=1wnOXVPVdWtS8vbOuF2WbyvdsSSsC0S7G)

