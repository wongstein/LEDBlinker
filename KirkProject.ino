/*
  Kirk Project Pressure Sensor Code
*/
#include <Timer.h>

Timer inner_t;
Timer outer_t;
Timer blink_t;

// the setup routine runs once when you press reset:
const int numReadings = 1;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading                  // the running total
int average;                // the average

int inputPin = A0;

//User controlled variables
int max_input;
int desired_max_hertz;
double significant_hertz_diff; 

//Blink rates varaibles
int half_cycle_ms;
double current_hertz;

//for the bins
double bin_range;

void setup() {
  /*****************************************************************************
  //USER INTERFACE DON"T FORGET TO FILL
  *****************************************************************************/
  
  set_max_input(1023);
  set_desired_max_hertz(10); //also can be used for now to give maximum number of bin
  set_significant_hertz_diff(0.5); //for now
  //set_bin_mapping(0);
  /*****************************************************************************
  *****************************************************************************/
  
  // initialize serial communication with computer:
  Serial.begin(9600);
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  //pinMode(10, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
   if(!max_input || !desired_max_hertz){
     if(!max_input){
      Serial.println("Dude, you need to tell me what is the max input integer I can expect");
    };
    if(!desired_max_hertz){
       Serial.println("Dude, you need to tell me what is the max Hertz I should blink at");
    };
    /*
    if(!bin_number){
       Serial.println("Dude, you need to tell me what bin range you'd like to split up the inputed range, \n or 0 if you want me to do it for you");
    } */
    digitalWrite(LED_BUILTIN, HIGH);
    delay(10000); //ten second delay so you can get your shit together.
  };
  
  //Set up rest of calculationg
  calc_bins();
  Serial.println("here are the bins to hertz conversions");
  delay(500);
  print_bins();
  /*
  //Set up first average
  //take measurements for a second to build average
  digitalWrite(LED_BUILTIN, HIGH);
  calc_average_and_hertz_and_hertz();
 */
 
  //using timer
  record_measurement(5);
  //delay(1000); //wait a second to get enough data
  set_average(3); //let's start with taking measurements every 3 seconds
}

void loop() { 
 set_blink(1);
 inner_t.update(); 
 outer_t.update();
 
}

/*****************************************************************************
  //Calculation Functions
*****************************************************************************/
void take_measurement(){
   readings[readIndex] = analogRead(inputPin);
    readIndex += 1;
   //if readIndex is over numReadings, then let's set to 0
   if (readIndex >= numReadings) {
      readIndex = 0;
    }
}

void calc_average_and_hertz(){
  int total = 0;
  for(int i = 0; i < numReadings; i ++){
    if(readings[i] == 0){
      Serial.print(0);
    }
    total += readings[i];
  }
  average = (int)total/numReadings;
  //set current_hertz
  double new_hertz = ((average/bin_range) > 0) ? ((average/bin_range) + 1) : 0 ;
  current_hertz = new_hertz;
  calc_half_cycle_ms();
    Serial.print("here is the current average ");
    Serial.println(average);
    Serial.print("and here is what the sensor gives me ");
    Serial.println(analogRead(inputPin));
    Serial.println(' ');
    
    if (current_hertz > 0){
      Serial.print("here is half_cycle time");
      Serial.println(half_cycle_ms);
      //change pulse
      //set_blink(1);
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
    }
  /*
  if (  
      //If we have no current_hertz
        !current_hertz 
      //If there is a change in hertz and we aren't picky about how much change is significant
      || ((significant_hertz_diff == 0) && ((new_hertz - current_hertz) > 0))
      {
         Serial.print("Because hertz difference is ");
         Serial.println(new_hertz - current_hertz);
      } else  if(
      //different in hertzes is above the threshold in either hte positive or negative direction
      || (new_hertz - current_hertz) >  significant_hertz_diff
      || (new_hertz - current_hertz) < (-1 * significant_hertz_diff) 
      )
  {
    current_hertz = new_hertz;
    calc_half_cycle_ms();
    Serial.print("Because hertz difference is ");
    Serial.println(new_hertz - current_hertz);
    Serial.print("here is the current average ");
    Serial.println(average);
    Serial.print("and here is what the sensor gives me ");
    Serial.println(analogRead(inputPin));
    Serial.println(' ');
    
    if (current_hertz > 0){
      //change pulse
      outer_t.pulse(LED_BUILTIN, half_cycle_ms, LOW);
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
    }
  } */
}

void calc_bins(){
  double max_input_db = (double) max_input;
  double desired_max_hertz_db = (double) desired_max_hertz;
  //bin_range = ((double) max_input)/(double)desired_max_hertz); //maximum 30 hertz range in blinker output
  bin_range = max_input_db/desired_max_hertz_db;
  Serial.print("Bin range as double");
  //Serial.println(max_input/desired_max_hertz);
  //Serial.print("Bin range as int");
  Serial.print(bin_range);
}

void calc_half_cycle_ms(){
  double cycle_sec = 1.0/current_hertz;  //ex: 
  double cycle_ms = cycle_sec * 1000;
  //return half cycle time
  half_cycle_ms = (int)(cycle_ms/2);
}

/*****************************************************************************
  //Timer Functions
*****************************************************************************/
//recalculates the average over an inputed seconds range
void set_average(int seconds){
  Serial.println("setting average");
  int tickEvent = outer_t.every((seconds*1000), calc_average_and_hertz);
}

//take a measurement every millisecond
void record_measurement(int milliseconds){
  Serial.println("Recording measurements ");
  Serial.print("First measurement: ");
  Serial.println(analogRead(inputPin));
  int tickEvent = inner_t.every(milliseconds, take_measurement);
}

void set_blink(int seconds_run){
  //cycle by second
  for(int cycle = 0; cycle < (current_hertz * seconds_run); cycle++){
     digitalWrite(LED_BUILTIN, HIGH);
     delay(half_cycle_ms);
     digitalWrite(LED_BUILTIN, LOW);
     delay(half_cycle_ms);
  }
}

void check_pulse(int seconds){
  //int pulseEvent = blink_t.every(seconds, set_blink);
}

/*****************************************************************************
  //Human GUI functions
*****************************************************************************/
//Where input is an integer from 0-1023 that correlates to maximum amount of input volutage expected
void set_max_input(int input){
  max_input = input;
}

void set_desired_max_hertz(int current_hertz){
  desired_max_hertz = current_hertz;
}

void print_bins(){
  int start_range = 1;
  for (int hertz_count = 1; hertz_count <= desired_max_hertz; hertz_count+=1){
    Serial.print(hertz_count);
    Serial.print(": ");
    Serial.print(start_range);
    Serial.print(" - ");
    Serial.println(start_range += bin_range);
  }
  Serial.println("*****************************************************");
}

void set_significant_hertz_diff(int sig_fig){
  significant_hertz_diff = sig_fig;
}

//will calculate a significant_hert_diff based on the desired number of bins inputted
//The purpose is for the program to differentially change the Blink to correlate
//with whatever resolution the 
void set_bin_mapping(int desired_bins){
  if(desired_bins == 0 && significant_hertz_diff != 0){
    significant_hertz_diff = 0;
  } else if(desired_bins == 0 && significant_hertz_diff != 0) {
    significant_hertz_diff = double(desired_bins)/desired_max_hertz;
  }
  
}
