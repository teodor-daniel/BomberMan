/*
Regrettably, this program relies on 'delay' and blocking 'while' loops, potentially impeding multitasking and responsiveness due to global scope obstruction. However, within this particular application, the absence of multitasking aligns with the design intent, as each section operates autonomously, ensuring a methodical progression from one task to the next. This approach fosters a deliberate and orderly execution, befitting the game's straightforward and linear nature.
Acknowledging the  recurrent use of variables,  especially in controller initialization it's important to note that this strategy was deliberately employed to reduce complexity and uphold a uniform code structure 
Moreover, current memory utilization stands at 58%, indicating a moderate consumption level. Introducing a more complex system of flags to track the player's menu position could unnecessarily inflate variable usage and memory demand. The chosen methodology, therefore, strikes a balance between functionality and resource efficiency, offering a pragmatic solution for the game's scale and scope.
 */

#include <EEPROM.h>
#include "LedControl.h"
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;
boolean lastButtonState = false;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
byte tempMap[8][8];
byte life[8] = {
  0b01110,
  0b01110,
  0b00100,
  0b11111,
  0b00100,
  0b01110,
  0b01010,
  0b01010
};

byte startGameIcon[8] = {
  0b00000,
  0b01110,
  0b00100,
  0b00100,
  0b00100,
  0b11111,
  0b11111,
  0b11111
};

byte settingIcon[8] = {
  0b01010,
  0b01010,
  0b01110,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};

byte heartIcon[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};
byte highScoreIcon[8] = {
  0b11111,
  0b10001,
  0b10001,
  0b01110,
  0b00100,
  0b00100,
  0b01110,
  0b11111
};
byte questionMarkIcon[8] = {
  0b11110,
  0b10010,
  0b10010,
  0b00010,
  0b00010,
  0b00000,
  0b00010,
  0b00000
};

byte happyFaceIcon[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00100,
  0b10001,
  0b01110,
  0b00000
};
byte downArrow[8] = {
  0b00000,
  0b00100,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00000
};

byte topArrow[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b00100,
  0b00000
};

byte matrixSize = 8;
byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;
const int buzzerPin = 13;
unsigned long lastButtonPressTime = 0;
const byte moveInterval = 150;
unsigned long long lastMoved = 0;
bool matrixChanged = true;
int xLast;
int yLast;
boolean buttonState = false;
boolean exist = false;
int xBlink = -1;
int yBlink = -1;
unsigned long lastPositionSetTime;
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
const int minThreshold = 400;
const int maxThreshold = 600;
const int debounceDelay = 80;
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
int levels = 1;
int timeForBomb = 3000;
boolean allowSound;
boolean allowFasterBombs;
int highScore1st = 0;
int highScore2nd = 0;
int highScore3rd = 0;
char name[4] = { 'A', 'A', 'A', '\0' };  // Initial name
int charPosition = 0;
char currentPlayerName[4] = { 'A', 'A', 'A', '\0' };
const int nameLength = 3;
//names
const int highScore1stNameAddress = 30;
const int highScore2ndNameAddress = 34;
const int highScore3rdNameAddress = 38;


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
  highScore1st = readHighScore(8);
  highScore2nd = readHighScore(10);
  highScore3rd = readHighScore(12);
  loadNameFromEEPROM();

  selectedDifficulty = EEPROM.read(0);
  matrixBrightness = EEPROM.read(1);
  allowSound = EEPROM.read(7);
  allowFasterBombs = EEPROM.read(39);
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);

  lcd.createChar(1, startGameIcon);
  lcd.createChar(2, settingIcon);
  lcd.createChar(3, heartIcon);
  lcd.createChar(4, highScoreIcon);
  lcd.createChar(5, questionMarkIcon);
  lcd.createChar(6, happyFaceIcon);
  lcd.createChar(7, downArrow);
  lcd.createChar(0, topArrow);
  updateMenu();
}


void loop() {
  int joyValue = analogRead(xPin);

  if (joyValue < minThreshold) {
    currentMenu++;
    if (allowSound) {
      playMenuSound();
    }
    updateMenu();
    delay(debounceDelay);
    while (analogRead(xPin) < minThreshold)
      ;
  }

  if (joyValue > maxThreshold) {
    currentMenu--;
    if (allowSound) {
      playMenuSound();
    }
    updateMenu();
    delay(debounceDelay);
    while (analogRead(xPin) > maxThreshold)
      ;
  }

  if (!digitalRead(pinSW)) {
    executeAction();
    delay(debounceDelay);
    updateMenu();
    delay(debounceDelay);
    while (!digitalRead(pinSW))
      ;
  }
}

//Update display based on the joystick inpus
void updateMenu() {
  if (currentMenu < menuStart) {
    currentMenu = menuhowToPlay;
    if (allowSound) {
      playMenuSound();
    }
  }
  if (currentMenu > menuhowToPlay) {
    currentMenu = menuStart;
    if (allowSound) { playMenuSound(); }
  }

  lcd.clear();

  switch (currentMenu) {
    case menuStart:
      lcd.setCursor(0, 0);
      lcd.print(F(">START"));
      lcd.setCursor(7, 0);
      lcd.write((uint8_t)1);
      lcd.setCursor(0, 1);  //starGame icon
      lcd.print(F(" SETTINGS"));
      lcd.setCursor(10, 1);
      lcd.write((uint8_t)2);  //setting icon
      lcd.setCursor(15, 1);
      lcd.write((uint8_t)7);

      break;
    case menuSettings:
      lcd.setCursor(0, 0);
      lcd.print(F(" START"));
      lcd.setCursor(7, 0);
      lcd.write((uint8_t)1);
      lcd.setCursor(0, 1);
      lcd.print(F(">SETTINGS"));
      lcd.setCursor(10, 1);
      lcd.write((uint8_t)2);
      lcd.setCursor(15, 1);
      lcd.write((uint8_t)7);
      break;
    case menuCredits:
      lcd.setCursor(0, 0);
      lcd.print(F(">CREDITS"));
      lcd.setCursor(9, 0);
      lcd.write((uint8_t)3);  //credits icon heart
      lcd.setCursor(0, 1);
      lcd.print(F(" HIGHSCORES"));
      lcd.setCursor(12, 1);
      lcd.write((uint8_t)4);  //cup
      lcd.setCursor(15, 1);
      lcd.write((uint8_t)7);  //down arrow
      break;
    case menuHighScore:
      lcd.setCursor(0, 0);
      lcd.print(F(" CREDITS"));
      lcd.setCursor(9, 0);
      lcd.write((uint8_t)3);
      lcd.setCursor(0, 1);
      lcd.print(F(">HIGHSCORES"));
      lcd.setCursor(12, 1);
      lcd.write((uint8_t)4);
      lcd.setCursor(15, 1);
      lcd.write((uint8_t)7);
      break;
    case menuhowToPlay:
      lcd.setCursor(0, 0);
      lcd.print(F(">TUTORIAL"));
      lcd.setCursor(10, 0);
      lcd.write((uint8_t)5);  //questionMark
      lcd.setCursor(15, 0);
      lcd.write((uint8_t)0);
      break;
  }
}

//Switch for the actions based on the action either go into a new menu like handleGameSettings, or start the game/tutorial or show credits/highscore
void executeAction() {
  switch (currentMenu) {
    case menuStart:
      startGame();
      lcd.createChar(0, topArrow);
      break;
    case menuSettings:
      handleGameSettings();
      break;
    case menuCredits:
      showCredits();  //display to the screen
      break;
    case menuHighScore:
      showHighScores();  //this will just display to the screen the highscores
      break;
    case menuhowToPlay:
      showHowToPlay();  //this will display how to play the game
      break;
  }
}

//Settings wait for the player to select a setting, have to use button on exit to leave
void handleGameSettings() {
  lcd.clear();
  subMenu = 9;
  exitMenu = false;
  displayGameSettings();

  while (!exitMenu) {
    int joyValue = analogRead(xPin);

    if (joyValue < minThreshold) {
      subMenu++;
      if (allowSound) { playMenuSound(); }
      displayGameSettings();
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold)
        ;
    }

    if (joyValue > maxThreshold) {
      subMenu--;
      if (allowSound) { playMenuSound(); }
      displayGameSettings();
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold)
        ;
    }

    if (!digitalRead(pinSW)) {
      executeGameSettingsAction();
      delay(debounceDelay);
      displayGameSettings();
      delay(debounceDelay);
      while (!digitalRead(pinSW))
        ;
    }
  }
}
//here is the display
void displayGameSettings() {
  if (subMenu < 1) {
    subMenu = 7;
    if (allowSound) { playMenuSound(); }
  }
  if (subMenu > 7 && subMenu != 9) {
    subMenu = 1;
    if (allowSound) { playMenuSound(); }
  }
  //trick to no immediately go into the first option, as the program will wait for the next input, 
  if (subMenu > 9) subMenu = 1;

  lcd.clear();

  switch (subMenu) {
    case 1:
      lcd.print(">MATRIX");
      lcd.setCursor(0, 1);
      lcd.print(" LCD");
      lcd.setCursor(15, 1);
      lcd.write((uint8_t)7);
      break;
    case 2:
      lcd.print(" MATRIX");
      lcd.setCursor(0, 1);
      lcd.print(">LCD");
      lcd.setCursor(15, 1);
      lcd.write((uint8_t)7);
      break;
    case 3:
      lcd.print(">SOUND");
      lcd.setCursor(0, 1);
      lcd.print(" RESET SCORES");
      lcd.setCursor(15, 1);
      lcd.write((uint8_t)7);
      break;
    case 4:
      lcd.setCursor(0, 0);
      lcd.print(" SOUND");
      lcd.setCursor(0, 1);
      lcd.print(">RESET SCORES");
      lcd.setCursor(15, 1);
      lcd.write((uint8_t)7);
      break;
    case 5:
      lcd.setCursor(0, 0);
      lcd.print(">SET NAME");
      lcd.setCursor(0, 1);
      lcd.print(" SET DIFFICULTY");
      lcd.setCursor(15, 1);
      lcd.write((uint8_t)7);
      break;
    case 6:
      lcd.setCursor(0, 0);
      lcd.print(" SET NAME");
      lcd.setCursor(0, 1);
      lcd.print(">SET DIFFICULTY");
      lcd.setCursor(15, 1);
      lcd.write((uint8_t)7);
      break;
    case 7:
      lcd.setCursor(0, 0);
      lcd.print(">EXIT");
      lcd.setCursor(15, 0);
      lcd.write((uint8_t)0);  //up arrow
      break;
  }
}


void executeGameSettingsAction() {
  switch (subMenu) {
    case 1:
      chooseLightLevelMatrix();
      break;
    case 2:
      chooseLightLevelLcd();
      break;
    case 3:
      //handleSoundOptions();
      chooseSoundOption();
      break;
    case 4:
      chooseHighScoreOption();
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


//Start game
void startGame() {
  //reinitialize the values to default.
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
  lcd.createChar(0, life);
  exitGame = false;
  boolean copyMatrix = false;
  while (exitGame == false) {
    if (!digitalRead(joyStickBtn)) {//quick exit the game.
      exitGame = true;
    }
    if (copyMatrix == false) {
      generateMap();//verify the map has been generated 
      updateMatrix();
      copyMatrix = true;
    }
    for (int i = 0; i < lives; i++) {//place the lives on the lcd.
      lcd.setCursor(i, 0);
      lcd.write((uint8_t)0);
    }
    if (allowFasterBombs == true && selectedDifficulty == 1) {//if the player plays on the hardest mode easter egg.
      lcd.write((uint8_t)6);
    }
    lcd.setCursor(0, 1);
    lcd.print("Points");
    lcd.setCursor(8, 1);
    lcd.print(points);
    lcd.setCursor(12, 1);
    lcd.print(currentPlayerName);

    bool buttonPressed = digitalRead(pinSW);
    if (millis() - lastMoved >= moveInterval) {//how fast is the player moving,
      updatePositions();//update the position of the player, on the matrix
      if ((xLast != xPos || yLast != yPos) && exist == false && buttonPressed == LOW) { // if the player moved and he pressed the bomb button, and the bomb does not exit,
        tempMap[xLast][yLast] = 1;//set the last position on (create the bomb)
        lc.setLed(1, xLast, yLast, tempMap[xLast][yLast]);
        lastPositionSetTime = millis();//start a timmer fot he bomb
        exist = true;//set that it exists
        xBlink = xLast;//set bomb location for the blink fast function
        yBlink = yLast;
      }
      lastMoved = millis();//start the counter for lastMoved, 
    }
    //bomb starts blinking if it exists
    if (exist) {
      blinkFast(xBlink, yBlink);
    }

    if (exist && millis() - lastPositionSetTime > timeForBomb) {//if the bomb exists the timmer will start and once the timmer for the bomb is over
      tempMap[xBlink][yBlink] = 0;//remove the current bomb
      lc.setLed(1, xBlink, yBlink, tempMap[xBlink][yBlink]);

      if (xBlink > 0) {
        
        if (tempMap[xBlink - 1][yBlink] == 1) {
          points = points + 10 / selectedDifficulty;//add points
        }
        tempMap[xBlink - 1][yBlink] = 0;//destroy the walls
        lc.setLed(1, xBlink - 1, yBlink, tempMap[xBlink - 1][yBlink]);

        //you die if you stand too close to the bomb and you go back to spawn;
        if (xPos == xBlink - 1 && yBlink == yPos) {
          handlePlayerRespawn();//handle if the player is in the blast
        }
      }

      if (xBlink < (8 - 1)) {
        if (tempMap[xBlink + 1][yBlink] == 1) {
          points = points + 10 / selectedDifficulty;
        }
        tempMap[xBlink + 1][yBlink] = 0;
        lc.setLed(1, xBlink + 1, yBlink, tempMap[xBlink + 1][yBlink]);

        if (xPos == xBlink + 1 && yBlink == yPos) {
          handlePlayerRespawn();
        }
      }

      if (yBlink > 0) {
        if (tempMap[xBlink][yBlink - 1] == 1) {
          points = points + 10 / selectedDifficulty;
        }
        tempMap[xBlink][yBlink - 1] = 0;
        lc.setLed(1, xBlink, yBlink - 1, tempMap[xBlink][yBlink - 1]);

        if (xPos == xBlink && yBlink - 1 == yPos) {
          handlePlayerRespawn();
        }
      }

      if (yBlink < (8 - 1)) {
        if (tempMap[xBlink][yBlink + 1] == 1) {
          points = points + 10 / selectedDifficulty;
        }
        tempMap[xBlink][yBlink + 1] = 0;
        lc.setLed(1, xBlink, yBlink + 1, tempMap[xBlink][yBlink + 1]);

        if (xPos == xBlink && yBlink + 1 == yPos) {
          handlePlayerRespawn();
        }
      }

      exist = false;//reset the flag for the bomb
      if (allowSound) {//if the player allowed sound play the bomb sound
        playBombSound();
      }
      if (allowFasterBombs) {//extra difficulty setting for the player.
        points += 2;
      }
      updateMatrix();
    }

    if (matrixChanged) {
      updateMatrix();//update the matrix again for the bomb explotion
      matrixChanged = false;
    }

    blink(xPos, yPos);//blink the player current position

    if (lives == 0) {//if the player runs out of lives end the game and set highscores.
      exitGame = true;
      if (points > highScore3rd) {
        beatenHighScore = 3;
        if (points > highScore2nd) {
          beatenHighScore = 2;
          if (points > highScore1st) {
            beatenHighScore = 1;
          }
        }
      }
      if (beatenHighScore > 0) {
        lcd.clear();
        lcd.print(F("You beat the:"));
        lcd.setCursor(0, 1);

        char tempName[nameLength + 1];  // Temporary buffer for name

        if (beatenHighScore == 1 && !shown1st) {

          // Shift 2nd to 3rd
          highScore3rd = highScore2nd;
          loadHighScoreNameToEEPROM(highScore2ndNameAddress, tempName);
          saveHighScoreNameToEEPROM(highScore3rdNameAddress, tempName);

          // Shift 1st to 2nd
          highScore2nd = highScore1st;
          loadHighScoreNameToEEPROM(highScore1stNameAddress, tempName);
          saveHighScoreNameToEEPROM(highScore2ndNameAddress, tempName);

          // Update 1st high score
          highScore1st = points;
          saveHighScoreNameToEEPROM(highScore1stNameAddress, currentPlayerName);
          lcd.print(F("1st HIGHSCORE!"));
          shown1st = true;
        } else if (beatenHighScore == 2 && !shown2nd) {
          highScore3rd = highScore2nd;
          loadHighScoreNameToEEPROM(highScore2ndNameAddress, tempName);
          saveHighScoreNameToEEPROM(highScore3rdNameAddress, tempName);

          highScore2nd = points;
          saveHighScoreNameToEEPROM(highScore2ndNameAddress, currentPlayerName);
          lcd.print(F("2nd HIGHSCORE!"));
          shown2nd = true;
        } else if (beatenHighScore == 3 && !shown3rd) {
          highScore3rd = points;
          saveHighScoreNameToEEPROM(highScore3rdNameAddress, currentPlayerName);
          lcd.print(F("3rd HIGHSCORE!"));
          shown3rd = true;
        }
        delay(1000);
        lcd.clear();
        beatenHighScore = 0;
      }
      bool exitThisThing = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("You died"));
      if (points > highScore1st) {
        highScore1st = points;
        saveHighScoreNameToEEPROM(highScore1stNameAddress, currentPlayerName);
      } else if (points > highScore2nd && points < highScore1st) {
        highScore2nd = points;
        saveHighScoreNameToEEPROM(highScore2ndNameAddress, currentPlayerName);
      } else if (points > highScore3rd && points < highScore2nd) {
        highScore2nd = points;
        saveHighScoreNameToEEPROM(highScore3rdNameAddress, currentPlayerName);
      }
      delay(1000);
      lcd.clear();
      lcd.print(F("Score:"));
      lcd.setCursor(7, 0);
      lcd.print(points);
      delay(1000);
      copyMatrix = false;
      points = 0;
      while (exitThisThing == false) {
        while (!digitalRead(joyStickBtn)) {
          exitThisThing = true;
        }
      }
      lcd.clear();
    }
    //Here if the player finished a map, clear the map, advance the level counter, generate a new map.
    if (areAllLedsOff(xPos, yPos)) {
      copyMatrix = false;
      //redo highscores, no reason to do it on every bomb, you can just do it at the end of a level or if the player dies, if you exit the game it does not save the highscore
      if (points > highScore3rd) {
        beatenHighScore = 3;
        if (points > highScore2nd) {
          beatenHighScore = 2;
          if (points > highScore1st) {
            beatenHighScore = 1;
          }
        }
      }
      //only update if the player hit a highscore
      if (beatenHighScore > 0) {
        lcd.clear();
        lcd.print(F("You beat the:"));
        lcd.setCursor(0, 1);

        char tempName[nameLength + 1];

        if (beatenHighScore == 1 && !shown1st) {

          highScore3rd = highScore2nd;
          loadHighScoreNameToEEPROM(highScore2ndNameAddress, tempName);
          saveHighScoreNameToEEPROM(highScore3rdNameAddress, tempName);
          highScore2nd = highScore1st;
          loadHighScoreNameToEEPROM(highScore1stNameAddress, tempName);
          saveHighScoreNameToEEPROM(highScore2ndNameAddress, tempName);
          highScore1st = points;
          saveHighScoreNameToEEPROM(highScore1stNameAddress, currentPlayerName);
          lcd.print(F("1st HIGHSCORE!"));
          shown1st = true;
        } else if (beatenHighScore == 2 && !shown2nd) {
          highScore3rd = highScore2nd;
          loadHighScoreNameToEEPROM(highScore2ndNameAddress, tempName);
          saveHighScoreNameToEEPROM(highScore3rdNameAddress, tempName);

          highScore2nd = points;
          saveHighScoreNameToEEPROM(highScore2ndNameAddress, currentPlayerName);
          lcd.print(F("2nd HIGHSCORE!"));
          shown2nd = true;
        } else if (beatenHighScore == 3 && !shown3rd) {

          highScore3rd = points;
          saveHighScoreNameToEEPROM(highScore3rdNameAddress, currentPlayerName);
          lcd.print(F("3rd HIGHSCORE!"));
          shown3rd = true;
        }
        delay(1000);
        lcd.clear();
        beatenHighScore = 0;
      }


      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Finished map"));
      lcd.setCursor(13, 0);
      lcd.print(levels);
      delay(2000);
      lcd.clear();
      xPos = 0;
      yPos = 0;
      levels++;
      updateMatrix();
      if (levels == 4) {
        //finished all 3 levels;
        boolean exitThisThing = false;
        lcd.setCursor(0, 0);
        lcd.print(F("You won!"));
      if (points > highScore1st) {
        highScore1st = points;
        saveHighScoreNameToEEPROM(highScore1stNameAddress, currentPlayerName);
      } else if (points > highScore2nd && points < highScore1st) {
        highScore2nd = points;
        saveHighScoreNameToEEPROM(highScore2ndNameAddress, currentPlayerName);
      } else if (points > highScore3rd && points < highScore2nd) {
        highScore3rd = points;
        saveHighScoreNameToEEPROM(highScore3rdNameAddress, currentPlayerName);
      }
        lcd.setCursor(0, 1);
        lcd.print("Score:");
        lcd.setCursor(8, 1);
        lcd.print(points);
        delay(1000);
        while (exitThisThing == false) {
          while (!digitalRead(joyStickBtn)) {
            exitThisThing = true;
          }
        }
        exitGame = true;
        levels = 0;
        lives = selectedDifficulty;
        points = 0;
        copyMatrix = false;
        updateMatrix();
      }
    }
  }
  //at the end of the game update the values to EEPROM.
  EEPROM.write(8, highScore1st >> 8);
  EEPROM.write(9, highScore1st & 0xFF);
  EEPROM.write(10, highScore2nd >> 8);
  EEPROM.write(11, highScore2nd & 0xFF);
  EEPROM.write(12, highScore3rd >> 8);
  EEPROM.write(13, highScore3rd & 0xFF);
  lc.clearDisplay(0);
}

//Read the highscore from 2 eeprom adresses
int readHighScore(int address) {
  int highByte = EEPROM.read(address);
  int lowByte = EEPROM.read(address + 1);
  return (highByte << 8) | lowByte;
}
//if the player dies go back to spawn, lose points, reset bomb values and last values as well.
void handlePlayerRespawn() {
  yPos = 0;
  xPos = 0;
  lives--;
  points = points - 20;
  if (points < 0) {
    points = 0;
  }
  xLast = 1;
  yLast = 1;
  xBlink = 3;
  xBlink = 3;
  exist = false;
  lcd.clear();
}
//Function to generate the map based on the difficulty, if fasterbombs is on, also make the game more difficult as the player clears levels.
void generateMap() {
  int maxBlocks;//counter for how many blocks to place
  if (selectedDifficulty < 4) {
    maxBlocks = 20 + levels * 8;
    if (allowFasterBombs) {
      timeForBomb -= 500;  // i did this to add difficulty on the levels progressively
    }
  } else if (selectedDifficulty < 8 && selectedDifficulty > 3) {
    maxBlocks = 20 + levels * 5;
    if (allowFasterBombs) {
      timeForBomb -= 300;
    }

  } else {
    maxBlocks = 20 + levels;
    if (allowFasterBombs) {
      timeForBomb -= 150;
    }
  }

  //initialize map with zeros in case you played the tutorial before
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      tempMap[i][j] = 0;
    }
  }

  // Randomly place blocks
  int blocks = 0;
  while (blocks < maxBlocks) {
    int x = random(8);
    int y = random(8);
    if ((x != 0 || y != 1) && (x != 1 || y != 0) && tempMap[x][y] == 0) {  //ignore these position so any map can be finished
      tempMap[x][y] = 1;
      blocks++;
    }
  }
}

//Game Options, here the player selects between difficulty and fasterbombs, also has an exit it is a submenu of settings.
void handleGameOptions() {
  lcd.clear();
  subMenu = 5;
  exitMenu = false;
  displayGameOptions();

  while (!exitMenu) {
    int joyValue = analogRead(xPin);

    if (joyValue < minThreshold) {
      subMenu++;
      if (allowSound) {
        playMenuSound();
      }
      displayGameOptions();
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold)
        ;
    }

    if (joyValue > maxThreshold) {
      subMenu--;
      if (allowSound) {
        playMenuSound();
      }
      displayGameOptions();
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold)
        ;
    }

    if (!digitalRead(pinSW)) {
      executeGameMenuAction();
      delay(debounceDelay);
      displayGameOptions();
      delay(debounceDelay);
      while (!digitalRead(pinSW))
        ;
    }
  }
}
//Display on the lcd the setting for the game options
void displayGameOptions() {
  if (subMenu < 1) subMenu = 3;
  if (subMenu > 3 && subMenu != 5) {
    subMenu = 1;
  }
  if (subMenu > 5) subMenu = 1;

  lcd.clear();

  switch (subMenu) {
    case 1:
      lcd.print(F(">Game Mode"));
      lcd.setCursor(0, 1);
      lcd.print(F(" Faster Bombs"));
      lcd.setCursor(15, 1);
      lcd.write((uint8_t)7);
      break;
    //here i should make it lives, bomb time, ??
    case 2:
      lcd.print(" Game Mode");
      lcd.setCursor(0, 1);
      lcd.print(">Faster Bombs");
      lcd.setCursor(15, 1);
      lcd.write((uint8_t)7);
      break;
    case 3:
      lcd.print(F(">Exit"));
      lcd.setCursor(15, 0);
      lcd.write((uint8_t)0);  //up arrow
  }
}
//execute that said option
void executeGameMenuAction() {
  switch (subMenu) {
    case 1:
      chooseDifficulty();
      break;
    case 2:
      allowBombTime();
      break;
    case 3:
      exitMenu = true;
      break;
    case 5:
      lcd.clear();
      lcd.print("LOADING...");
      delay(400);
      subMenu = 1;
      break;
  }
}
//Here the player uses the up/down of the joystick to select difficulty level
void chooseDifficulty() {
  lcd.clear();
  lcd.print("1-Hard, 10-Easy");
  delay(1500);
  lcd.clear();
  lcd.print(F("Game Mode"));
  boolean exitThis = false;
  while (exitThis == false) {
    int joyValue = analogRead(xPin);
    if (!digitalRead(joyStickBtn)) { //only after the player presse the confirm button (the joystick button) will the option be saved.
      exitThis = true;
    }
    if (joyValue < minThreshold) {
      lcd.clear();
      selectedDifficulty++;
      if (selectedDifficulty > 10) {
        selectedDifficulty = 10;
      }
      lcd.setCursor(0, 0);
      if (selectedDifficulty < 4) {
        lcd.print(F("Game Mode-Hard"));
      } else if (selectedDifficulty > 3 && selectedDifficulty < 8) {
        lcd.print(F("Game Mode-Normal"));
      } else {
        lcd.print(F("Game Mode-Easy"));
      }

      lcd.setCursor(0, 1);
      lcd.print(selectedDifficulty);
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold)
        ;
    }

    if (joyValue > maxThreshold) {
      lcd.clear();
      selectedDifficulty--;
      if (selectedDifficulty < 1) {
        selectedDifficulty = 1;
      }
      lcd.setCursor(0, 0);
      if (selectedDifficulty < 4) {
        lcd.print(F("Game Mode-Hard  "));
      } else if (selectedDifficulty > 3 && selectedDifficulty < 8) {
        lcd.print(F("Game Mode-Normal"));
      } else {
        lcd.print(F("Game Mode-Easy  "));
      }
      lcd.setCursor(0, 1);
      lcd.print(selectedDifficulty);
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold)
        ;
    }
  }
  EEPROM.write(0, selectedDifficulty);

  delay(500);
}

//The same as the chooseDifficulty menu, all the menues where you have to select a value are done the same, here is just yes/no compared to selecting a value from a range.
void allowBombTime() {
  lcd.clear();
  lcd.print(F("Fast bombs?:"));
  boolean exitThis = false;
  while (!exitThis) {
    int joyValue = analogRead(xPin);
    if (joyValue < minThreshold) {
      allowFasterBombs = true;
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold)
        ;
    } else if (joyValue > maxThreshold) {
      allowFasterBombs = false;
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold)
        ;
    }

    lcd.setCursor(0, 1);
    if (allowFasterBombs) {
      lcd.print(F("Yes"));
    } else {
      lcd.print(F("No "));
    }

    // Exit on button press
    if (!digitalRead(joyStickBtn)) {
      exitThis = true;
    }
  }
  EEPROM.write(39, allowFasterBombs);
  delay(150);
}
void executeMatrixMenuAction() {
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
//The same as the chooseDifficulty menu, all the menues where you have to select a value are done the same.
void chooseLightLevelMatrix() {
  lcd.clear();
  lcd.print(F("Set Brightness"));
  boolean exitThis = false;
  while (exitThis == false) {
    int joyValue = analogRead(xPin);
    if (!digitalRead(joyStickBtn)) {
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
      while (analogRead(xPin) < minThreshold)
        ;
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
      while (analogRead(xPin) > maxThreshold)
        ;
    }
    lc.setIntensity(0, matrixBrightness);
    matrixLight();
  }
  lc.setIntensity(0, matrixBrightness);
  delay(500);
  lc.clearDisplay(0);
}
//Set on the matrix.
void matrixLight() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, 1);
    }
  }
}

//The same as the chooseDifficulty menu, all the menues where you have to select a value are done the same.

void chooseLightLevelLcd() {
  lcd.clear();
  lcd.print(F("Set Brightness:"));
  boolean exitThis = false;
  while (exitThis == false) {
    int joyValue = analogRead(xPin);
    boolean moved = false;
    if (!digitalRead(joyStickBtn)) {
      exitThis = true;
    }

    if (joyValue < minThreshold && moved == false) {
      lcdBrightness += 20;
      if (lcdBrightness > 200) {
        lcdBrightness = 200;
      }
      lcd.setCursor(0, 0);
      lcd.print(F("Set Brightness"));
      lcd.setCursor(0, 1);
      lcd.print(lcdBrightness);
      delay(debounceDelay);
      while (analogRead(xPin) < minThreshold)
        ;
      moved = true;
    }

    if (joyValue > maxThreshold && moved == false) {
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
      while (analogRead(xPin) > maxThreshold)
        ;
      moved = true;
    }
    analogWrite(Apin, lcdBrightness);
  }
  EEPROM.write(2, lcdBrightness);
  analogWrite(Apin, lcdBrightness);

  delay(500);
}

//Sound off/on:
void handleSoundOptions() {
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
      while (analogRead(xPin) < minThreshold)
        ;
    }

    if (joyValue > maxThreshold) {
      subMenu--;
      displaySoundOptions();
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold)
        ;
    }

    if (!digitalRead(pinSW)) {
      executeSoundMenuAction();
      delay(debounceDelay);
      displaySoundOptions();
      delay(debounceDelay);
      while (!digitalRead(pinSW))
        ;
    }
  }
}
//The display for the sound option.
void displaySoundOptions() {
  if (subMenu < 1) subMenu = 2;
  if (subMenu > 2 && subMenu != 4) {
    subMenu = 1;
  }
  if (subMenu > 4) subMenu = 1;

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

void executeSoundMenuAction() {
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
      subMenu = 1;
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
      while (analogRead(xPin) < minThreshold)
        ;
    } else if (joyValue > maxThreshold) {
      allowSound = false;
      delay(debounceDelay);
      while (analogRead(xPin) > maxThreshold)
        ;
    }

    lcd.setCursor(0, 1);
    if (allowSound) {
      lcd.print(F("Yes"));
    } else {
      lcd.print(F("No "));
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

//Reset the highscores, add 3 values this was done for testing how the highscores work.
void chooseHighScoreOption() {
  lcd.clear();
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




//The player uses up/down to change the current letter position, and right/left to change the value of the letter. you start on position 1, go up -> position 2 -> go right form A -> B;
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

//Show the credits made using delay and scroll.
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
  lcd.setCursor(0, 0);
  lcd.print(F("Hope you like it!"));
  delay(1500);
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
  tone(buzzerPin, 800, 350);  //bomb sound
}

void playMenuSound() {
  tone(buzzerPin, 500, 200);
}
//update the current map with the new player location, this can happen only in one direction, (up/down or left/right)
void updatePositions() {
  bool hasMoved = false;

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
//update the current map
  if (tempMap[newXPos][newYPos] == 0 && hasMoved) {
    matrixChanged = true;
    tempMap[xPos][yPos] = 0;
    tempMap[newXPos][newYPos] = 1;
    xLast = xPos; //change the last positions
    yLast = yPos;
    xPos = newXPos;
    yPos = newYPos;
  } else {
    hasMoved = false;
  }
}
//Blink for the player i turn the current position on and off, on a specified interval
void blink(byte x, byte y) {
  static unsigned long lastBlinkTime = 0;
  static bool isOn = true;
  static unsigned blinkInterval = 400;
  if (millis() - lastBlinkTime >= blinkInterval) {
    isOn = !isOn;
    lc.setLed(0, x, y, isOn);

    lastBlinkTime = millis();
  }
}
//Blink for the bomb
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
//Function to check if the map is finished (the player destroyed all the walls, and ignore his currentPosition)
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

//If the player enetres the showHighScore menu present all the highscores in order
void showHighScores() {
  char nameBuffer[nameLength + 1];

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

//Start the tutorial
void showHowToPlay() {
  //classic video game teach fundementals
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Move Up 3 times");
  int count = 0;
  xPos = 4;
  yPos = 4;
  generateTutorial();
  updateMatrix();
//Wait for the player to move in the said direction, the player in the movment selection cannot place bombs
  while (count < 3) {

    if (millis() - lastMoved >= moveInterval) {
      xLast = xPos;
      yLast = yPos;
      updatePositions(); //once the player moved update the position
      if (xLast < xPos) { //if the player moved up
        count++;
      }
      lastMoved = millis();
    }

    if (matrixChanged) {
      updateMatrix();
      matrixChanged = false;
    }
    blink(xPos, yPos);
  }
  lcd.clear();
  lcd.print("Move Down 3 times");
  count = 0;
  while (count < 3) {

    if (millis() - lastMoved >= moveInterval) {
      xLast = xPos;
      yLast = yPos;
      updatePositions(); 
      if (xLast > xPos) {  //if the player moved down
        count++;
      }
      lastMoved = millis();
    }

    if (matrixChanged) {
      updateMatrix();
      matrixChanged = false;
    }
    blink(xPos, yPos);
  }
  lcd.clear();
  lcd.print("Move < 3 times");
  count = 0;

  while (count < 3) {
    if (millis() - lastMoved >= moveInterval) {
      xLast = xPos;
      yLast = yPos;
      updatePositions();
      if (yLast < yPos) { //if the player moved left
        count++;
      }
      lastMoved = millis();
    }

    if (matrixChanged) {
      updateMatrix();
      matrixChanged = false;
    }
    blink(xPos, yPos);
  }
  lcd.clear();
  lcd.print("Move > 3 times");

  count = 0;
  while (count < 3) {
    if (millis() - lastMoved >= moveInterval) {
      xLast = xPos;
      yLast = yPos;
      updatePositions();
      if (yLast > yPos) { //if the player moved right.
        count++;
      }
      lastMoved = millis();
    }

    if (matrixChanged) {
      updateMatrix();
      matrixChanged = false;
    }
    blink(xPos, yPos);
  }

  lcd.clear();
  lcd.print("Use btn for bombs");
  delay(1250);
  lcd.clear();
  lcd.print("Blow the walls!");
  count = 0;


//Here the player has to destroy the walls in order to advance.
  while (count < 3) {
    bool buttonPressed = digitalRead(pinSW);
    if (millis() - lastMoved >= moveInterval) {
      xLast = xPos;
      yLast = yPos;
      updatePositions();
      if ((xLast != xPos || yLast != yPos) && exist == false && buttonPressed == LOW) {
        tempMap[xLast][yLast] = 1;
        lc.setLed(1, xLast, yLast, tempMap[xLast][yLast]);
        lastPositionSetTime = millis();
        exist = true;
        xBlink = xLast;
        yBlink = yLast;
      }
      lastMoved = millis();
    }
    if (exist) {
      blinkFast(xBlink, yBlink);
    }

    if (exist && millis() - lastPositionSetTime > timeForBomb) {
      tempMap[xBlink][yBlink] = 0;
      lc.setLed(1, xBlink, yBlink, tempMap[xBlink][yBlink]);

      if (xBlink > 0) {
        if (tempMap[xBlink - 1][yBlink] == 1) {
          count++;
        }
        tempMap[xBlink - 1][yBlink] = 0;
        lc.setLed(1, xBlink - 1, yBlink, tempMap[xBlink - 1][yBlink]);

        if (xPos == xBlink - 1 && yBlink == yPos) { //if the player blows himself up go back to spawn
          xPos = 4;
          yPos = 4;
          count--;
          lcd.clear();
          lcd.print("Bombs hurt!");
          delay(1500);
          lcd.clear();
          lcd.print("Destroy the walls!");
        }
      }

      if (xBlink < (8 - 1)) {
        if (tempMap[xBlink + 1][yBlink] == 1) {
          count++; //if the player blows a wall update counter.
        }
        tempMap[xBlink + 1][yBlink] = 0;
        lc.setLed(1, xBlink + 1, yBlink, tempMap[xBlink + 1][yBlink]);

        if (xPos == xBlink + 1 && yBlink == yPos) { //if the player blows himself up go back to spawn
          xPos = 4;
          yPos = 4;
          count--;
          lcd.clear();
          lcd.print("Bombs hurt!");
          delay(1500);
          lcd.clear();
          lcd.print("Destroy the walls!");
        }
      }

      if (yBlink > 0) {
        if (tempMap[xBlink][yBlink - 1] == 1) {
          count++;
        }
        tempMap[xBlink][yBlink - 1] = 0;
        lc.setLed(1, xBlink, yBlink - 1, tempMap[xBlink][yBlink - 1]);

        if (xPos == xBlink && yBlink - 1 == yPos) { //if the player blows himself up go back to spawn
          xPos = 4;
          yPos = 4;
          count--;
          lcd.clear();
          lcd.print("Bombs hurt!");
          delay(1500);
          lcd.clear();
          lcd.print("Destroy the walls!");
        }
      }

      if (yBlink < (8 - 1)) {
        if (tempMap[xBlink][yBlink + 1] == 1) {
          count++;
        }
        tempMap[xBlink][yBlink + 1] = 0;
        lc.setLed(1, xBlink, yBlink + 1, tempMap[xBlink][yBlink + 1]);

        if (xPos == xBlink && yBlink + 1 == yPos) { //if the player blows himself up go back to spawn
          xPos = 4;
          yPos = 4;
          count--;
          lcd.clear();
          lcd.print("Bombs hurt!");
          delay(1500);
          lcd.clear();
          lcd.print("Destroy walls!");
        }
      }

      exist = false;
      if (allowSound) {
        playBombSound();
      }
      updateMatrix();//update after the bomb exploded.
    }

    if (matrixChanged) {
      updateMatrix();
      matrixChanged = false;
    }
    blink(xPos, yPos);
  }

  // Final completion message
  lcd.clear();
  lc.clearDisplay(0);
  lcd.print("Training Complete!");
  delay(1500);
  lcd.clear();
}
//Generate tutorial map, i set all the values to 0 and add the corners
void generateTutorial() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      tempMap[i][j] = 0;
    }
  }  // reset in case you start play and then you go into tutorial
  tempMap[7][7] = 1;
  tempMap[0][0] = 1;
  tempMap[7][0] = 1;
  tempMap[0][7] = 1;
}
// Save the player's name to EEPROM

void saveNameToEEPROM() {
  for (int i = 0; i < 3; ++i) {
    EEPROM.write(20 + i, currentPlayerName[i]);
  }
}
// Load the player's name from EEPROM
void loadNameFromEEPROM() {
  for (int i = 0; i < 3; ++i) {
    currentPlayerName[i] = EEPROM.read(20 + i);
  }
  currentPlayerName[3] = '\0';
}

  // Save a high score name from the specified EEPROM address
void saveHighScoreNameToEEPROM(int address, const char* name) {
  for (int i = 0; i < nameLength; ++i) {
    EEPROM.write(address + i, name[i]);
  }
}
// Read a high score name from the specified EEPROM address
void loadHighScoreNameToEEPROM(int address, char* name) {
  for (int i = 0; i < nameLength; ++i) {
    name[i] = EEPROM.read(address + i);
  }
  name[nameLength] = '\0';
}
