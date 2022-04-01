#include <PWM.h>

#define VOLT_DIV 11
#define MAX_DUTY 96
#define MIN_DUTY 6

//Initializing LED Pin
int mosfet_pin = 3;
int32_t frequency = 10000;  //frequency (in Hz)
//int32_t frequency = 8000;  //frequency (in Hz)

int PV_pos_pin = A1;
int PV_neg_pin = A0;
int PV_pos_val = 0;
int PV_neg_val = 0;

double PV_volt = 0;
double PV_current = 0;
uint8_t counter = 0;
//double duty = 50;
double duty = 85;
//double duty_inc = 0.5;
double duty_inc = 3;
double prev_power = 0;
double prev_volt = 0;
double prev_current = 0;

void setup() {
  //Declaring LED pin as output
//  pinMode(led_pin, OUTPU/T);
  Serial.begin(9600);


  //initialize all timers except for 0, to save time keeping functions
  InitTimersSafe(); 
  //sets the frequency for the specified pin
  bool success = SetPinFrequencySafe(mosfet_pin, frequency);
  if(success) 
  {
//    pinMode(13, OUTPUT);
//    digitalWrite(13, HIGH);
    pinMode(mosfet_pin,OUTPUT);
  }
}

void loop() {
  
  if (duty > MAX_DUTY)
  {
    duty = MAX_DUTY-2;
  }
  if (duty < MIN_DUTY)
  {
    duty = MIN_DUTY+2;
  }


  if ((counter % 12) == 0)
  {
    // USE THIS TO FIND MPPT
    duty = cal_duty(PV_volt, PV_current);

    // USE THIS TO DO A SWEEP
//    duty += duty_inc; // (might need to config other variables)
//    if (duty > MAX_DUTY-3)
//      duty = MIN_DUTY;
  }
  
//  analogWrite(mosfet_pin, duty); //duty cycle 15%
  pwmWrite(mosfet_pin, duty_calibrate());
  delayMicroseconds(1000000/(frequency*2)); // delay by 50 micro seconds (half the period)
  
  counter++;
  int slow = 20000;
  int mid = 10000;
  int fmid = 5000;
  int fast = 1000;
  int vfast = 500;
  if ((counter % 3) == 0)
  {
    double tmp_value = 0.0;
    double tmp_sum = 0.0;
    
    for (int ii=0; ii<5; ii++)
    {
      tmp_value = analogRead(PV_pos_pin);
      if (tmp_value != 0)
      {
        tmp_sum += tmp_value;
      }
      else
      {
        ii--;
      }
      delay(3);
    }
    PV_pos_val = int(tmp_sum/5.0);

    tmp_value = 0.0;
    tmp_sum = 0.0;
    for (int ii=0; ii<3; ii++)
    {
      tmp_value += analogRead(PV_neg_pin);
      delay(3);
    }
    PV_neg_val = int(tmp_value/3.0);
    
    PV_volt  = double(PV_pos_val - PV_neg_val) * double(5.0/1023.0) * VOLT_DIV;
    ////////  I = I1 + I2 = (V_pv/R_total) + (V_pv * duty^2/R_2)  ////////
    PV_current = PV_volt/11000.0 + PV_volt*(duty/100.0)*(duty/100.0)/10.0;
    
    serial_comms(PV_volt, PV_current, duty);
  }
}


static double duty_calibrate()
{
  return double(256.0 * float(duty/100.0));
}



static void serial_comms(double volt, double current, double duty_cycle)
{
  double PV_power = volt * current;
  if (PV_power > 7) // invalid data
  {
    return;
  }
  //print voltage
  Serial.print("PV_vol: ");
  Serial.print(PV_volt, 5);
  Serial.println("");

  //print current
  Serial.print("PV_cur: ");
  Serial.print(PV_current, 5);
  Serial.println("");

  //print power
  Serial.print("PV_pow: ");
  Serial.print(PV_power, 5);
  Serial.println("");

  //print duty
  Serial.print("PV_dut: ");
  Serial.print(duty_cycle, 5);
  Serial.println("");
}

double max_power = 0;
static double cal_duty(double loc_PV_volt, double loc_PV_current)
{ 
  double loc_PV_power = loc_PV_volt * loc_PV_current;
  if (loc_PV_power > 7) // invalid data
  {
    return duty;
  }

  if (prev_power < loc_PV_power)
  {
    max_power = loc_PV_power;
//    duty_inc = duty_inc * abs(max_power - loc_PV_power)*(0.75);
  }
  
  double delta_power = loc_PV_power - prev_power;
  double delta_volt = loc_PV_volt - prev_volt;
  double new_inc = 4;
  double d;
//  double delta_volt_threshold = 0.0001;
//  double delta_power_threshold = 0.0001;
  if (loc_PV_power != 0)
  {
    new_inc = duty_inc * max_power / loc_PV_power * 0.60;
  }
  if ((new_inc <= 0.5 || new_inc >= 8) && (max_power / loc_PV_power) > 1.10)
  {
    new_inc = duty_inc;
  }
  
  if (loc_PV_power > prev_power)
  {
    //check delta volt
    if (loc_PV_volt > prev_volt)
    {
      d = duty - new_inc;
    }
    else if (loc_PV_volt < prev_volt)
    {
      d = duty + new_inc;
    }
    else
    {
      d = duty;
    }
  }
  else if (loc_PV_power < prev_power)
  {
    if (loc_PV_volt > prev_volt)
    {
      d = duty + new_inc;
    }
    else if (loc_PV_volt < prev_volt)
    {
      d = duty - new_inc;
    }
    else
    {
      d = duty;
    }
  }
  else
  {
    d = duty;
  }

// set previous values with new values
  prev_power = loc_PV_power;
  prev_volt = loc_PV_volt;
  prev_current = loc_PV_current;

  if (d > MAX_DUTY)
  {
    return MAX_DUTY-2;
  }
  if (d < MIN_DUTY)
  {
    return MIN_DUTY+2;
  }
  
  return d;
}
