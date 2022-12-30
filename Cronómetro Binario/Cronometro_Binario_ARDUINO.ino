// C++ code
//
void setup()
{
    pinMode(2, INPUT);    //start button
    pinMode(3, INPUT);    //reset button
    pinMode(4, OUTPUT);   //first led
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);  //last led
    Serial.begin(9600);
}



bool isStartButtonStillPushed = false;
bool isResetButtonStillPushed = false;
bool justResetted = false;
bool programIsRunning = false;
char binaryTimer[11] = {"0000000000"};
int decimalTimer = -1;
int cycle[10] = {1,2,4,8,16,32,64,128,256,512};
int status[10] = {0,0,0,0,0,0,0,0,0,0};
unsigned int interval = 1000;
unsigned int lastMillis = 0;



void loop()
{
  unsigned int currentMillis = millis();
  int startButtonStatus = digitalRead(2);
  int resetButtonStatus = digitalRead(3);
  
  if(startButtonStatus && !isStartButtonStillPushed){
    pushButton("start", true);  //msg display = true
  }
  
  if(resetButtonStatus && !isResetButtonStillPushed){
    pushButton("reset", true);  //msg display = true
  }
  
  if((currentMillis - lastMillis > interval) || justResetted)
  {
    lastMillis = currentMillis;
    
    if(programIsRunning)
    {
      justResetted = false;
      executeChronometer();
      updateBinaryTimer();
      printConsoleMsg();
      checkMaxNumber();
    }
  }

  isStartButtonStillPushed = startButtonStatus;
  isResetButtonStillPushed = resetButtonStatus;
  delay(20);
}





void executeChronometer()
{
  for(int i=0;i<=9;i++){
    if(cycle[i]<1){
      status[i] = !status[i];
      resetCycle(i);
      digitalWrite(i+4, status[i]);
    }
    cycle[i]--;
  }
  decimalTimer++;
}



void resetCycle(int i)
{
  float buffer = pow(2, i);
  int resettedCycle = round(buffer);
  cycle[i] = resettedCycle;
}



void pushButton(char* type, bool displayMessage)
{
  if(type == "start")
  {
    programIsRunning = 1 - programIsRunning;
    if(!programIsRunning && displayMessage){
      printButtonMsg("- Paused. -");
    }
  }
  else if(type == "reset")
  {
    for(int i=0;i<=9;i++){
      resetCycle(i);
      status[i] = 0;
      digitalWrite(i+4, status[i]);
    }
    if(displayMessage && !justResetted){
      printButtonMsg(" Resetted. ");
    }
    decimalTimer = -1;
    justResetted = true;
  }
}



void updateBinaryTimer()
{
    for (int i=0, j = 9; i<=9; i++, j--){
      char buffer;
      if(status[i]){
        buffer = 49;
      }else{
        buffer = 48;
      }
      binaryTimer[j] = buffer;
    }
}



void printConsoleMsg()
{
    Serial.print("Seconds: ");
    Serial.print(decimalTimer);
    Serial.print(" | Binary: ");
    Serial.print(binaryTimer);
    Serial.print("\n");
}



void checkMaxNumber()
{
    if(decimalTimer>1022){
      pushButton("reset", false);
      programIsRunning = false;
    }
}



void printButtonMsg(char* core)
{
  char* line0 = "----------";
  char* line1 = "-----------";
  char* line2 = "------------";
  char* lineA;
  char* lineB;
  
  if(decimalTimer>99 && decimalTimer<1000){
    lineA = line1;
    lineB = line1;
  }else if(decimalTimer<10){
    lineA = line0;
    lineB = line0;
  }else if(decimalTimer>9 && decimalTimer<100){
    lineA = line0;
    lineB = line1;
  }else{          //decimalTimer>1000
    lineA = line1;
    lineB = line2;
  }
  
  Serial.print(lineA);    //prints left '-'s
  Serial.print(core);   //prints core msg
  Serial.print(lineB);    //prints right '-'s
  Serial.print("\n");
}
