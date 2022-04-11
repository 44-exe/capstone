#define VOLT_DIV 11
#define MAX_DUTY 93
#define MIN_DUTY 0

//Initializing LED Pin
int mosfet_pin = 5;
int32_t frequency = 10000;  //frequency (in Hz)

int R1 = 75;
int R2 = 10;

int counter = 1;
int PV_pos_pin = A1;
int PV_pos_val = 0;

double PV_volt = 0;
double PV_current = 0;
double duty = 0;
double duty_inc = 1;
double prev_power = 0;
double prev_volt = 0;
double prev_current = 0;

void setup() {
  Serial.begin(9600);


//  TCCR0B = TCCR0B & B11111000 | B00000001; // PWM 31372.55 Hz pins 5 and 6
    TCCR0B = TCCR0B & B11111000 | B00000010; // PWM 7000 Hz pins 5 and 6
//  TCCR2B = TCCR2B & B11111000 | B00000001; // PWM 31372.55 Hz pins 3 and 11
//  TCCR1B = TCCR1B & B11111000 | B00000001; // PWM 62745.10 Hz pins 9 and 10

  pinMode(mosfet_pin, OUTPUT);
  delay(1000);
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


  if ((counter % 40) == 0)
  {
    //**** SEND DATA TO PC ****//
    serial_comms(PV_volt, PV_current, duty);
    
    //**** USE THIS TO FIND MPPT ****//
//    duty = cal_duty(PV_volt, PV_current);

    //**** USE THIS TO DO A SWEEP  ****//
    duty += 1; // (might need to config other variables)
    if (duty > MAX_DUTY-3)
    {
      Serial.println("end of sweep");
      delay(600000);  // delay 60sec
      duty = MIN_DUTY;
    }

    //**** USE THIS TO FOR CONSTANT VOLT ****//
//    duty = 96;
  }
  
  analogWrite(mosfet_pin, duty_calibrate()); //duty cycle 15%
  TCCR0B = TCCR0B & B11111000 | B00000010; // PWM 31372.55 Hz pins 5 and 6

  counter++;
  if ((counter % 5) == 0)
  {
    double tmp_value = 0.0;
    double tmp_sum = 0.0;

    for (int ii=0; ii<20; ii++)
    {
      delayMicroseconds(100);
      tmp_sum += analogRead(PV_pos_pin);
      delayMicroseconds(100);
    }
    PV_pos_val = int(tmp_sum/20.0);
    
    PV_volt = double(PV_pos_val) * 0.0049 * VOLT_DIV;
    PV_current = PV_volt/11000.0 + PV_volt*(duty/100.0)*(duty/100.0)/10.0;
  }
}


//static double duty_calibrate()
//{
//  return double(256.0 * float(duty/100.0));
//}

static int duty_calibrate()
{
  return (duty * 2.55);
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
uint16_t count_max = 0;
static double cal_duty(double loc_PV_volt, double loc_PV_current)
{ 
  double loc_PV_power = loc_PV_volt * loc_PV_current;
  
  if (loc_PV_power > 10) // invalid data
  {
    return duty;
  }
  else if (loc_PV_power == 0)
  {
    return duty = 50;
  }
  
  double delta_power = loc_PV_power - prev_power;
  double delta_volt = loc_PV_volt - prev_volt;
  double new_inc = duty_inc;
  double d;
//  double delta_volt_threshold = 0.001;
  double delta_power_threshold = 0.001;

  if (delta_power > delta_power_threshold)
  {
    //check delta volt
    if (delta_volt > 0)
    {
      d = duty + new_inc;
    }
    else if (delta_volt < 0)
    {
      d = duty - new_inc;
    }
    else
    {
      d = duty;
    }
  }
  else if (delta_power < -delta_power_threshold)
  {
    if (delta_volt > 0)
    {
      d = duty - new_inc;
    }
    else if (delta_volt < 0)
    {
      d = duty + new_inc;
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
