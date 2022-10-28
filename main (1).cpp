#include "mbed.h"

Serial user(PA_9,PA_10); // TX, RX on pins PA_9 and PA_9
Serial sms(PA_2,PA_3);

char command,room,appliance;
int mode; // 0: user is at home. 
          // 1: user is outside i.e outside mode.
          
DigitalOut TurnOff (PF_0); // All the reset pins of load side dual flip flop ( IC 4013) is connected to the  PF_0 pin of the microcontroller
DigitalIn pir (PF_1);  // Passive infrared sensor output connected to PF_1 pin of the microcontroller.
DigitalOut alarm(PA_12);

DigitalOut night_light(PA_8);
AnalogIn ldr(PA_1);  // PA_1 pin is connected to the light dependant resistor


BusOut x (PB_1,PB_6,PB_7,PB_0);  // Output pins are defined which are connected to Central Decoder.

      char AT[]="AT";     // To initialize mode
      char noecho[]="ATE0";   // To stop echo
      char mode_text[]="AT+CMGF=1";   // to set text mode
      char mobile_no[]="AT+CMGS=\"+15146324990\"";   //use to set receinpent number and mesg
      char terminator=0x1A;     // chartacter form of control + z terminator character
      char mssg[]="Intruder Detected";   // mesg we want to send

void write(char *s){
    
   while(*s)
   sms.putc(*s++);
   sms.putc(0x0D);    
    
    }
    
    void write2(char *s)
    {
        while(*s)
        sms.putc(*s++);
        
        }



int main()
{ alarm =0;
  
  x=0x00;
  user.baud(9600);  // Set baud rate to 9600 i.e. default
  sms.baud(9600);
  TurnOff = 0;
   
   while(1){
  if(user.readable()==1) // is there a character to be read?
  command=user.getc(); // Room no. is LSB and Appliance no. is MSB.
  
  room= command & 0xf0;  // AND out the unwanted bits
  appliance = command & 0xf0;  // AND out the unwanted bits
  appliance = (appliance>>4);
  
  if ( command==0x0f0f)
  { 
  mode =1;
      }
  
  if ( mode==0 && room < 0x0d){
  x=room;              // 1st 4 bit input 
  x=0x0f;              // 2st 4 bit input 
  x=appliance;         // 3st 4 bit input 
  x=0x0e;              // 4st 4 bit input 
  x=0x00;              // 5st 4 bit input 
  }
  
  if ( mode==1 ){
        TurnOff = 1;
      
      }
  
  if (mode==1 & pir == 1)
  {   alarm = 1; // alarm is triggered.
    
      write(AT);   // Sending AT command to gsm module.
      wait(2);
      write(noecho); // To stop Echo
      wait(2);
      write(mode_text);  // To set Text mode
      wait(2);
      write(mobile_no);  // sending mobile no. 
      wait(2);
      write2(mssg);  // sending the sms
      wait_ms(100);
      sms.putc(terminator);
      wait(1);
      }
      else 
      alarm =0;
  
      
  if ( ldr < 4 & pir == 1){
          night_light =1; // night light is turned on when human movement is detected at night time.
          
          } 
          else
          night_light =0;
    
}}
