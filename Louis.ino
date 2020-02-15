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

void circle(int counts1, int counts2, int totalOutsideCounts)
{
  int count1 = 0;
  int prvCount1 = -1;
  int count2 = 0;
  int prvCount2 = -1;
  int fullCycles = (int) totalOutsideCounts / max(counts1, counts2);
  int totalCount1 = 0;
  int totalCount2 = 0;
  if(debugFlag)
  {
    Serial.println("Full Cycles: " + String(fullCycles));
  }
  for(int i = 0; i < fullCycles; i++)
  {
    if(debugFlag)
    {
      Serial.println("Cycle number: " + String(i));
    }
    resetCount();
    count1 = getCount1();
    count2 = getCount2();
    totalCount1 += count1 - prvCount1;
    totalCount2 += count2 - prvCount2;
    
    while (count1 < counts1 || count2 < counts2)
    {
      count1 = getCount1();
      count2 = getCount2();
      totalCount1 += count1 - prvCount1;
      totalCount2 += count2 - prvCount2;
      HSVCycle(min(totalCount1, totalCount2), totalOutsideCounts);
      if(count1 != prvCount1 || count2 != prvCount2)
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
      int spd1 = count1 < counts1 ? maxEncodableSpeed : 0;
      int spd2 = count2 < counts2 ? maxEncodableSpeed : 0;
      motor1(spd1);
      motor2(spd2);
    }
  }
  int countsleft1 = max(counts1, counts2) == counts1 ? totalOutsideCounts - fullCycles * counts1 : (int) (totalOutsideCounts - fullCycles * counts2) * (counts1 / counts2);
  int countsleft2 = max(counts1, counts2) == counts2 ? totalOutsideCounts - fullCycles * counts2 : (int) (totalOutsideCounts - fullCycles * counts1) * (counts2 / counts1);
  resetCount();
  while (count1 < countsleft1 || count2 < countsleft2)
  {
    count1 = getCount1();
    count2 = getCount2();
    totalCount1 += count1 - prvCount1;
    totalCount2 += count2 - prvCount2;
    HSVCycle(min(totalCount1, totalCount2), totalOutsideCounts);
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
    int spd1 = count1 < counts1 ? maxEncodableSpeed : 0;
    int spd2 = count2 < counts2 ? maxEncodableSpeed : 0;
    motor1(spd1);
    motor2(spd2);
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
