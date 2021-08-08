/*-------------------------------------------------------------------------------
Course		: CMP3006 Embedded Systems Programming
Purpose   	: Spring 2021 Term Project - Digital Clock

Created   	: 2021 Jun 1
Modified   	: 2021 Jun 14
Author(s)	: Ercument Burak Tokman (1315490)
Reference	:
Licence   	: Unauthorized copying of this file, via any medium is strictly prohibited
			  proprietary and confidential
-------------------------------------------------------------------------------*/

#include <LiquidCrystal.h>;

// CONFIG
const int rs=12, en=11, d5=6, d6=5, d7=4, d8=3;
LiquidCrystal lcd(rs, en, d5, d6, d7, d8);

const int btnPin1 = 10;				// B1
const int btnPin2 = 9;				// B2
const int btnPin3 = 8;				// B3
const int btnPin4 = 7;				// B4
const int piezoPin = 2;				// PIEZO BUZZER
const int tempPin = 0;				// TEMPERATURE



// ---------------------- SETUP ----------------------
// ALARM ICON
//byte Bell[8] = {0b00100,0b01110,0b01110,0b01110,0b11111,0b00000,0b00100,0b00000};
uint8_t bell[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};

unsigned long timeSnoozed;		// TIME POSTPONED

int flag24=0, flagTime=0, flagAlarm=1, flagTemp=0, flagBlink=0;//0=12hr,1=24hr, 0=AM,1=PM | 0=OFF,1=ON | 0=C,1=F
int hour=11, minute=59, second=55;
int alarmHour=12, alarmMinute=00;
bool postponed = false;
int screenMode = 0;				// 0=time, 1=setTime, 2=setAlarm

int celsius = 0;				// CELSIUS
int fahrenheit = 0;				// FAHRENHEIT

int btnState1 = 0;				// BUTTON PRESSED OR NOT
int btnState2 = 0;
int btnState3 = 0;
int btnState4 = 0;


void setup(){
  Serial.begin(9600);
  lcd.begin(16, 2);				// LCD
  lcd.createChar(0, bell);    	// LOAD BELL SYMBOL TO LCD
  
  pinMode(btnPin1, INPUT);		// BUTTON 1		(input)
  pinMode(btnPin2, INPUT);		// BUTTON 2		(input)
  pinMode(btnPin3, INPUT);		// BUTTON 3		(input)
  pinMode(btnPin4, INPUT);		// BUTTON 4		(input)
  pinMode(tempPin, INPUT);		// TEMPERATURE	(input)
  cli();               			// DISABLE INTERRUPTS
  
  // Timer HZ = ArduinoClock(16MHz) / Prescaler
  
  // TIMER 1					(interrupt evey 500ms) 
  TCCR1A = 0;					// RESET REGISTER
  TCCR1B = 0;
  TCNT1 = 3035 ;      			// PRELOAD TIMER
  TCCR1B |= (1 << CS12);  		// PRESCALER=256  (0.5 sec)     (1 << CS10)| 
  TIMSK1 |= B00000010;        	// ENABLE COMPARE MATCH A - SET OCIE1A to 1
  OCR1A = 62500;				// SET TOP COUNT - //31250
  sei();                 		// ENABLE INTERRUPTS
}



// ---------------------- TIMER1 ISR -----------------

ISR(TIMER1_COMPA_vect){         // OVERFLOW
  TCNT1 = 0;                  	// RESET TIMER
  //timeSinceInit += 1;
  /*Serial.print(timeSinceInit);
  Serial.print("  ");
  Serial.println(TCNT1);*/
  updateClock();				// UPDATE CLOCK
}



// ---------------------- START ----------------------

void loop(){
  if (screenMode == 0)			clock();
  else if(screenMode == 1)		setTime();
  else if(screenMode == 2)		setAlarm();
}



// ------------------- UPDATE CLOCK ------------------

void updateClock(){
  second += 1;						// ADD +1 SECOND
  if (second == 60){
    second = 0;
    minute += 1;
  }
  if (minute == 60){				// ADD +1 MINUTE
    minute = 0;
    hour += 1;
  }
  if (hour >= 24) hour = 0; 		// RESET HOUR
  if (hour >= 12) flagTime = 1;		// FLAG PM				
  if (hour < 12)  flagTime = 0;		// FLAG AM
  Serial.println(String(hour) + ":" + String(minute) + ":" + String(second));
}



// ------------------- CHECK BUTTON ------------------


unsigned long timePressed;
int btnPrevState1, btnPrevState2;
bool checkPress1, checkPress2;

void checkButtons(){
  
  btnState1 = digitalRead(btnPin1);			// READ BUTTONS
  btnState2 = digitalRead(btnPin2);			
  btnState3 = digitalRead(btnPin3);
  btnState4 = digitalRead(btnPin4);
  
  // CHECK IF B1 HOLD FOR 3 SEC.
  if (btnState1 != btnPrevState1){
    if(btnPrevState1 == 0){
      timePressed = millis();				// PRESS START TIME
      checkPress1 = true;					// B1 PRESS ACTIVE
      Serial.println("HOLD BUTTON: B1 pressed started");
    }
    else checkPress1 = false;				// B1 RELEASED BEFORE 3 sec.
    btnPrevState1 = btnState1;				// SAVE
  }
  
  if (checkPress1
      && (millis() - timePressed) >= 3000){ // CHECK IF 3 sec. PASSED
    screenMode = 1;							// GO TO SET TIME SCREEN
    checkPress1 = false;
    Serial.println("HOLD BUTTON: Go to set time");
  }

  // CHECK IF B2 HOLD FOR 3 SEC.
  if (btnState2 != btnPrevState2){
    if(btnPrevState2 == 0){
      timePressed = millis();				// PRESS START TIME
      checkPress2 = true;					// B2 PRESS ACTIVE
      Serial.println("HOLD BUTTON: B2 pressed started");
    }
    else checkPress2 = false;				// B2 RELEASED BEFORE 3 sec.
    btnPrevState2 = btnState2;				// SAVE
  }
  
  if (checkPress2
      && (millis() - timePressed) >= 3000){ // CHECK IF 3 sec. PASSED
    screenMode = 2;							// GO TO SET ALARM SCREEN
    checkPress2 = false;
    Serial.println("HOLD BUTTON: Go to set alarm");
  }
  
  
  if (btnState4){							// CHANGE TIME FORMAT (button 4)
    Serial.println("CHECK BUTTON: change time format");
    flag24 = !flag24;
    lcd.clear();
  }
  else if (btnState3){						// CHANGE TEMP. FORMAT (button 3)
    Serial.println("CHECK BUTTON: change temperature mode");
    flagTemp = !flagTemp;
  }
   //else	screenMode = 0;					// SHOW CLOCK
   
}



// ---------------------- CLOCK ----------------------

void clock(){
  lcd.setCursor(0,0);	  			// LINE 1 (time)
  lcd.print("    ");
  getClock(); 						// 15:52:12 PM

  lcd.setCursor(0,1);				// LINE 2 (temp + alarm)
  getTemperature();					// 25*C
  lcd.print("   ");
  getAlarm();						// 05:30 ON
  
  if (flagAlarm) checkAlarm();		// CHECK ALARM IF ON
  checkButtons();					// CHECK BUTTON
  
  delay(200);
}



void getClock(){					// PRINT CLOCK LCD - 15:52:12 PM
  String h = String(hour);
  
  if (flag24 == 0 && hour == 12) h = "12";	// 12:00 PM
  else if (flag24 == 0 && hour > 12)
    h = String(hour % 12); // AM/PM MOD
  
  String m = String(minute);
  String s = String(second);
  
  if ((flag24==0 && hour%12<10 && h!="12")
      || (flag24==1 && hour<10)){ 	// 0 PADDING			
    lcd.print(0);
  }
  lcd.print(h + ":");				// PRINT hour
  if (minute < 10)	lcd.print(0);	// 0 PADDING
  lcd.print(m + ":");				// PRINT minute
  if (second < 10)	lcd.print(0);	// 0 PADDING	
  lcd.print(s);						// PRINT second
  
  
  if (flag24 == 0){					// PRINT AM/PM
    if (flagTime == 0) lcd.print(" AM");
    if (flagTime == 1) lcd.print(" PM");
  } //else clock += "   ";			// CLEAR LCD		
}



void getAlarm(){					// PRINT ALARM LCD - 05:30 PM
  String h = String(alarmHour);
  String m = String(alarmMinute);
  
  if (flag24 == 0 && alarmHour == 12) h = "12";		// 12:00 PM
  else if (flag24 == 0 && alarmHour > 12)			// AM/PM MOD
    h = String(alarmHour % 12); 
  
  //String clock = String(h + ":" + m);	
    
  if ((flag24==0 && alarmHour%12<10 && h!="12")
      || (flag24==1 && alarmHour<10)){ 				// 0 PADDING		
    lcd.print(0);
  }
  lcd.print(h + ":");								// PRINT hour
  if (alarmMinute < 10) lcd.print(0);				// 0 PADDING
  lcd.print(m + " ");								// PRINT minute
  
  if (flagAlarm == 0)	lcd.print("OFF");
  if (flagAlarm == 1)	lcd.print("ON "); // lcd.print(char(0));
  
  /*lcd.createChar(0, Bell);// ALARM ICON
  lcd.setCursor(1,1);
  lcd.write((uint8_t)0);
  lcd.write(0);*/
}



// ---------------------- CHECK ALARM ----------------

void checkAlarm(){
  
  if ((alarmHour == hour && alarmMinute == minute && !postponed) // TIME MATCH or POSTPONED 5min
      || ((millis() - timeSnoozed) / 1000) >= 5*60 && postponed){
    
    lcd.setCursor(7,1);
    lcd.print("ALARM!  ");						// ALARM ON LCD
    Serial.println("ALARM!");
    tone(piezoPin, 1000);    					// MAKE BUZZ 
     //analogWrite(piezoPin, 255);
    
    while(true){
      btnState1 = digitalRead(btnPin1);			// READ BUTTONS
      btnState2 = digitalRead(btnPin2);
      btnState3 = digitalRead(btnPin3);
      btnState4 = digitalRead(btnPin4);
      
      if (btnState1 || btnState2 || btnState3){ // STOP ALARM	  (Button 1,2,3)
        noTone(piezoPin);						// STOP BUZZ
        flagAlarm = 0;
        postponed = false;
        Serial.println("ALARM STOPPED");
        break;
      }
      if (btnState4){							// SNOOZE 5 MINS. (Button 4)
        noTone(piezoPin);						// STOP BUZZ
        timeSnoozed = millis();					// REMEMBER POSTPONE TIME
        postponed = true;
        Serial.println("POSTPONED!");
        break;
      }

    }
  }
}


// ---------------------- SET TIME -------------------

unsigned long setTimeStart;

void setTime(){
  int lcdX = 0;								// LCD INDEX X
  //int lcdY = 1;							// LCD INDEX Y
  int blinkVar = hour;						// STORE FOR BLINKING
  int switchIndex = 0;						// SWITCH INDEX = 0=hour, 1=minute
  lcd.clear();								// CLEAR SCREEN
  setTimeStart = millis();					// START
  
  while(true){
    lcd.setCursor(0,0);						// LCD LINE 1 - SET TIME
  	lcd.print("SET TIME:");
  	
    lcd.setCursor(0,1);						// LCD LINE 2	11:05
	if (hour < 10)		lcd.print("0");
	lcd.print(hour);						// PRINT HOUR
   	lcd.print(":");
	if (minute < 10)	lcd.print("0");
	lcd.print(minute);						// PRINT MINUTE
    
    btnState1 = digitalRead(btnPin1);		// READ BUTTON
  	btnState2 = digitalRead(btnPin2);
  	btnState3 = digitalRead(btnPin3);
    	
    if (btnState2 && switchIndex == 1){		// SWITCH to HOUR	(Button 2)
      Serial.println("SET TIME: switched to HOUR");
      switchIndex = 0;
      lcdX = 0;
    }
    else if (btnState2 && switchIndex == 0){// SWITCH to MINUTE (Button 2)
      Serial.println("SET TIME: switched to MINUTE");
      switchIndex = 1; 
      lcdX = 3;
    }
    if (btnState3 && switchIndex == 0){		// INCREASE HOUR 	(Button 3)
      if(hour == 24)	hour = 0;
      hour += 1;	
    }
    if (btnState3 && switchIndex == 1){		// INCREASE MINUTE 	(Button 3)
      if(minute == 60)	minute = 0;
      minute += 1;
    }
    if (btnState1 &&						// EXIT SETUP		(Button 1 & after 2sec)
        (millis() - setTimeStart) >= 2000){
      screenMode = 0;
      break;
    }	
    
    // BLINK
    if (switchIndex == 0)	blinkVar = hour;
    if (switchIndex == 1)	blinkVar = minute;
    blink(blinkVar, lcdX, 1);
  }
}



// ---------------------- SET ALARM ------------------

void setAlarm(){
  int lcdX = 0;								// LCD INDEX X
  int blinkVar = alarmHour;					// STORE FOR BLINKING
  int switchIndex = 0;						// SWITCH INDEX = 0=hour, 1=minute
  lcd.clear();								// CLEAR LCD SCREEN
  setTimeStart = millis();					// START
  
  while(true){
    lcd.setCursor(0,0);						// LCD PRINT LINE 1 - (SET ALARM)
  	lcd.print("SET ALARM:");
  	
    lcd.setCursor(0,1);						// LCD PRINT LINE 2	(05:30)
	if (alarmHour < 10)	lcd.print("0");
	lcd.print(alarmHour);
   	lcd.print(":");
	if (alarmMinute < 10) lcd.print("0");
	lcd.print(alarmMinute);
    
    btnState1 = digitalRead(btnPin1);		// READ BUTTON
  	btnState2 = digitalRead(btnPin2);
  	btnState3 = digitalRead(btnPin3);
    	
    if (btnState1 && switchIndex == 1){		// SWITCH to HOUR	(Button 1)
      Serial.println("SET ALARM: switched to HOUR");
      switchIndex = 0;
      lcdX = 0;
    }
    else if (btnState1 && switchIndex == 0){// SWITCH to MINUTE (Button 1)
      Serial.println("SET ALARM: switched to MINUTE");
      switchIndex = 1; 
      lcdX = 3;
    }
    if (btnState3 && switchIndex == 0){		// INCREASE HOUR 	(Button 3)
      if(alarmHour == 24)	alarmHour = 0;
      alarmHour += 1;
    }
    if (btnState3 && switchIndex == 1){		// INCREASE MINUTE 	(Button 3)
      if(alarmMinute == 60)	alarmMinute = 0;
      alarmMinute += 1;	
    }
    if (btnState2 &&						// EXIT SETUP		(Button 2 & after 2sec)
        (millis() - setTimeStart) >= 2000){
      screenMode = 0;
      break;
    }
  
    // BLINK
    if (switchIndex == 0)	blinkVar = alarmHour;
    if (switchIndex == 1)	blinkVar = alarmMinute;
    blink(blinkVar, lcdX, 1);	
  }
  flagAlarm = 1;							//ALARM ACTIVATED
}



// ---------------------- BLINK ----------------------

void blink(int blinkVar, int x, int y){
  lcd.setCursor(x, y);
  if (flagBlink == 0){						// PRINT VALUE
    if (blinkVar < 10)	lcd.print("0");
      lcd.print(blinkVar);
      delay(400);
  	}
  if (flagBlink == 1){						// PRINT BLANK
      lcd.print("  ");
 	  delay(400);
  }
  flagBlink = (flagBlink + 1) % 2;
}



// ---------------- PRINT TEMPERATURE LCD ------------

void getTemperature(){
  celsius = map(((analogRead(tempPin)-20)*3.04),0,1023,-40,125);
  fahrenheit = ((celsius*9) / 5 + 32);
  
  if (flagTemp == 0) lcd.print(celsius);	// 0=C - PRINT LCD CELSIUS 
  if (flagTemp == 1) lcd.print(fahrenheit);	// 1=F - PRINT LCD FAHRENHEIT 
 	
  lcd.print((char)0xB0);	// ° symbol, (char)223
  
  if (flagTemp == 0) lcd.print("C");
  if (flagTemp == 1) lcd.print("F");
         
  //Serial.println(String(celsius) + " C\t" + String(fahrenheit) + " F");
  // return String(celsius) + (char)0xB0+"C";  
}
