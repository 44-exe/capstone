#include <PWM.h>

#define VOLT_DIV 11
#define MAX_DUTY 96
#define MIN_DUTY 4

//#define MAX_DUTY 245
//#define MIN_DUTY 10


//Initializing LED Pin
int mosfet_pin = 3;
//int32_t frequency = 10000;  //frequency (in Hz)
int32_t frequency = 8000;  //frequency (in Hz)

//int PV_pos_pin = A2;
//int PV_neg_pin = A0;
int PV_pos_pin = A1;
int PV_neg_pin = A0;
int PV_pos_val = 0;
int PV_neg_val = 0;
double PV_volt = 0;
double PV_current = 0;
double PV_power = 0;
int counter = 0;
int loop_count = 0;
//int duty = 127;
int duty = 50;
//int duty = 4;
int duty_inc = 0;
double prev_power = 0;
double prev_volt = 0;
double prev_current = 0;

int mpp_reached = 0;
int unlock = 0;
int received = 0;
int rec_50 = 1;
int rec_mp = 0;
double avg_PV_volt = 0.0;
double avg_PV_curr = 0.0;
double avg_PV_pwer = 0.0;

int tmpcount = 0;


void setup() {
  //Declaring LED pin as output
//  pinMode(led_pin, OUTPUT);
  Serial.begin(9600);


  //initialize all timers except for 0, to save time keeping functions
  InitTimersSafe(); 
  //sets the frequency for the specified pin
  bool success = SetPinFrequencySafe(mosfet_pin, frequency);
  if(success) 
  {
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    pinMode(mosfet_pin,OUTPUT);
  }
}


void loop() 
{
  if (Serial.available() > 0) 
  {
    // read the incoming byte:
    received = Serial.read();

    //unlock main
    if (received == 44)
    {
      unlock = 1;
    }
  }
  
  if (unlock == 1)
  {
    if (duty > MAX_DUTY)
    {
      duty = MAX_DUTY;
    }
    if (duty < MIN_DUTY)
    {
      duty = MIN_DUTY;
    }
  
//    analogWrite(mosfet_pin, duty); //duty cycle 15%
//    duty = duty_calibrate();
    pwmWrite(mosfet_pin, duty_calibrate());
//    delayMicroseconds(40); // delay by 50 micro seconds (half the period)
    
    int slow = 20000;
    int mid = 10000;
    int fmid = 5000;
    int fast = 1000;
    int vfast = 100;
    if (loop_count >= fast)
    {
      measure_analog();  // MEASURED DATA FROM ANALOG PINS
      PV_volt  = double(PV_pos_val - PV_neg_val) * double(5.0/1023.0) * VOLT_DIV;
      
      ////////  I = I1 + I2 = (V_pv/R_total) + (V_pv * duty^2/R_2)  ////////
      PV_current = PV_volt/11000.0 + PV_volt*(duty/100.0)*(duty/100.0)/10.0;
  
      PV_power = PV_volt * PV_current;
      
      // rec_50
      if (rec_50 == 1)
      {
        duty = 50;
  
        if (40 <= counter && counter < 60)
        {
          avg_PV_volt += PV_volt;
          avg_PV_curr += PV_current;
          avg_PV_pwer += PV_power;
          tmpcount++;
        }
        else if (counter >= 60)
        {
          serial_comms((avg_PV_volt/tmpcount), (avg_PV_curr/tmpcount), (avg_PV_pwer/tmpcount), duty);
          avg_PV_volt = 0;
          avg_PV_curr = 0;
          avg_PV_pwer = 0;
          counter = 0;
          rec_50 = 0;
          rec_mp = 1;
          tmpcount = 0;
        }
      }
    
      // rec_mp
      if (rec_mp == 1)
      {
        duty = cal_duty(PV_volt, PV_current);
  
        if (40 <= mpp_reached && mpp_reached < 60)
        {
          avg_PV_volt += PV_volt;
          avg_PV_curr += PV_current;
          avg_PV_pwer += PV_power;
          tmpcount++;
        }
        else if (mpp_reached >= 60)
        {
          serial_comms((avg_PV_volt/tmpcount), (avg_PV_curr/tmpcount), (avg_PV_pwer/tmpcount), duty);
          avg_PV_volt = 0;
          avg_PV_curr = 0;
          avg_PV_pwer = 0;
          counter = 0;
          rec_50 = 1;
          rec_mp = 0;
          mpp_reached = 0;
          tmpcount = 0;
        }
      }
      counter++;
    }
    
    loop_count++;
  }
}


static int duty_calibrate()
{
  return (256 * float(duty/100.0));
}



static void serial_comms(double volt, double current, double power, int duty_cycle)
{
  //print voltage
  Serial.print("PV_vol: ");
  Serial.print(volt, 5);
  Serial.println("");

  //print current
  Serial.print("PV_cur: ");
  Serial.print(current, 5);
  Serial.println("");

  //print power
  Serial.print("PV_pow: ");
  Serial.print(power, 5);
  Serial.println("");

  //print duty
  Serial.print("PV_dut: ");
  Serial.print(duty_cycle);
  Serial.println("");

  if (rec_50 == 1)
  {
    Serial.print("REC_50:");
    Serial.println("");
  }
  if (rec_mp == 1)
  {
    Serial.print("REC_MP:");
    Serial.println("");
  }
}


static void measure_analog()
{
  double tmp_value = 0.0;
  double tmp_sum = 0.0;
  
  for (int ii=0; ii<20; ii++)
  {
    tmp_value = analogRead(PV_pos_pin);
    if (tmp_value > 0)
    {
      tmp_sum += tmp_value;
    }
    else
    {
      ii--;
    }
    delay(3);
  }
  PV_pos_val = int(tmp_sum/20.0);

  tmp_value = 0.0;
  tmp_sum = 0.0;
  for (int ii=0; ii<20; ii++)
  {
    tmp_value += analogRead(PV_neg_pin);
    if (tmp_value == 0.0) // 2nd times the charm?
    {
      tmp_value += analogRead(PV_neg_pin);
    }
    delay(3);
  }
  PV_neg_val = int(tmp_value/20.0);
}


static int cal_duty(double PV_volt, double PV_current)
{
  double PV_power = PV_volt * PV_current;
  double delta_power = PV_power - prev_power;
  double delta_volt = PV_volt - prev_volt;
  int d;
  double delta_threshold = 0.02;
  
  if (delta_power > 0)
  {
    //check delta volt
    if (delta_volt > delta_threshold)
    {
      d = duty - duty_inc;
    }
    else if (delta_volt < -delta_threshold)
    {
      d = duty + duty_inc;
    }
    else
    {
      //mpp reached
      mpp_reached++;
    }
  }
  else if (delta_power < 0)
  {
    if (delta_volt < -delta_threshold)
    {
      d = duty + duty_inc;
    }
    else if(delta_volt > delta_threshold)
    {
      d = duty - duty_inc;
    }
    else
    {
      //mpp reached
      mpp_reached++;
    }
  }
//  else
//  {
//    //mpp reached
//    mpp_reached++;
//  }

// set previous values with new values
  prev_power = PV_power;
  prev_volt = PV_volt;
  prev_current = PV_current;

  if (d > MAX_DUTY)
  {
    return MAX_DUTY;
  }
  if (d < MIN_DUTY)
  {
    return MIN_DUTY;
  }
  
  return d;
}

// End of File
