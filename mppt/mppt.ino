#define VOLT_DIV 11

//Initializing LED Pin
int mosfet_pin = 3;
int PV_pos_pin = A2;
int PV_neg_pin = A0;
int PV_pos_val = 0;
int PV_neg_val = 0;
double PV_volt = 0;
double PV_current = 0;
int counter = 0;
int duty = 127;
int duty_inc = 2;
double prev_power = 0;
double prev_volt = 0;
double prev_current = 0;

void setup() {
  //Declaring LED pin as output
//  pinMode(led_pin, OUTPU/T);
  Serial.begin(9600);
}

void loop() {
  //Fading the LED
//  for(int i=0; i<255; i++){
//    analogWrite(led_pin, i);
//    delay(5);
//  }
//  for(int i=255; i>0; i--){
//    analogWrite(led_pin, i);
//    delay(5);
//  }

// PWM for MOSFET
  for(int i=0; i<200; i++){
    analogWrite(mosfet_pin, duty);
//    analogWrite(mosfet_pin, 3);
    delay(3);
  }
  
  counter++;
  if (counter >= 1)
  {
    double tmp_value = 0.0;
    counter = 0;
    for (int ii=0; ii<20; ii++)
    {
      tmp_value += analogRead(PV_pos_pin);
      delay(3);
    }
    PV_pos_val = int(tmp_value/20.0);
    Serial.print("PV_pos_val: ");
    Serial.print(double(PV_pos_val) * double(5.0/1023.0));
    delay(20);

    PV_neg_val = analogRead(PV_neg_pin);
    Serial.print("   PV_neg_val: ");
    Serial.println(double(PV_neg_val) * double(5.0/1023.0));
    
    PV_volt  = double(PV_pos_val - PV_neg_val) * double(5.0/1023.0) * VOLT_DIV;
    Serial.print("PV_vol: ");
    Serial.print(PV_volt);
    Serial.println("");
    PV_current = PV_volt/11000.0 + PV_volt*(duty/255.0)*(duty/255.0)/10.0;
    Serial.print("PV_cur: ");
    Serial.print(PV_current);
    Serial.println("");
    
    duty = cal_duty(PV_volt, PV_current);
    Serial.print("Duty: ");
    Serial.println(duty);
    Serial.println("");
  }

  
  
}


static int cal_duty(double PV_volt, double PV_current)
{
  double PV_power = PV_volt * PV_current;
  double delta_power = PV_power - prev_power;
  Serial.print("PV_pow: ");
  Serial.print(PV_power);
  Serial.println("");
  Serial.print("delta_power: ");
  Serial.print(delta_power);
  Serial.println("W");

  double delta_volt = PV_volt - prev_volt;
  Serial.print("delta_volt: ");
  Serial.print(delta_volt);
  Serial.println("V");
  int d;

  if (delta_power > 0)
  {
    //check delta volt
    if (delta_volt > 0)
    {
      d = duty - duty_inc;
    }
    else
    {
      d = duty + duty_inc;
    }
  }
  else
  {
    if (delta_volt < 0)
    {
      d = duty + duty_inc;
    }
    else
    {
      d = duty - duty_inc;
    }
  }

// set previous values with new values
  prev_power = PV_power;
  prev_volt = PV_volt;
  prev_current = PV_current;

  
  if (d > 253)
  {
    return 253;
  }
  if (d < 3)
  {
    return 3;
  }
  
  return d;
}
