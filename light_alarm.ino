//0 -> waiting for ldr signal
//1 -> got the ldr signal, ramping up
//2 -> fully on, waiting to get turned off
int state = 0;

//state 0 vars
const int bufferSize = 10;
int ldrBuffer[bufferSize];
int lastWrittenBufferPos = 0;
int threshold = 400;
int ldrPin = A0;

//state1 vars
unsigned long rampUpTime = 1800000; //thirty minutes in milliseconds
unsigned long startTime = 0;

//general vars
int rPin = 3;
int gPin = 5;
int bPin = 6;

void setup(){

  Serial.begin(9600); // FOR DEBUGGING PURPOSES
  
  pinMode(ldrPin, INPUT);

  pinMode(rPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(gPin, OUTPUT);

}

void loop() {
  switch (state) {
  case 0:
    state0();
    break;
  case 1:
    state1();
    break;
  case 2:
    state2();
    break;
  }

  //Serial.print("ldr: ");
  //Serial.println(analogRead(ldrPin));// FOR DEBUGGING PURPOSES
}

void state0(){
  ldrBuffer[lastWrittenBufferPos] = analogRead(ldrPin);
  //increment the buffer position unless it's exceeding 
  //the buffer size, in that case go back to zero.
  lastWrittenBufferPos = (lastWrittenBufferPos+1) % bufferSize;

  //take the average of the last reads of the ldr sensor
  int sum = 0;
  for(int i = 0; i < bufferSize; i++){
    sum += ldrBuffer[i];
  }
  int avg = sum / bufferSize;
  Serial.print("avg: ");
  Serial.println(avg);

  //if the ldr has sensed light start ramping up the leds
  if(avg > threshold){
    Serial.println("switching to state 1");// FOR DEBUGGING PURPOSES
    startTime = millis(); // start time variable that's gonna be used in state1
    state = 1; //switch to ramping up state
  }
  delay(500);
}

void state1(){
  unsigned long milliseconds = millis() - startTime;
//  Serial.print("milliseconds: ");
//  Serial.print(milliseconds);
  
  analogWrite(rPin, redRampUp(milliseconds));
  analogWrite(gPin, greenRampUp(milliseconds));
  analogWrite(bPin, blueRampUp(milliseconds));
  
  Serial.print("rPin:");
  Serial.print(redRampUp(milliseconds));
  Serial.print(" gPin: ");
  Serial.print(greenRampUp(milliseconds));
  Serial.print(" bPin: ");
  Serial.println(blueRampUp(milliseconds));// FOR DEBUGGING PURPOSES

  if(milliseconds >= rampUpTime){
    Serial.println("switching to state 2");// FOR DEBUGGING PURPOSES
    state = 2;  
  }
  
  delay(3000);
}

//returns a value for the red leds depending on 
//how many minutes have passed since going into the ramp up state
int redRampUp(unsigned long m){){
  double ratio = (double)m/(double)rampUpTime;
  return ratio*255;
}

int greenRampUp(unsigned long m){){
  double ratio = (double)m/(double)rampUpTime;
  return ratio*ratio*255;
}

int blueRampUp(unsigned long m){
  double ratio = (double)m/(double)rampUpTime;
  return ratio*ratio*ratio*255;
}

void state2(){
  digitalWrite(rPin, HIGH);
  digitalWrite(gPin, HIGH);
  digitalWrite(bPin, HIGH);
  delay(600000); //10 minutes
}
