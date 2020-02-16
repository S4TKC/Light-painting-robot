unsigned long IRData = 0;
unsigned long prvIRData = 1;

int maxEncodableSpeed = 100;

boolean debugFlag = false;

void setup()
{
  pinSetup();
}

void loop()
{
  prepareBlink();
  circle(8, 5, 100);
  delay(500);
  circle(5, 8, 200);
  delay(500);
  circle(8, 5, 100);
  straight(200);
  black();
  delay(3000);
}


void setIRData(int received_IRData)
{
  IRData = received_IRData;
}

void straight(int len)
{
  resetCount();
  int count1 = 0;
  int prvCount1 = -1;
  int count2 = 0;
  int prvCount2 = -1;
  while (count1 < len && count2 < len)
  {
    count1 = getCount1();
    count2 = getCount2();
    if (count1 != prvCount1 || count2 != prvCount2)
    {
      if (debugFlag)
      {
        Serial.print(count1);
        Serial.print(" ");
        Serial.println(count2);
      }
      prvCount1 = count1;
      prvCount2 = count2;
    }
    int spd1 = count1 <= count2 ? maxEncodableSpeed : 0;
    int spd2 = count2 <= count1 ? maxEncodableSpeed : 0;
    motor1(spd1);
    motor2(spd2);
    HSVSidewaysCycle(count1, len);
  }
  motor1(0);
  motor2(0);
}

void circle(int targetCounts1, int targetCounts2)
{
  unsigned int progress = 0;
  int count1 = 0;
  int count2 = 0;
  int prvCount1 = -1;
  int prvCount2 = -1;

  while(count1 < targetCounts1 || count2 < targetCount2)
  {
    
  }
}

void remoteControl()
{
  updateIRResults();
  if (IRData != prvIRData)
  {
    switch (IRData)
    {
      case 16718055: //2
        Serial.println("2");
        motor1(255);
        motor2(255);
        delay(1000);
        motor1(0);
        motor2(0);
        break;
      case 16734885: //6
        Serial.println("6");
        motor1(255);
        motor2(-255);
        delay(250);
        motor1(0);
        motor2(0);
        break;
      case 16730805: //8
        Serial.println("8");
        motor1(-255);
        motor2(-255);
        delay(1000);
        motor1(0);
        motor2(0);
        break;
      case 16716015: //4
        Serial.println("4");
        motor1(-255);
        motor2(255);
        delay(250);
        motor1(0);
        motor2(0);
        break;
    }
  }
  prvIRData = IRData;
}
