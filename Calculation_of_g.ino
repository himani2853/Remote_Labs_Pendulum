#define BLYNK_TEMPLATE_ID           "TMPL3Pm2rInpo"
#define BLYNK_TEMPLATE_NAME         "Servocheck"
#define BLYNK_AUTH_TOKEN            "GxaHOjnLK4G2liYeK_Hw1aow_8XAj_ox"

#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

char ssid[] = "Mt.";
char pass[] = "vava@2026";

Servo servo1;
Servo servo2;
unsigned long startTime = 0;
const int irPin = D1;
bool isIrOn = false;
unsigned long pretime;
unsigned long currtime;
unsigned long timePeriod;
boolean flag = true;
int check = 0;
float g;
float l;
float t_avg;
float g_avg = 0;
unsigned int count = 0;
float g_total = 0;
unsigned int g_count = 0;


BLYNK_WRITE(V0)
{  
  int buttonState = param.asInt();
  if (buttonState == 1) {
    startTime = millis();
    Serial.println("Rotate on");
    while(millis() - startTime < 5000)
    {
      servo1.write(199);
      delay(1500);
      servo1.write(0);
      delay(1500);
    }
    servo1.write(199);
    Serial.println("Oscillations done!");    
  } 
  else {
    // Turn the servo off
    servo1.write(0);
    Serial.println("Rotate off");
    
  }
}

BLYNK_WRITE(V1)
{
  isIrOn = param.asInt();

  Serial.println("Changing");
  Serial.println(check);

  if(isIrOn)
  {
    // if (check == 0)
    // {
      servo2.write(0);
      check = 0;
    // }

    // else
    // {
    //   servo2.write(199);
    //   check = 0;
    // }
  }
  else
  {
    servo2.write(199);
    check = 1;
  }
}

void setup()
{


  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  servo1.attach(2);  // Attach servo to GPIO pin D4
  servo1.write(0);   // Rotate servo to 0 degrees initially

  servo2.attach(0);  // Attach servo to GPIO pin D4
  servo2.write(0);

  delay(2000);      // Wait for 2 seconds after setup
  startTime = millis(); // Record the start time
  Serial.begin(9600);
  pinMode(irPin, INPUT);
  pretime = micros();
}

void loop()
{
  Blynk.run();
   if (digitalRead(irPin) == LOW) { // Changed to LOW for active low sensor
    if (flag == true) {
      currtime = millis();
      timePeriod = currtime - pretime;
      pretime = currtime;
      flag = !flag;

      double period_sq = timePeriod * timePeriod;

      if (check == 0)
      {
        l = 24.2;        
      }
      else
      {
        l = 25.8;
      }

      Blynk.virtualWrite(V4,l);

      g = ((4 * 3.141 * 3.141 * l) / period_sq) * 10000;
      Serial.println(g);
      Blynk.virtualWrite(V2,g);

      // new block
      count++;
      if(count>=10)
      {
        g_count++;
        g_total = g_total + g;
        if(g_count == 5)
        {
          // Serial.print("g_total_sum: ");
          // Serial.println(g_total);
          g_avg = g_total / 5;

          t_avg = 2*3.14*sqrt(l/g_avg);

          if(g_avg > 7 && g_avg <12)
          {
            Serial.print("g_total_avg: ");
            Serial.println(g_avg);
            Blynk.virtualWrite(V3, g_avg);
            // Blynk.virtualWrite(V4, t_avg);
          }
          
          g_count = 0;
          g_total = 0;
        }
      }
      
      delay(100);
    } else {
      flag = !flag;
      delay(100);
    }
  }
}
