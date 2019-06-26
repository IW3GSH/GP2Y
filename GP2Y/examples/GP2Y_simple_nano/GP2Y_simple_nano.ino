/*
simple example programs that use GP2Y1014 to read PM2.5 values
connections:
GP2Y  - - - - - - - - -  aRDUINO
pin3        <-           D2

pin5        ->           A0

pin1       ----150ohm-----5v
            |
            220uF
            |
            _ gnd

pin6      -----    +5v
            

*/

int led = 13; //pin for alive led on arduino

int nr=100;  //number of records to skip writing data field names


// a simple "alive" timer
long sec, sec_old =0;
int minu,minu_old =0 ;
int   hr,hr_old =0;


float dust_single=0;
float dust_avg=0;
int navg=50;  //number of averaging to give a right measure
int sampletime=50;//milliseconds between each sample from GP2Y


long id=1;  //record id for serial capture




#include <GP2Y.h>

GP2Y sensor(A0,2,_NORMAL_DRIVE);  //usage  GP2Y mysensor(analog_sample_measure_pin,   IRled_drive_Pin,    driving_type)
                                  // when using ARDUINO NANO or other 5volt board use _NORMAL_DRIVE
                                  //when using ESP12 or other 3v3 board use _REVERSE_DRIVE  and place a NPN transitor as open collector inverter
                                  //  base goes to ESP output pin trough a small resitor (1-3 kohm), collector goes to PIN 3 of GP2Y


void setup(){
  pinMode(led,OUTPUT);
Serial.begin(9600);




}


 ////////////////////////MAIN LOOP/////////////////////////////////////////
void loop(){


//a simple timer
  sec = millis() / 1000;
  minu = sec / 60;
  hr = minu / 60;

if (sec-sec_old >5){  // begin of reading cycle; a reading cycle is performed every 5 seconds   %%%%%


     dust_single=sensor.readDust();  // a single sample of dust sensor, maybe useful to check what is the voltage sampled....
     dust_avg=sensor.readDustCycle(30); //usage  sensor.readDustCycle((number_of_reading_cycles, sampling_time_between_cycle [ DEFAULT= 2+10ms=12msec])
 
 

         if(nr>9){  //print heading on serialmonitor every 9 records
             Serial.print("\t");
             Serial.print("record ID: \t"); 
  
             Serial.print("last sampled Signal :\t"); 

             Serial.println(" Sharp calib. values PM2.5[ug /mc]");
             nr=1;
         }
     nr++ ;

     Serial.print("\t");
     Serial.print(id); Serial.print("\t");

     Serial.print(int(dust_single)); Serial.print("\t");  

     Serial.println(int((   dust_avg))); 

     id++;




     sec_old = sec;  //updating "alive time"
     minu_old = minu;
     hr_old=hr;

}
 // end of readin cycle  %%%%%%%%%%%%%%% 







digitalWrite(led, !digitalRead(led));
delay(1000);  

  

}
