/*
 * This will be the main code to use machien learning MPPT
 *
 */

#include <PWM.h>

#define VOLT_DIV 11
#define MAX_DUTY 92
#define MIN_DUTY 8

//Initializing LED Pin
int mosfet_pin = 5;
int32_t frequency = 10000;  //frequency (in Hz)

int R1 = 75;
int R2 = 10;

// int PV_curr_pin = A3;
// int PV_curr_val = 0;
int PV_pos_pin = A1;
// int PV_neg_pin = A0;
int PV_pos_val = 0;
int PV_neg_val = 0;

double OC_volt = 0;
double PV_volt = 0;
double PV_current = 0;
uint8_t counter = 0;
//double duty = 50;
double duty = 15;
//double duty_inc = 0.5;
double duty_inc = 1;
double prev_power = 0;
double prev_volt = 0;
double prev_current = 0;

bool send = false;

void setup()
{
  //Declaring LED pin as output
  Serial.begin(9600);



  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);


  ////////  THIS GENERATES 10KHz PWM at PIN5 //////////
  TCCR0A = B10000011;
  TCCR0B = TCCR0B & B11111000 | B10001010;
  OCR0A = 200;

  pinMode(mosfet_pin, OUTPUT);

  sei();//allow interrupts
}

void loop() {
  double dummy_read = 0;
//   Serial.println("Starting loop");
  //LIMIT THE DUTY CYCLE
  if (duty > MAX_DUTY)
  {
    duty = MAX_DUTY-2;
  }
  if (duty < MIN_DUTY)
  {
    duty = MIN_DUTY+2;
  }

  //measure and send OC volt
  if (send == true)
  {
    send = false;
    //measure OC volt
    analogWrite(mosfet_pin, 0);
    double tmp_sum = 0.0;
    for (int ii=0; ii<20; ii++)
    {
      delayMicroseconds(100);
      tmp_sum += analogRead(PV_pos_pin);
    }
    PV_pos_val = int(tmp_sum/20.0);
    //calculate OC volt
    OC_volt = double(PV_pos_val) * 0.0049 * VOLT_DIV;
    //send OC volt
    Serial.print("OC_vol: ");
    Serial.print(OC_volt);
    Serial.println(" ");
  }

  // if ((counter % 20) == 0)
  // {
  //**** SEND DATA TO PC ****//

  if (Serial.available() > 0)
  {
    delay(50);
    send = false;
    dummy_read = Serial.parseFloat();
    Serial.print("dummy_read: ");
    Serial.print(dummy_read);
    if (dummy_read > 0)
    {
      duty = dummy_read;
      Serial.print("   duty_read: ");
      Serial.print(duty, 5);
      Serial.println("");
    }
  }


    //**** USE THIS TO FIND MPPT ****//
//    duty = cal_duty(PV_volt, PV_current);
//    duty = var_cal_duty(PV_volt, PV_current);

    //**** USE THIS TO DO A SWEEP  ****//
//     duty += 1; // (might need to config other variables)
//     if (duty > MAX_DUTY-5)
//       duty = MIN_DUTY;

    //**** USE THIS TO FOR CONSTANT VOLT ****//
//    duty = 50;
  // }
  
  //output PWM with duty
  analogWrite(mosfet_pin, duty_calibrate());

  counter++;
//   if (counter > 250)
//   {
//     counter = 0;
//   }
//   if ((counter % 5) == 0)
//   {
//     double tmp_value = 0.0;
//     double tmp_sum = 0.0;
//
//     for (int ii=0; ii<20; ii++)
//     {
//       delayMicroseconds(100);
//       tmp_sum += analogRead(PV_pos_pin);
//
//       delayMicroseconds(100);
//     }
//     PV_pos_val = int(tmp_sum/20.0);
//
// //    PV_volt  = double(PV_pos_val - PV_neg_val) * double(5.0/1023.0) * VOLT_DIV;
//     PV_volt = double(PV_pos_val) * 0.0049 * VOLT_DIV;
// //    Serial.print("   PV_volt: ");
// //    Serial.println(PV_volt);
//     ////////  I = I1 + I2 = (V_pv/R_total) + (V_pv * duty^2/R_2)  ////////
//     PV_current = PV_volt/11000.0 + PV_volt*(duty/100.0)*(duty/100.0)/10.0;
//
// //    serial_comms(PV_volt, PV_current, duty);
//   }
}


//static double duty_calibrate()
//{
//  return double(256.0 * float(duty/100.0));
//}

static int duty_calibrate()
{
  return (duty * 2.0);
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
  
  if (loc_PV_power > 7) // invalid data
  {
    return duty;
  }
  else if (loc_PV_power == 0)
  {
    return duty = 50;
  }
  
  double delta_power = loc_PV_power - prev_power;
  double delta_volt = loc_PV_volt - prev_volt;
  double new_inc = 4;
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


/******************* VARIABLE POWER *******************/
/********************** FUNCTION **********************/
static double var_cal_duty(double loc_PV_volt, double loc_PV_current)
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
//     new_inc = duty_inc * max_power / loc_PV_power * 0.60;
    new_inc = duty_inc * max_power / loc_PV_power;
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


volatile uint16_t int_counter = 0;

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5Hz (takes two cycles for full wave- toggle high then toggle low)

  //every 3sec?
  if (int_counter > 3)
  {
    send = true;
    int_counter = 0;
  }
  else
  {
    send = false;
    int_counter++;
    // Serial.print("int_counter: ");
    // Serial.print(int_counter);
    // Serial.println("");
  }
}


// End of File
