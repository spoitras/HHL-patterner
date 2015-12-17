// Hitchhiker Laboratories Patterner
//
// Patterner by Sylvain Poitras is licensed under
// the GNU General Public License v.3
// https://www.gnu.org/copyleft/gpl.html
//
// Looping engine code adapted from Glenn Mossy's sequence recorder at 
// http://littlebits.cc/projects/sequence-recorder
// licenced under Creative Commons CC-BY-SA
// 
// Iterrupt setup code from: 
// http://playground.arduino.cc/Main/PinChangeInterrupt

#include <Metro.h>

const byte CTL00 = 10;
const byte CTL01 = 9;
const byte CTL10 = 11;
const byte CTL11 = 8;
const byte button1 = 2;
const byte button2 = 3;
const byte REC = 4;
const byte SPEED = A0;
const byte loopRED = 5;
const byte loopGREEN = 6;
Metro reset = Metro(500);
Metro loopMetro = Metro(0);
Metro playbackMetro = Metro(0);

// buffers to record state/duration combinations
// This is the maximum number of samples in bytes that can be stored.
int maxSamples = 300;   

// Create a states array to record up to this number of on and off states.
byte states[300];    

// Create a durations array and we can have up to this many duration times.
unsigned int durations[300];     

// Initialize the index for the Playback array to 0
// Initialize the index for the Record array to 1
volatile int idxPlayback  = 0;   
volatile int idxRecord    = 0;   

int sampleLength = 2;   // 2 ms

// 2=>half SPEED, 0.5=>double SPEED, change this if you want to play back 
// slower or faster than the recorded rate.
volatile float playbackRate = 1;   
volatile float recordRate=1;

volatile boolean button1ON = false;
volatile boolean button2ON = false;
volatile boolean recON = false;

volatile unsigned long seqDuration=0;

enum loopStatus {
  armed,
  recording,
  playing,
  paused,
  emptyLoop};

volatile loopStatus loopState = emptyLoop;

// Interrupt setup

void pciSetup(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

// Interrupt Service Routine (ISR)
ISR (PCINT2_vect) 
{
  // react to button1 release
  if (digitalRead (button1) == LOW && button1ON==true){

    button1ON = false;
    digitalWrite(CTL01, LOW);
    digitalWrite(CTL11, LOW);
    if(button2ON){
        digitalWrite(CTL10, HIGH);
    }else{
      digitalWrite(CTL00, HIGH);
    }
  }

  // react to button1 press
  if(digitalRead(button1)==HIGH && button1ON==false){
    //      Serial.print("doing 1");
    button1ON = true;
    if(button2ON){
      digitalWrite(CTL10, LOW);
      digitalWrite(CTL11, HIGH);
    }else{
      digitalWrite(CTL00, LOW);
      digitalWrite(CTL01, HIGH);      
    }

    
    if(loopState==armed){
      float dimmerVal = analogRead(SPEED);             
      recordRate = (dimmerVal/1023 *1.5)+0.5;
      playbackRate = recordRate;

      loopState=recording;
      recordingLED();
      memset(states, 0, sizeof(states));        // Set the size of the states array
      memset(durations, 0, sizeof(durations));  // Set the size of the durations array
      idxRecord = 0;     // reset record idx just to make playback start point obvious
      return;
    }
  }

  // react to button2 release
  if (digitalRead (button2) == LOW && button2ON==true){

    button2ON = false;
    digitalWrite(CTL10, LOW);
    digitalWrite(CTL11, LOW);
    if(button1ON){
        digitalWrite(CTL01, HIGH);
    }else{
      digitalWrite(CTL00, HIGH);
    }

  }

  // react to button2 press
  if(digitalRead(button2)==HIGH && button2ON==false){
    //      Serial.print("doing 1");
    button2ON = true;
    if(button1ON){
      digitalWrite(CTL01, LOW);
      digitalWrite(CTL11, HIGH);
    }else{
      digitalWrite(CTL00, LOW);
      digitalWrite(CTL10, HIGH);      
    }
    
    if(loopState==armed){
      float dimmerVal = analogRead(SPEED);             
      recordRate = (dimmerVal/1023 *1.5)+0.5;
      playbackRate = recordRate;

      loopState=recording;
      recordingLED();
      memset(states, 0, sizeof(states));        // Set the size of the states array
      memset(durations, 0, sizeof(durations));  // Set the size of the durations array
      idxRecord = 0;     // reset record idx just to make playback start point obvious
      return;
    }
  }

  // react to REC release
  if (digitalRead (REC) == LOW && recON==true){
    recON = false;

  }

  // react to REC press
  if(digitalRead(REC)==HIGH && recON==false){
    recON = true;
    reset.reset();
    //            Serial.print("off");

    if(loopState==recording){
      seqDuration=0;
      for(int i=0;i<idxRecord+1;i++){
        seqDuration+=durations[i];
      }

      loopMetro.interval(0);
      loopState=playing;
      playingLED();  
      return;
    }

    if(loopState==playing){
      offLED();
      pauseLED();  
      loopState=paused;


      return;
    }

    if(loopState==paused){
      playingLED();
      playbackMetro.reset();
      playbackMetro.interval(0);
      idxPlayback=0;
      loopState=playing;
      return;
    }

    if(loopState==emptyLoop){
      loopState=armed;
      recordingLED();
      return;
    } 
  }  
}
// LED helper functions
void playingLED(){
  digitalWrite(loopRED,HIGH);
  digitalWrite(loopGREEN,LOW);
}

void recordingLED(){
  digitalWrite(loopRED,LOW);
  digitalWrite(loopGREEN,HIGH);
}

void pauseLED(){
  digitalWrite(loopRED,LOW);
  digitalWrite(loopGREEN,LOW);
}

void offLED(){
  digitalWrite(loopRED,HIGH);
  digitalWrite(loopGREEN,HIGH);
  digitalWrite(CTL00, HIGH);
  digitalWrite(CTL01,LOW);
  digitalWrite(CTL10,LOW);
  digitalWrite(CTL11,LOW);
}

// setup
void setup ()
{
//  buttons
pinMode(button1, INPUT);
pinMode(button2, INPUT);
pinMode(REC, INPUT);

// button iterrupts setup
pciSetup(button1);
pciSetup(button2);
pciSetup(REC);

//  CTL outputs
pinMode(CTL00, OUTPUT);
pinMode(CTL01, OUTPUT);
pinMode(CTL10,OUTPUT); 
pinMode(CTL11,OUTPUT);  
 
// LEDS
pinMode(loopRED, OUTPUT);
pinMode(loopGREEN, OUTPUT);
offLED();
  
//  Potentiometers for playback speed
pinMode(SPEED,INPUT_PULLUP);

}  // end of setup

// main loop
void loop ()
{   
  if(recON==true && reset.check()==true){
    loopState=emptyLoop;
    memset(states, 0, sizeof(states));        // Set the size of the states array
    memset(durations, 0, sizeof(durations));  // Set the size of the durations array
    idxRecord = 0;     // reset record idx just to make playback start point obvious
    loopMetro.reset();
    loopMetro.interval(0);
    offLED();
    
  }  
  if(loopState==recording) {                          
    recordLoop();        // perform the recording loop function
  }
  if(loopState==playing){
    playbackLoop();      // perform the playback loop function
  }
}

// playback loop
void playbackLoop()
{
  if(loopMetro.check()==true){
    loopMetro.reset();
    idxPlayback=0;
    float dimmerVal = analogRead(SPEED);             
    playbackRate = (dimmerVal/1023 *1.5)+0.5;
    loopMetro.interval(seqDuration*playbackRate/recordRate);

    playbackMetro.interval(0);
    playbackMetro.reset();
    
  }
  
  if(playbackMetro.check()==true){
    playbackMetro.reset();
    float dimmerVal = analogRead(SPEED);             
    playbackRate = (dimmerVal/1023 *1.5)+0.5;
    loopMetro.interval(seqDuration*playbackRate/recordRate);
    // play the loop back at the desired SPEED
    
    if(states[idxPlayback]==0){
      digitalWrite(CTL00, HIGH);
      digitalWrite(CTL01, LOW);
      digitalWrite(CTL10, LOW);
      digitalWrite(CTL11, LOW);
    }

    if(states[idxPlayback]==1){
      digitalWrite(CTL00, LOW);
      digitalWrite(CTL01, HIGH);
      digitalWrite(CTL10, LOW);
      digitalWrite(CTL11, LOW);
    }
    if(states[idxPlayback]==2){
      digitalWrite(CTL00, LOW);
      digitalWrite(CTL01, LOW);
      digitalWrite(CTL10, HIGH);
      digitalWrite(CTL11, LOW);
    }
    if(states[idxPlayback]==3){
      digitalWrite(CTL00, LOW);
      digitalWrite(CTL01, LOW);
      digitalWrite(CTL10, LOW);
      digitalWrite(CTL11, HIGH);
    }

    playbackMetro.interval(durations[idxPlayback] * playbackRate/recordRate);  
    idxPlayback++;                                   // increment the playback index so we can play the next value
    if(idxPlayback == maxSamples) { idxPlayback=0; } // repeat the recorded loop when we reach the maximum number of samples
  }
}

// record loop
void recordLoop() {
  byte state = 0; 
  if(button1ON){
    state = state + 1;
  }
  if(button2ON){
    state = state + 2;
  }

  if(states[idxRecord] == state) {
    // if the state is not changed, add to duration of current state
    durations[idxRecord] += sampleLength;
  } else {
    // if the state is changed, go to next index (idx) and set default duration of 2ms
    idxRecord++;
    if(idxRecord == maxSamples) { idxRecord = 0; } // reset idx if max array size reached
    states[idxRecord] = state;                     // save the state to the states array by index
    durations[idxRecord] = sampleLength;           // save the duration of the sample to the durations array by index
  }
  
  delay(sampleLength); // slow the loop to a time constant so we can reproduce the timeliness of the recording
}
