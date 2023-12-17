//To do: menu sound, combos, maps,better icons/ tutorial?
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "LedControl.h"
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;
boolean lastButtonState = false;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
const int numMaps = 3;  // Number of matrix maps to do more and select random 
byte matrixMap[numMaps][8][8] = {
{
  {1, 0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 1, 0, 1, 0, 1},
  {1, 0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 1, 0, 1, 0, 1},
  {1, 0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 1, 0, 1, 0, 1},
  {1, 0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 1, 0, 1, 0, 1}
},
{
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
},
{
  {0, 0, 1, 0, 0, 1, 0, 0},
  {0, 1, 0, 1, 1, 0, 1, 0},
  {1, 0, 1, 0, 0, 1, 0, 1},
  {0, 1, 0, 1, 1, 0, 1, 0},
  {0, 1, 0, 1, 1, 0, 1, 0},
  {1, 0, 1, 0, 0, 1, 0, 1},
  {0, 1, 0, 1, 1, 0, 1, 0},
  {0, 0, 1, 0, 0, 1, 0, 0}
}
};

byte customChar[8] = {
	0b01110,
	0b01110,
	0b00100,
	0b11111,
	0b00100,
	0b01110,
	0b01010,
	0b01010
};

byte matrixSize = 8;
byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;
const int buzzerPin = 13; 
unsigned long lastButtonPressTime = 0;
const byte moveInterval = 100;
unsigned long long lastMoved = 0;
bool matrixChanged = true;
int xLast;
int yLast;
boolean buttonState = false;
boolean exist = false;
int xBlink = -1;
int yBlink = -1;
unsigned long lastPositionSetTime;
int selectedMap = 0;
const byte rsPin = 9;
const byte enPin = 8;
const byte d4Pin = 7;
const byte d5Pin = 3;
const byte d6Pin = 5;
const byte d7Pin = 4;
const byte Apin = 6;
const int xPin = A0;
const int yPin = A1;
const int pinSW = 2;
const int joyStickBtn = A2;
const int minThreshold = 300;
const int maxThreshold = 600;
const int debounceDelay = 50;
const int menuStart = 1;
const int menuSettings = 2;
const int menuCredits = 3;
const int menuHighScore = 4;
const int menuhowToPlay = 5;
LiquidCrystal lcd(rsPin, enPin, d4Pin, d5Pin, d6Pin, d7Pin);
int selectedDifficulty; 
int matrixBrightness;
int lcdBrightness;
int currentMenu = 1;
int subMenu = 1;
int lives = 3;
bool exitMenu = false;
int points = 0;
bool exitGame = false;
bool shownHighScore = false;
int highScore = 0;
int levels = 0;
boolean allowSound;

int highScore1st = 0;
int highScore2nd = 0;
int highScore3rd = 0;
char name[4] = {'A', 'A', 'A', '\0'}; // Initial name
int charPosition = 0; // Position of the character being edited (0, 1, or 2)
char currentPlayerName[4] = {'A', 'A', 'A', '\0'}; 


const int nameLength = 3; 
const int highScore1stNameAddress = 30; 
const int highScore2ndNameAddress = 34;
const int highScore3rdNameAddress = 38; 

//All the functions
void updateMenu();
void executeAction();
void startGame();
void handleGameOptions();
void handleMatrixOptions();
void handleLcdOptions();
void showCredits();
void executeGameMenuAction();
void displayGameOptions();
void setup();
void loop();
void updateMatrix();
void playBombSound();
void updatePositions();
void blink(byte x, byte y);
void blinkFast(byte x, byte y);
void generateRandomMap(byte matrix[8][8]);
bool areAllLedsOff(byte ignoreX, byte ignoreY);
void chooseDifficulty();
void chooseLightLevelMatrix();
void matrixLight();
void handleMatrixOptions();
void displayMatrixOptions();
void executeMatrixMenuAction();
void chooseLightLevelLcd();
void handleLcdOptions();
void displayLcdOptions();
void executeLcdMenuAction();
void showCredits();
void handlePlayerRespawn();
void saveNameToEEPROM();
void loadNameFromEEPROM();
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(Apin, OUTPUT); 
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(joyStickBtn, INPUT_PULLUP); 
  pinMode(buzzerPin, OUTPUT);

  highScore = EEPROM.read(4);
  lcdBrightness = EEPROM.read(2);
  analogWrite(Apin, lcdBrightness);  
  lcd.print(F("Boomber Man"));
  lcd.setCursor(0, 1);
  lcd.print(F("By Teodor"));
  delay(1000);
  lcd.clear();
  /*
  highScore1st = 300;
  highScore2nd = 200;
  highScore3rd = 100;
  EEPROM.write(8, highScore1st >> 8);
  EEPROM.write(9, highScore1st & 0xFF);

  EEPROM.write(10, highScore2nd >> 8);
  EEPROM.write(11, highScore2nd & 0xFF);

  EEPROM.write(12, highScore3rd >> 8);
  EEPROM.write(13, highScore3rd & 0xFF);
  */
  highScore1st = readHighScore(8);
  highScore2nd = readHighScore(10); 
  highScore3rd = readHighScore(12); 
  loadNameFromEEPROM();


  selectedDifficulty = EEPROM.read(0);
  matrixBrightness = EEPROM.read(1);
  allowSound = EEPROM.read(7);
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);

 //Matrix
  matrixMap[selectedMap][xPos][yPos] = 1;
  selectedMap = EEPROM.read(3);
  if(selectedMap > 2){
    selectedMap = 0; 
  }
  EEPROM.write(3, selectedMap); 
  
  updateMenu();
}


void loop() {

  /**/
  //HighScores: 

  int joyValue = analogRead(xPin);

  if (joyValue < minThreshold) {
    currentMenu++;
    if(allowSound){
      playMenuSound();
    }
    updateMenu();
    delay(debounceDelay);
    while (analogRead(xPin) < minThreshold);
  }

  if (joyValue > maxThreshold) {
    currentMenu--;
    if(allowSound){
      playMenuSound();
    }
    updateMenu();
    delay(debounceDelay);
    while (analogRead(xPin) > maxThreshold);
  }

  if (!digitalRead(pinSW)) {
    executeAction();
    delay(debounceDelay);
    updateMenu();
    delay(debounceDelay);
    while (!digitalRead(pinSW));
  }


}

//Update display based on the joystick inpus
void updateMenu() {
  if (currentMenu < menuStart){ currentMenu = menuhowToPlay;
      if(allowSound){playMenuSound();
        }
      }
  if (currentMenu > menuhowToPlay){
    currentMenu = menuStart;
    if(allowSound){playMenuSound();}
  } 

  lcd.clear();

  switch (currentMenu) {
    case menuStart:
      lcd.print(F(">START"));
      lcd.setCursor(0, 1);
      lcd.print(F(" SETTINGS"));
      break;
    case menuSettings:
      lcd.print(F(" START"));
      lcd.setCursor(0, 1);
      lcd.print(F(">SETTINGS"));
      break;
    case menuCredits:
      lcd.print(F(">CREDITS"));
      lcd.setCursor(0,1);
      lcd.print(F(" HIGHSCORES"));
      break;
    case menuHighScore:
    lcd.print(F(" CREDITS"));
    lcd.setCursor(0,1);
    lcd.print(F(">HIGHSCORES"));
    break;
    case menuhowToPlay:
    lcd.print(F(">HOW TO PLAY"));
    break;

  }
}

//Switch for the actions
void executeAction() {
  switch (currentMenu) {
    case menuStart:
      startGame();
      break;
  //Here i put handleSettings that has exit, but also has all the handles expect credtis.
    case menuSettings:
      handleGameSettings();//new menu
      break;
    case menuCredits:
      showCredits(); //display to the screen
      break;
    case menuHighScore:
      showHighScores(); //this will just display to the screen the highscores
      break;
    case menuhowToPlay:
      showHowToPlay(); //this will display how to play the game
      break;
  }
}

//Settings
void handleGameSettings(){
  lcd.clear();
  subMenu = 9;
  exitMenu = false;
  displayGameSettings();
  
  while (!exitMenu) {
    int joyValue = analogRead(xPin);

    if (joyValue < minThreshold) {
      subMenu++;
    if(allowSound){playMenuSound();}
      displayGameSettings();
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold);
    }

    if (joyValue > maxThreshold) {
      subMenu--;
      if(allowSound){playMenuSound();}
      displayGameSettings();
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold);
    }

    if (!digitalRead(pinSW)) {
      executeGameSettingsAction();
      delay(debounceDelay);
      displayGameSettings();
      delay(debounceDelay);
      while (!digitalRead(pinSW));
    }
  }
}
void displayGameSettings(){
  if (subMenu < 1){ subMenu = 7; if(allowSound){playMenuSound();}}
  if (subMenu > 7 && subMenu != 9) {
    subMenu = 1;
    if(allowSound){playMenuSound();}
  }
  if(subMenu > 9) subMenu = 1;
  
  lcd.clear();

  switch (subMenu) {
    case 1:
      lcd.print(">MATRIX");
      lcd.setCursor(0, 1);
      lcd.print(" LCD");
      break;
    case 2:
      lcd.print(" MATRIX");
      lcd.setCursor(0, 1);
      lcd.print(">LCD");
      break;
    case 3:
      lcd.print(">SOUND");
      lcd.setCursor(0, 1);
      lcd.print(" RESET HIGHSCORES");
      break;
    case 4: 
    lcd.setCursor(0,0);
      lcd.print(" SOUND");
      lcd.setCursor(0, 1);
      lcd.print(">RESET HIGHSCORES");
      break;
    case 5:
      lcd.setCursor(0,0);
      lcd.print(">SET NAME");
      lcd.setCursor(0, 1);
      lcd.print(" SET DIFFICULTY");
      break;
    case 6:
      lcd.setCursor(0,0);
      lcd.print(" SET NAME");
      lcd.setCursor(0, 1);
      lcd.print(">SET DIFFICULTY");
      break;
    case 7:
      lcd.setCursor(0,0);
      lcd.print(">EXIT");
      break;
  }
}


void executeGameSettingsAction(){
  switch(subMenu){
    case 1:
      //handleMatrixOptions();
    chooseLightLevelMatrix();
      break;
    case 2:
    //handleLcdOptions();
    chooseLightLevelLcd();
      break;
    case 3:
    //handleSoundOptions();
      chooseSoundOption();
      break;
    case 4:
      handleHighScoreOptions();
      break;
    case 5:
      handleChooseName();
      break;
    case 6:
    handleGameOptions();
    break;
    case 7:
      exitMenu = true;
    break;
    case 9:
      lcd.clear();
      lcd.print("LOADING...");
      delay(400);
      subMenu = 1;
    break;
  }
}
//Matrix Options
void handleMatrixOptions() {
  lcd.clear();
  subMenu = 4;
  exitMenu = false;
  displayMatrixOptions();
  
  while (!exitMenu) {
    int joyValue = analogRead(xPin);

    if (joyValue < minThreshold) {
      subMenu++;
      displayMatrixOptions();
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold);
    }

    if (joyValue > maxThreshold) {
      subMenu--;
      displayMatrixOptions();
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold);
    }

    if (!digitalRead(pinSW)) {
      executeMatrixMenuAction();
      delay(debounceDelay);
      displayMatrixOptions();
      delay(debounceDelay);
      while (!digitalRead(pinSW));
    }
  }
}




byte tempMap[8][8]; 
void copyMap() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            tempMap[i][j] = matrixMap[selectedMap][i][j];
        }
    }
}
//Start game
void startGame() {
  int beatenHighScore = 0;
  xPos = 0;
  yPos = 0;
  points = 0;
  boolean shown3rd = false;
  boolean shown2nd = false;
  boolean shown1st = false;
  lives = selectedDifficulty;
  lcd.clear();
  lcd.print("LOADING...");
  delay(400);
  lcd.clear();
  lcd.createChar(0, customChar);
  exitGame = false;
  boolean copyMatrix = false;
  while (exitGame == false) {
    if(copyMatrix == false){
      copyMap();
      updateMatrix();
      copyMatrix = true;
    }
    for (int i = 0; i < lives; i++) {
      lcd.setCursor(i, 0);  
      lcd.write((uint8_t) 0);  
    }
    lcd.setCursor(0,1);
    lcd.print("Points");
    lcd.setCursor(8, 1);
    lcd.print(points);
    lcd.setCursor(12, 1); 
    lcd.print(currentPlayerName); 

    boolean currentButtonState = digitalRead(pinSW);
    if(!digitalRead(joyStickBtn)) {
        exitGame = true;
        copyMatrix = false;
        }

    if (currentButtonState != lastButtonState) {
      lastButtonPressTime = millis();
    }
    if ((millis() - lastButtonPressTime) > debounceDelay) {
      if (currentButtonState == LOW) {
        buttonState = !buttonState;
      }
    }

    
    lastButtonState = currentButtonState;

    if (millis() - lastMoved >= moveInterval) { //how fast is the player allowed to move
      xLast = xPos;
      yLast = yPos;

      updatePositions();
      
      if ((xLast != xPos || yLast != yPos) && buttonState == true && exist == false) { //i am so dumb forgot to test if the bomb already exist to not be able to put 100 milion bombs...
        tempMap[xLast][yLast] = 1;
        lc.setLed(1, xLast, yLast, tempMap[xLast][yLast]); //i should use a shallow copy of the matrix i guess when i initialize the game;
        lastPositionSetTime = millis();
        exist = true;
        xBlink = xLast;
        yBlink = yLast;
        buttonState = false;
      }
      lastMoved = millis(); 
    }
    //bomb starts blinking if it exists
    if (exist) {
        blinkFast(xBlink, yBlink);
      }
      
      if (exist && millis() - lastPositionSetTime > 3000) {
        
        tempMap[xBlink][yBlink] = 0;
        lc.setLed(1, xBlink, yBlink, tempMap[xBlink][yBlink]);

        if (xBlink > 0) {
          //destroy the walls
          if(tempMap[xBlink-1][yBlink] == 1){
          points = points + 10/selectedDifficulty;
          }
          tempMap[xBlink - 1][yBlink] = 0;
          lc.setLed(1, xBlink - 1, yBlink, tempMap[xBlink - 1][yBlink]);

        //you die if you stand too close to the bomb and you go back to spawn;
          if (xPos == xBlink - 1 && yBlink == yPos) {
            handlePlayerRespawn();
          }

      }

      if (xBlink < (8 - 1)) {
        if( tempMap[xBlink + 1][yBlink] == 1){
          points = points + 10/selectedDifficulty;

          }
          tempMap[xBlink + 1][yBlink] = 0;
          lc.setLed(1, xBlink + 1, yBlink, tempMap[xBlink + 1][yBlink]);

          if (xPos == xBlink + 1 && yBlink == yPos) {
              handlePlayerRespawn();
          }
      }

        if (yBlink > 0) {
          if(tempMap[xBlink][yBlink - 1] == 1){
           points = points + 10/selectedDifficulty;
          }
          tempMap[xBlink][yBlink - 1] = 0;
          lc.setLed(1, xBlink, yBlink - 1, tempMap[xBlink][yBlink - 1]);

          if (xPos == xBlink && yBlink - 1 == yPos) {
            handlePlayerRespawn();
          }
        }

        if (yBlink < (8 - 1)) {
          if(tempMap[xBlink][yBlink + 1] == 1){
          points = points + 10/selectedDifficulty;
          }
          tempMap[xBlink][yBlink + 1] = 0;
          lc.setLed(1, xBlink, yBlink + 1, tempMap[xBlink][yBlink + 1]);

          if (xPos == xBlink && yBlink + 1 == yPos) {
            handlePlayerRespawn();
          }
        }

        exist = false;
        if(allowSound == true){
          playBombSound();
        }
        updateMatrix();
    }

      if (matrixChanged) {
        updateMatrix();
        matrixChanged = false;
      }
      
      blink(xPos, yPos);
      if(lives == 0){
        exitGame = true;
        if (points > highScore3rd) {
            highScore3rd = points;
            beatenHighScore = 3;
            if (points > highScore2nd) {
              highScore3rd = highScore2nd;
              highScore2nd = points;
              beatenHighScore = 2;
              if (points > highScore1st) {
                highScore2nd = highScore1st;
                highScore1st = points;
                beatenHighScore = 1;
              }
            }
          }
            if (beatenHighScore > 0) {
            lcd.clear();
            lcd.print(F("You beat the:"));
            lcd.setCursor(0, 1);
            if (beatenHighScore == 1 && shown1st == false) {
              lcd.print(F("1st HIGHSCORE!"));
              shown1st = true;
              saveHighScoreNameToEEPROM(highScore1stNameAddress, currentPlayerName);  // Save name for 1st high score
            } else if (beatenHighScore == 2 && shown2nd == false) {
              lcd.print(F("2nd HIGHSCORE!"));
              shown2nd = true;
              saveHighScoreNameToEEPROM(highScore2ndNameAddress, currentPlayerName);  
            } else if (beatenHighScore == 3 && shown3rd == false) {
              lcd.print(F("3rd HIGHSCORE!"));
              shown3rd = true;
              saveHighScoreNameToEEPROM(highScore3rdNameAddress, currentPlayerName);  
            }
            delay(2000);
            lcd.clear();
            beatenHighScore = 0; 
          }
        bool exitThisThing = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(F("You died"));
          delay(1000);
          lcd.clear();
          lcd.print(F("Score:"));
          lcd.setCursor(7,0);
          lcd.print(points);
          delay(1000);
          selectedMap++;
          if(selectedMap > 2){
            selectedMap = 0;
          }
          copyMatrix = false;
          points = 0; 
        while (exitThisThing == false){
          while(!digitalRead(joyStickBtn)) {
            exitThisThing = true;
          }
        }
        lcd.clear();
            
        }
    if (areAllLedsOff(xPos, yPos)) {
          copyMatrix = false;
          selectedMap++;
          if(selectedMap > 2){
            selectedMap = 0;
          }
      if (points > highScore3rd) {
            highScore3rd = points;
            beatenHighScore = 3;
            if (points > highScore2nd) {
              highScore3rd = highScore2nd;
              highScore2nd = points;
              beatenHighScore = 2;
              if (points > highScore1st) {
                highScore2nd = highScore1st;
                highScore1st = points;
                beatenHighScore = 1;
              }
            }
          }
              if (beatenHighScore > 0) {
            lcd.clear();
            lcd.print(F("You beat the:"));
            lcd.setCursor(0, 1);
            if (beatenHighScore == 1 && shown1st == false) {
              lcd.print(F("1st HIGHSCORE!"));
              shown1st = true;
              saveHighScoreNameToEEPROM(highScore1stNameAddress, currentPlayerName);  // Save name for 1st high score
            } else if (beatenHighScore == 2 && shown2nd == false) {
              lcd.print(F("2nd HIGHSCORE!"));
              shown2nd = true;
              saveHighScoreNameToEEPROM(highScore2ndNameAddress, currentPlayerName);  
            } else if (beatenHighScore == 3 && shown3rd == false) {
              lcd.print(F("3rd HIGHSCORE!"));
              shown3rd = true;
              saveHighScoreNameToEEPROM(highScore3rdNameAddress, currentPlayerName);  
            }
            delay(2000);
            lcd.clear();
            beatenHighScore = 0; // Reset the flag
          }
          EEPROM.write(3, selectedMap);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(F("Finished map"));
          lcd.setCursor(13,0);
          lcd.print(levels);
          delay(2000);
          lcd.clear();
          xPos = 0;
          yPos = 0;
          levels++;
          updateMatrix();
          if(levels == 3){
            //finished all 3 levels;
            boolean exitThisThing = false;
            lcd.setCursor(0,0);
            lcd.print(F("You won!"));
            lcd.setCursor(0,1);
            lcd.print("Score:");
            lcd.setCursor(8,1);
            lcd.print(points);
            delay(1000);
            while (exitThisThing == false){
            while(!digitalRead(joyStickBtn)) {
              exitThisThing = true;
            }
            }
            exitGame = true;
            levels = 0;
            selectedMap = 0;
            lives = selectedDifficulty;
            points = 0;
            copyMatrix = false;
            updateMatrix();
            
        }
    
    }
  }
  //Write the values in the storagespcae
    EEPROM.write(8, highScore1st >> 8);
    EEPROM.write(9, highScore1st & 0xFF);
    EEPROM.write(10, highScore2nd >> 8);
    EEPROM.write(11, highScore2nd & 0xFF);
    EEPROM.write(12, highScore3rd >> 8);
    EEPROM.write(13, highScore3rd & 0xFF);
    lc.clearDisplay(0);
  
  
}

int readHighScore(int address) {
  int highByte = EEPROM.read(address);
  int lowByte = EEPROM.read(address + 1);
  return (highByte << 8) | lowByte;
}

void handlePlayerRespawn(){
  yPos = 0;
  xPos = 0;
  lives--;
  points = points - 20;
  if(points < 0){
    points = 0;
  }
  xLast = 1;
  yLast = 1;
  xBlink = 3;
  xBlink = 3;
  exist = false;
  lcd.clear();
}

//Game Options
void handleGameOptions() {
  lcd.clear();
  subMenu = 4;
  exitMenu = false;
  displayGameOptions();
  
  while (!exitMenu) {
    int joyValue = analogRead(xPin);

    if (joyValue < minThreshold) {
      subMenu++;
      displayGameOptions();
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold);
    }

    if (joyValue > maxThreshold) {
      subMenu--;
      displayGameOptions();
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold);
    }

    if (!digitalRead(pinSW)) {
      executeGameMenuAction();
      delay(debounceDelay);
      displayGameOptions();
      delay(debounceDelay);
      while (!digitalRead(pinSW));
    }
  }
}

void displayGameOptions() {
  if (subMenu < 1) subMenu = 2;
  if (subMenu > 2 && subMenu != 4) {
    subMenu = 1;
  }
  if(subMenu > 4) subMenu = 1;
  
  lcd.clear();

  switch (subMenu) {
    case 1:
      lcd.print(F(">Difficulty"));
      lcd.setCursor(0, 1);
      lcd.print(F(" Exit"));
      break;
    //here i should make it lives, bomb time, ??
    case 2:
      lcd.print(" Difficulty");
      lcd.setCursor(0, 1);
      lcd.print(">Exit");
      break;
  }
}

void executeGameMenuAction() {
  switch (subMenu) {
    case 1:
      chooseDifficulty();
      break;
    case 2:
      exitMenu = true;
      break;
    case 4:
      lcd.clear();
      lcd.print("LOADING...");
      delay(400);
      subMenu =1;
      break;
  }
}

void chooseDifficulty() {
  lcd.clear();
  lcd.print(F("Difficulty"));
  boolean exitThis = false;
  while (exitThis == false) { 
    int joyValue = analogRead(xPin);
     if(!digitalRead(joyStickBtn)) {
      exitThis = true;
      }
    if (joyValue < minThreshold) {
      selectedDifficulty++;
      if (selectedDifficulty > 10) {
        selectedDifficulty = 10;
      }
      lcd.setCursor(0, 0);
      lcd.print(F("Difficulty"));
      lcd.setCursor(0, 1);
      lcd.print(selectedDifficulty);
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold);
    }

    if (joyValue > maxThreshold) {
      lcd.clear();
      selectedDifficulty--;
      if (selectedDifficulty < 1) {
        selectedDifficulty = 1;
      }
        lcd.setCursor(0, 0);
      lcd.print(F("Difficulty"));
      lcd.setCursor(0, 1);
      lcd.print(selectedDifficulty);
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold);

    }
    EEPROM.write(0, selectedDifficulty);

    }
  delay(500); 
}

void displayMatrixOptions() {
  if (subMenu < 1) subMenu = 2;
  if (subMenu > 2 && subMenu != 4) {
    subMenu = 1;
  }
  if(subMenu > 4) subMenu = 1;
  
  lcd.clear();

  switch (subMenu) {
    case 1:
      lcd.print(">Brightness");
      lcd.setCursor(0, 1);
      lcd.print(" Exit");
      break;
    case 2:
      lcd.print(" Brightness");
      lcd.setCursor(0, 1);
      lcd.print(">Exit");
      break;
  }
}

void executeMatrixMenuAction(){
    switch (subMenu) {
    case 1:
      chooseLightLevelMatrix();
      break;
    case 2:
      exitMenu = true;
      break;
    case 4:
      lcd.clear();
      lcd.print("LOADING...");
      delay(400);
      subMenu = 1;
      break;
  }
}

void chooseLightLevelMatrix(){
  lcd.clear();
  lcd.print(F("Set Brightness"));
  boolean exitThis = false;
  while (exitThis == false) { 
  int joyValue = analogRead(xPin);
     if(!digitalRead(joyStickBtn)) {
      exitThis = true;
      }
    if (joyValue < minThreshold) {
      matrixBrightness++;
      if (matrixBrightness > 10) {
        matrixBrightness = 10;
      }
      lcd.setCursor(0, 0);
      lcd.print(F("Set Brightness"));
      lcd.setCursor(0, 1);
      lcd.print(matrixBrightness);
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold);
    }

    if (joyValue > maxThreshold) {
      lcd.clear();
      matrixBrightness--;
      if (matrixBrightness < 1) {
        matrixBrightness = 1;
      }
      lcd.setCursor(0, 0);
      lcd.print(F("Set Brightness"));
      lcd.setCursor(0, 1);
  
      lcd.print(matrixBrightness);
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold);

    }
    EEPROM.write(1, matrixBrightness);

    }
  lc.setIntensity(0, matrixBrightness);
  matrixLight();
  delay(500); 
  lc.clearDisplay(0);
}


void matrixLight() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, 1);
    }
  }

  delay(2000);  

  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, 0); 
    }
  }
}


//Lcd options
void handleLcdOptions() {
  lcd.clear();
  subMenu = 4;
  exitMenu = false;
  displayLcdOptions();
  
  while (!exitMenu) {
    int joyValue = analogRead(xPin);

    if (joyValue < minThreshold) {
      subMenu++;
      displayLcdOptions();
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold);
    }

    if (joyValue > maxThreshold) {
      subMenu--;
      displayLcdOptions();
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold);
    }

    if (!digitalRead(pinSW)) {
      executeLcdMenuAction();
      delay(debounceDelay);
      displayLcdOptions();
      delay(debounceDelay);
      while (!digitalRead(pinSW));
    }
  }

}
void displayLcdOptions(){
  if (subMenu < 1) subMenu = 2;
  if (subMenu > 2 && subMenu != 4) {
    subMenu = 1;
  }
  if(subMenu > 4) subMenu = 1;
  
  lcd.clear();

  switch (subMenu) {
    case 1:
      lcd.print(">LCD Brightness");
      lcd.setCursor(0, 1);
      lcd.print(" Exit");
      break;
    case 2:
      lcd.print(" LCD Brightness");
      lcd.setCursor(0, 1);
      lcd.print(">Exit");
      break;
  }
}

void executeLcdMenuAction(){
    switch (subMenu) {
    case 1:
      chooseLightLevelLcd();
      break;
    case 2:
      exitMenu = true;
      break;
    case 4:
      lcd.clear();
      lcd.print("LOADING...");
      delay(400);
      subMenu = 1;
      break;
  }
}

void chooseLightLevelLcd(){
  lcd.clear();
  lcd.print(F("Set Brightness:"));
  boolean exitThis = false;
  while (exitThis == false) { 
    int joyValue = analogRead(xPin);
     if(!digitalRead(joyStickBtn)) {
      exitThis = true;
      }
//thse values gave me a headache why when i used 10-30 the values are random it becomes brighter when the value is lower??? but for 20 it works what?
    if (joyValue < minThreshold) {
      lcdBrightness += 20;
      if (lcdBrightness > 200) {
        lcdBrightness = 200;
      }
      lcd.setCursor(0, 0);
      lcd.print(F("Set Brightness"));
      lcd.setCursor(0, 1);
      lcd.print(lcdBrightness);
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold);
    }

    if (joyValue > maxThreshold) {
      lcdBrightness -= 20;
      lcd.clear();
      if (lcdBrightness < 20) {
        lcdBrightness = 20;
      }
      lcd.setCursor(0, 0);
      lcd.print(F("Set Brightness"));
      lcd.setCursor(0, 1);
      lcd.print(lcdBrightness);
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold);

    }
    EEPROM.write(2, lcdBrightness);
    }
  analogWrite(Apin, lcdBrightness);  
 
  delay(500); 
}

//Sound off/on:
void handleSoundOptions(){
  lcd.clear();
  subMenu = 4;
  exitMenu = false;
  displaySoundOptions();
  
  while (!exitMenu) {
    int joyValue = analogRead(xPin);

    if (joyValue < minThreshold) {
      subMenu++;
      displaySoundOptions();
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold);
    }

    if (joyValue > maxThreshold) {
      subMenu--;
      displaySoundOptions();
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold);
    }

    if (!digitalRead(pinSW)) {
      executeSoundMenuAction();
      delay(debounceDelay);
      displaySoundOptions();
      delay(debounceDelay);
      while (!digitalRead(pinSW));
    }
  }
}

void displaySoundOptions(){
  if (subMenu < 1) subMenu = 2;
  if (subMenu > 2 && subMenu != 4) {
    subMenu = 1;
  }
  if(subMenu > 4) subMenu = 1;
  
  lcd.clear();

  switch (subMenu) {
    case 1:
      lcd.print(">Sound ON/OFF");
      lcd.setCursor(0, 1);
      lcd.print(" Exit");
      break;
    case 2:
      lcd.print(" Sound ON/OFF");
      lcd.setCursor(0, 1);
      lcd.print(">Exit");
      break;
  }
}

void executeSoundMenuAction(){
    switch (subMenu) {
    case 1:
      chooseSoundOption();
      break;
    case 2:
      exitMenu = true;
      break;
    case 4:
      lcd.clear();
      lcd.print("LOADING...");
      delay(400);
      subMenu =1;
      break;
  }
}
void chooseSoundOption() {
  lcd.clear();
  lcd.print(F("Enable Sound:"));
  boolean exitThis = false;
  while (!exitThis) {
    int joyValue = analogRead(xPin);
    if (joyValue < minThreshold) {
      allowSound = true;
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold);
    } else if (joyValue > maxThreshold) {
      allowSound = false;
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold); 
    }

    // Update LCD with current sound setting
    lcd.setCursor(0, 1);
    if (allowSound) {
      lcd.print(F("Yes"));
    } else {
      lcd.print(F("No"));
    }

    // Exit on button press
    if (!digitalRead(joyStickBtn)) {
      exitThis = true;
    }

    if (allowSound) {
      playBombSound();
    }
  }
  EEPROM.write(7, allowSound);
  delay(150); 
}
void handleHighScoreOptions(){
   lcd.clear();
  subMenu = 4;
  exitMenu = false;
  displayHighScoreOptions();
  
  while (!exitMenu) {
    int joyValue = analogRead(xPin);

    if (joyValue < minThreshold) {
      subMenu++;
      displayHighScoreOptions();
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold);
    }

    if (joyValue > maxThreshold) {
      subMenu--;
      displayHighScoreOptions();
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold);
    }

    if (!digitalRead(pinSW)) {
      executeHighScoreMenuAction();
      delay(debounceDelay);
      displayHighScoreOptions();
      delay(debounceDelay);
      while (!digitalRead(pinSW));
    }
  }
}

void displayHighScoreOptions(){
  if (subMenu < 1) subMenu = 2;
  if (subMenu > 2 && subMenu != 4) {
    subMenu = 1;
  }
  if(subMenu > 4) subMenu = 1;
  
  lcd.clear();

  switch (subMenu) {
    case 1:
      lcd.print(">Confirm Reset");
      lcd.setCursor(0, 1);
      lcd.print(" EXIT");
      break;
    case 2:
      lcd.print(" Confirm Reset");
      lcd.setCursor(0, 1);
      lcd.print(">EXIT");
      break;
  }
}

void executeHighScoreMenuAction(){
    switch (subMenu) {
    case 1:
      chooseHighScoreOption();
      break;
    case 2:
      exitMenu = true;
      break;
    case 4:
      lcd.clear();
      lcd.print("LOADING...");
      delay(400);
      subMenu =1;
      break;
  }
}

void chooseHighScoreOption() {
  lcd.clear();
  boolean exitThis = false;
  lcd.setCursor(0, 0);
  lcd.print(F("Scores reseted"));
  delay(1000);
  highScore1st = 110;
  highScore2nd = 50;
  highScore3rd = 10;
  EEPROM.write(8, highScore1st >> 8);
  EEPROM.write(9, highScore1st & 0xFF);
  EEPROM.write(10, highScore2nd >> 8);
  EEPROM.write(11, highScore2nd & 0xFF);
  EEPROM.write(12, highScore3rd >> 8);
  EEPROM.write(13, highScore3rd & 0xFF);

  // Save default names for the high scores
  saveHighScoreNameToEEPROM(highScore1stNameAddress, "ONE");
  saveHighScoreNameToEEPROM(highScore2ndNameAddress, "TWO");
  saveHighScoreNameToEEPROM(highScore3rdNameAddress, "TRD");

  highScore1st = readHighScore(8);
  highScore2nd = readHighScore(10);
  highScore3rd = readHighScore(12);

}





void handleChooseName() {
  bool exitThis = false;
  lcd.clear();
  while (!exitThis) {
    lcd.setCursor(0, 0);
    lcd.print(name);
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);
    if (yValue < minThreshold) {
      name[charPosition]--;
      if (name[charPosition] < 'A') name[charPosition] = 'Z';
      delay(200);
    } else if (yValue > maxThreshold) {
      name[charPosition]++;
      if (name[charPosition] > 'Z') name[charPosition] = 'A';
      delay(200);
    }

    //move to the next/previous character
    if (xValue < minThreshold) {
      charPosition = (charPosition + 1) % 3;
      delay(200);
    } else if (xValue > maxThreshold) {
      charPosition = (charPosition - 1 + 3) % 3;
      delay(200);
    }

    //exit on button press
    if (!digitalRead(joyStickBtn)) {
        strcpy(currentPlayerName, name); 
      saveNameToEEPROM(); 
      exitThis = true;
    }

    // Update the display
    lcd.setCursor(0, 1);
    lcd.print("Edit: ");
    lcd.print(charPosition + 1);
  }

}

//Credits
void showCredits() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game made by Balan Teodor, BomberMan");
  delay(2000); 

  for (int i = 0; i < 20; ++i) {
    lcd.scrollDisplayLeft();
    delay(250);  
  }
  delay(450);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Introduction to Robotics on GitHub.");
  delay(2000);  

  for (int i = 0; i < 19; ++i) {
    lcd.scrollDisplayLeft();
    delay(250);  
  }

  lcd.clear();
  lcd.print(F("High Score:"));
  lcd.setCursor(0, 1);
  lcd.print(highScore);
  delay(3500);

}


// MATRIX GAME FUNCTIONS : 

void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, tempMap[row][col]);
    }
  }
}


void playBombSound() {
tone(buzzerPin, 800,350); //bomb sound
}

void playMenuSound(){
tone(buzzerPin, 500, 200);
}
void updatePositions() {
  static bool hasMoved = false; // Declare as static

  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  byte newXPos = xPos;
  byte newYPos = yPos;

  if (xValue < minThreshold && newXPos > 0 && !hasMoved) {
    newXPos--;
    hasMoved = true;
  } else if (xValue > maxThreshold && newXPos < matrixSize - 1 && !hasMoved) {
    newXPos++;
    hasMoved = true;
  }

  if (yValue < minThreshold && newYPos < matrixSize - 1 && !hasMoved) {
    newYPos++;
    hasMoved = true;
  } else if (yValue > maxThreshold && newYPos > 0 && !hasMoved) {
    newYPos--;
    hasMoved = true;
  }

  if (tempMap[newXPos][newYPos] == 0 && hasMoved) {
    matrixChanged = true;
    tempMap[xPos][yPos] = 0;
    tempMap[newXPos][newYPos] = 1;
    xLast = xPos;
    yLast = yPos;
    xPos = newXPos;
    yPos = newYPos;
  } else {
    // Reset hasMoved if no movement occurred
    hasMoved = false;
  }
}

void blink(byte x, byte y ) {
  static unsigned long lastBlinkTime = 0;
  static bool isOn = true;
  static unsigned blinkInterval = 400;
  if (millis() - lastBlinkTime >= blinkInterval) {
    isOn = !isOn;
    lc.setLed(0, x, y, isOn);
    
    lastBlinkTime = millis();
  }
}
void blinkFast(byte x, byte y) {
  static unsigned long lastBlinkTime = 0;
  static bool isOn = true;
  static unsigned blinkInterval = 150;
  if (millis() - lastBlinkTime >= blinkInterval) {
    isOn = !isOn;
    lc.setLed(0, x, y, isOn);
    
    lastBlinkTime = millis();
  }
}
//here i could generate a random matrix but it seems more fun to have levels
void generateRandomMap(byte matrix[8][8]) {
  for (byte i = 0; i < matrixSize; ++i) {
    for (byte j = 0; j < matrixSize; ++j) {
      matrix[i][j] = random(2);  
    }
  }
  matrix[0][0] = 0;
  matrix[1][0] = 0; //to be able to escape any random generated levels you need at least 3 spaces
  matrix[2][0] = 0;
}

bool areAllLedsOff(byte ignoreX, byte ignoreY) {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if ((row != ignoreX || col != ignoreY) && tempMap[row][col] != 0) {
        return false;
      }
    }
  }
  return true;
}


void showHighScores() {
  char nameBuffer[nameLength + 1]; //

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Top Scores:"));
//display the scores in order with the names
  loadHighScoreNameToEEPROM(highScore1stNameAddress, nameBuffer);
  lcd.setCursor(0, 1);
  lcd.print(F("1st: "));
  lcd.print(highScore1st);
  lcd.print(" ");
  lcd.print(nameBuffer);
  delay(1500);

  loadHighScoreNameToEEPROM(highScore2ndNameAddress, nameBuffer);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("2nd: "));
  lcd.print(highScore2nd);
  lcd.print(" ");
  lcd.print(nameBuffer);
  delay(1500);

  loadHighScoreNameToEEPROM(highScore3rdNameAddress, nameBuffer);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("3rd: "));
  lcd.print(highScore3rd);
  lcd.print(" ");
  lcd.print(nameBuffer);
  delay(1500);
}


void showHowToPlay(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You use the joystick to move.");
  delay(1000); 

  for (int i = 0; i < 18; ++i) {
    lcd.scrollDisplayLeft();
    delay(250);  
  }
  delay(450);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Use the button to put bombs that-");
  delay(1000);  

  for (int i = 0; i < 17; ++i) {
    lcd.scrollDisplayLeft();
    delay(250);  
  }
  delay(250);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("destroy the walls and the player.");
  delay(1000);  
  for (int i = 0; i < 17; ++i) {
    lcd.scrollDisplayLeft();
    delay(250);  
  }
  delay(250);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("you get points for every wall and");
  delay(1000);  
  for (int i = 0; i < 18; ++i) {
    lcd.scrollDisplayLeft();
    delay(250);  
  }
  delay(250);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("lose points if you die GLHF!");
  delay(1000);  
  for (int i = 0; i < 15; ++i) {
    lcd.scrollDisplayLeft();
    delay(250);  
  }


}


void saveNameToEEPROM() {
  for (int i = 0; i < 3; ++i) {
    EEPROM.write(20 + i, currentPlayerName[i]);
  }
}

void loadNameFromEEPROM() {
  for (int i = 0; i < 3; ++i) {
    currentPlayerName[i] = EEPROM.read(20 + i);
  }
  currentPlayerName[3] = '\0';
}
//should you with address on all..
void saveHighScoreNameToEEPROM(int address, const char* name) {
  for (int i = 0; i < nameLength; ++i) {
    EEPROM.write(address + i, name[i]);
  }
}
void loadHighScoreNameToEEPROM(int address, char* name) {
  for (int i = 0; i < nameLength; ++i) {
    name[i] = EEPROM.read(address + i);
  }
  name[nameLength] = '\0';
}

