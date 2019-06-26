/* simple class library to use GP2Y1010 and GP2Y1014 dust sensors from sharp
GP2Y1010 give readout related to PM10
GP2y1014 give readout related to PM2.5

to have "something" that is really related to PM and not to dust, some filtering and cycles of sampels must be done

to be honest a common readout is near 30 to 50 ug/m3 of PM2.5  in fresh air of a small city away from traffic that is somewhat MORE than gravimetric measure
calibration must be done respect to a gravimetric PM measuremnt station. After compared measurements i noticed a value from 1 to 2 times the calibrate value from
professional equipment.
Anyway response tracking is good! 

Please take this library "AS is" since i'm studying the way to optimize and make with less error


----hardware---
devices work at +5vcc
one pin get pulses from ARDUINO board or whatever similar;
one pin give out a sampled voltage;
a simple single shot measurement does not give good results in terms of PM measurements,
 to have more realistic and correlated measurments some repeated and averaged samples must be done

---connections--
GP2Y dust sensors need one +5v supply, 
GP2Y -pin3      <-  one pin from an arduino output (digital); when LOW internal IRled is activated and dust value can be sampled
GP2Y-pin5     --->   go to one analogpin  input arduino


logic can be inverted when 3v3 modules (such NODEMCU....) are used instead of ARDUINO.


---tested with--
arduino nano
nODEMCU 0.9
ESP12
D1mini (ESP12)

*/








//>>>>>>>>>>>>>>>>>>>


#define _NORMAL_DRIVE 1
#define _REVERSE_DRIVE 0


//class headings
class GP2Y
{
  public:
    GP2Y(int w_dustPin, int w_ledPowerPin, boolean w_ledMode);     //// dustPin must be an analog; typicalli use A0 for dustpin and D5 for ledPowerPin (nodeMCU)


    float dustRaw;    //measured raw adc values, for some special purposes
    float dustVal;    //calculated dust value (related to PM)
    float dustValAvg;  //averaged dust value

    float kadc = 1 ; //calibration factor for adc readout into formula
    float readDust();  //function that return raw reading single cycle
    float readDustCycle( int numcycle, int sampletime);  //function that return raw reading single cycle
    ;
  private:
    float adc_vout=0;
    float adc_avg=0;
    int navg=50;  //number of averaging to give a right measure,  50 is a good value 
                          //anomalous peaks on readout are synthoms of dust particles inside sensor isntead of PM reading, can be easily filtered watching graphs
                          //is not good to increase too much averaging, since dust and pm could merge in a mixed measure
    int sampletime=2;//milliseconds between each sample from GP2Y

    int dustPin;
    int ledPowerPin;
   
    boolean ledMode = 1 ;

    int delayTime=280;     //deafult timing recommended from SHARP
    int delayTime2=40;
    float offTime=9680;
    
};





//------------------class methods and costructors

GP2Y:: GP2Y(int w_dustPin, int w_ledPowerPin, boolean w_ledMode){
  ledPowerPin=w_ledPowerPin;
  dustPin=w_dustPin;
  ledMode=w_ledMode;
  
  pinMode(ledPowerPin,OUTPUT);
  pinMode(dustPin, INPUT); //must be an analog

  if(ledMode){digitalWrite(ledPowerPin,HIGH); }        // in normal operation led drive pin is normally high
    else {digitalWrite(ledPowerPin,LOW); }
}



float GP2Y:: readDust(){  //single cycle or reading


  if (ledMode){                                  //if positive logic control of IRLED
    digitalWrite(ledPowerPin,LOW); 
    delayMicroseconds(delayTime);
    adc_vout=analogRead(dustPin); 
    delayMicroseconds(delayTime2);
    digitalWrite(ledPowerPin,HIGH); 
    delayMicroseconds(offTime);

    dustRaw=adc_vout;
    dustVal=(    ((kadc*5* (float)adc_vout)/1024 -0.2) * 171);   //pendenza= 171 [ug/mc V]  calcolata come 500/(3.5 volt- 0.6 volt)
    return dustVal;
  } 
  else                                                   //if negative logic control of IRLED
    {digitalWrite(ledPowerPin,HIGH); 
    delayMicroseconds(delayTime);
    adc_vout=analogRead(dustPin); 
    delayMicroseconds(delayTime2);
    digitalWrite(ledPowerPin,LOW); 
    delayMicroseconds(offTime);

    dustRaw=adc_vout;
    dustVal=(    ((kadc*5*(float)(adc_vout))/1024 -0.2) * 171);   //slope= 171 [ug/mc V]  calculated (from datasheet) as  500/(3.5 volt- 0.6 volt)
    return dustVal;
 }

}





//------------------------------------read dust cycles and average
float GP2Y:: readDustCycle(int numcycle, int sampletime=2 ){

   dustValAvg=0;
   for (int k=0; k<numcycle;k++){

    

   dustValAvg += readDust();
    delay(sampletime);
      }

 dustValAvg= dustValAvg/numcycle;
 return dustValAvg;
  
}

