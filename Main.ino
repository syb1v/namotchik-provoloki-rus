#include "header.h"
#include "keypad.h"
#include "lcd.h"
#include <math.h>

double D;
double L;
double l;
double A;
const double MC = .00000125663706;
double T;
double lmm;

int turns;
int theta;
int guid_distance;
char unit = 'U';
bool windatangle = false;
bool needtocalc = true;
bool ableangle = false;
float wiremm = 0.2; // Default

float spoolmuilpyler;
int speed_cruise = 40; // Default
int speed_start = 50;  // Default

void home() {
  digitalWrite(DirPinTwo, HIGH);
  lcd_clear();
  lcd_print(F("DOMOI..."));
  Serial.println("Homing started. Waiting for switchPin LOW...");

  while (digitalRead(switchPin) == HIGH) { // Ждем пока сработает концевик
    digitalWrite(StepPinTwo, HIGH);
    delayMicroseconds(250);
    digitalWrite(StepPinTwo, LOW);
    delayMicroseconds(250);

    // проверка на зависание
    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c == 'F' || c == 'x')
        break;
    }
  }
  lcd_clear();
  lcd_print("DOMA");
  Serial.println("Homing Complete!");
  delay(2000);
  digitalWrite(DirPinTwo, LOW);
}

// Функция для одного шага с проверкой экстренной остановки
// Возвращает true если стоп, false если продолжаем
bool doStep(int delay_us) {
  digitalWrite(StepPinOne, HIGH);
  delayMicroseconds(delay_us);
  digitalWrite(StepPinOne, LOW);
  delayMicroseconds(delay_us);

  if (scanKeys() == 'F')
    return true;
  return false;
}

void windTheta(int turns, float D) {
  float theta_rad = (theta * (3.1459 / 180));
  int length = (D * tan(theta_rad)) * stepsPerMM;
  int k = 0, j = 0, h = 0;
  long a = 0;
  int ratio = (stepsPerRotation / length);
  long steps = (stepsPerRotation * turns);
  for (long i = 0; i < steps; i++) {
    j++;
    h++;
    byte stepdelay = speed_start;
    if (i == (slowStart * stepsPerRotation))
      stepdelay = speed_cruise;
    if (i == (steps - (SlowEnd * stepsPerRotation)))
      stepdelay = speed_start;

    if (j >= stepsPerRotation) {
      lcd_clear();
      lcd_print(String(F("TSEL: ")) + String(turns));
      setcursor(1, 0);
      k++;
      j = 0;
      lcd_print(String(F("FACT: ")) + String(k));
    }

    // Шаг и проверка остановки
    if (doStep(stepdelay)) {
      lcd_clear();
      lcd_print(F("STOP!"));
      delay(1000);
      break;
    }

    if (h == ratio) {
      digitalWrite(StepPinTwo, HIGH);
      delayMicroseconds(stepdelay);
      digitalWrite(StepPinTwo, LOW);
      delayMicroseconds(stepdelay);
      h = 0;
      a++;
      if (a == stepsPerMM * lmm) {
        a = 0;
        if (digitalRead(DirPinTwo) == HIGH)
          digitalWrite(DirPinTwo, LOW);
        else
          digitalWrite(DirPinTwo, HIGH);
      }
    }
  }
  lcd_clear();
  lcd_print(F("finished"));
  delay(2000);
  lcd_clear();
  setup();
}

void wind(int turns) {
  int j = 0;
  int k = 0;
  long a = 0;
  int spool_steps = spoolmuilpyler * stepsPerMM;
  int ratio = (stepsPerRotation / spool_steps);
  long h = 0;
  long steps = (stepsPerRotation)*turns;
  long i = 0;
  while (i != steps) {
    j++;
    h++;
    byte stepdelay = speed_start;
    if (i == (slowStart * stepsPerRotation))
      stepdelay = speed_cruise;
    if (i == (steps - (SlowEnd * stepsPerRotation)))
      stepdelay = speed_start;

    if (j >= stepsPerRotation) {
      lcd_clear();
      lcd_print(String(F("TSEL: ")) + String(turns));
      setcursor(1, 0);
      k++;
      j = 0;
      lcd_print(String(F("FACT: ")) + String(k));
    }

    // Шаг и проверка остановки
    if (doStep(stepdelay)) {
      lcd_clear();
      lcd_print(F("STOP!"));
      delay(1000);
      break;
    }

    if (h >= ratio) {
      a++;
      digitalWrite(StepPinTwo, HIGH);
      delayMicroseconds(stepdelay);
      digitalWrite(StepPinTwo, LOW);
      delayMicroseconds(stepdelay);
      if (a == stepsPerMM * lmm) {
        a = 0;
        if (digitalRead(DirPinTwo) == HIGH)
          digitalWrite(DirPinTwo, LOW);
        else
          digitalWrite(DirPinTwo, HIGH);
      }
      h = 0;
    }
    i++;
  }
  lcd_clear();
  lcd_print(F("GOTOBO "));
  delay(2000);
  lcd_clear();
  setup();
}

void calculate() {
  switch (unit) {
  case 'M':
    L *= .001;
    break;
  case 'U':
    L *= .000001;
    break;
  case 'N':
    L *= .000000001;
    break;
  default:
    break;
  }

  // Испоьзуем пользовательские размеры
  // l и D уже в метрах из getKeyinput
  A = ((D / 2) * (D / 2)) * 3.14159;

  // Установка диаметра провода
  spoolmuilpyler = wiremm;

  if (needtocalc == true) {
    T = sqrt((L * l) / (A * MC));
    T = int(T);
    lcd_print(String(F("TSEL: ")) + String(T));
    turns = T;
  }
}

void getKeyinput() {
  char key;
  int index;
  String respL = "";
  bool i = true;
  lcd_clear();
  lcd_print(F("REZHIM RABOTY:"));
  setcursor(1, 0);
  lcd_print(F("A:HENRY D:VITKI"));
  delay(200);
  key = waitkey();
  if (key == 'A') {
    needtocalc = true;
    ableangle = false;
    while (i == true) {
      lcd_clear();
      lcd_print(F("INDUKTIVNOST?"));
      setcursor(1, 0);
      lcd_print(String(F("L=")) + respL + String(unit) + String(F("H ")));
      key = waitkey();
      switch (key) {
      case 'E':
        i = false;
        L = respL.toDouble();
        break;
      case 'A':
        unit = 'M';
        break;
      case 'B':
        unit = 'U';
        break;
      case 'C':
        unit = 'N';
        break;
      case 'F':
        index = respL.length() - 1;
        respL.remove(index);
        break;
      default:
        respL += key;
      }
    }
  } else {
    needtocalc = false;
    ableangle = true;
    String input_turns = "";
    while (i == true) {
      lcd_clear();
      delay(200);
      lcd_print(F("KOL-VO VITKOV:"));
      setcursor(1, 0);
      lcd_print(input_turns);
      key = waitkey();
      switch (key) {
      case 'F':
        index = input_turns.length() - 1;
        input_turns.remove(index);
        break;
      case 'E':
        i = false;
        turns = input_turns.toInt();
        break;
      default:
        input_turns += key;
        break;
      }
    }
  }

  // Запрос размеров (Всегда)
  lcd_clear();
  String input_length;
  String input_diameter;

  i = true;
  while (i == true) {
    lcd_clear();
    lcd_print(F("DLINA NAMOTKI:"));
    setcursor(1, 0);
    lcd_print(input_length + String(F(" MM")));
    key = waitkey();
    switch (key) {
    case 'E':
      i = false;
      lmm = input_length.toDouble(); // мм для намотчика
      l = (lmm / 1000);              // метры для расчетов
      break;
    case 'F':
      index = input_length.length() - 1;
      input_length.remove(index);
      break;
    case 'B':
      input_length += '.';
      break;
    default:
      input_length += key;
      break;
    }
  }

  i = true;
  while (i == true) {
    lcd_clear();
    lcd_print(F("DIAMETR KARKASA:"));
    setcursor(1, 0);
    lcd_print(input_diameter + String(F(" MM")));
    key = waitkey();
    switch (key) {
    case 'E':
      i = false;
      D = ((input_diameter.toDouble()) / 1000);
      break;
    case 'F':
      index = input_diameter.length() - 1;
      input_diameter.remove(index);
      break;
    case 'B':
      input_diameter += '.';
      break;
    default:
      input_diameter += key;
      break;
    }
  }

  if (ableangle == true) {
    lcd_clear();
    lcd_print(F("TIP NAMOTKI?"));
    setcursor(1, 0);
    lcd_print(F("A:UGOL D:OBYCHNA"));
    key = waitkey();
    if (key == 'A')
      windatangle = true;
    else
      windatangle = false;
    delay(200);
    lcd_clear();
    if (windatangle == true) {
      String input_theta = "";
      i = true;
      while (i == true) {
        lcd_clear();
        lcd_print(F("UGOL CEL: "));
        setcursor(1, 0);
        lcd_print(String(F("GRADUSI: ")) + input_theta);
        key = waitkey();
        switch (key) {
        case 'F':
          index = input_theta.length() - 1;
          input_theta.remove(index);
          break;
        case 'E':
          i = false;
          theta = input_theta.toInt();
          break;
        default:
          input_theta += key;
          break;
        }
      }
      theta = input_theta.toInt();
      i = true;
      String input_distance;
      while (i == true) {
        lcd_clear();
        lcd_print(F("VVEDITE D: "));
        lcd_print(input_distance);
        key = waitkey();
        switch (key) {
        case 'F':
          index = input_distance.length() - 1;
          input_distance.remove(index);
          break;
        case 'E':
          i = false;
          guid_distance = input_distance.toInt();
          break;
        default:
          input_distance += key;
          break;
        }
      }
    }
  }

  i = true;
  String input_wire = "";
  while (i == true) {
    lcd_clear();
    lcd_print(F("DIAMETR PROVODA:"));
    setcursor(1, 0);
    lcd_print(input_wire + String(F(" MM")));
    key = waitkey();
    switch (key) {
    case 'F':
      index = input_wire.length() - 1;
      input_wire.remove(index);
      break;
    case 'E':
      i = false;
      wiremm = input_wire.toDouble();
      break;
    case 'B':
      input_wire += '.';
      break;
    default:
      input_wire += key;
      break;
    }
  }

  i = true;
  while (i == true) {
    lcd_clear();
    lcd_print(F("SKOROST' (1-3):"));
    setcursor(1, 0);
    lcd_print(F("1:LO 2:MID 3:HI"));
    key = waitkey();
    switch (key) {
    case '1': // Slow / Медленная
      speed_cruise = 50;
      speed_start = 60;
      i = false;
      break;
    case '2': // Medium (Default) / Средняя
      speed_cruise = 25;
      speed_start = 35;
      i = false;
      break;
    case '3': // High (Fast) / Высокая
      speed_cruise = 10;
      speed_start = 20;
      i = false;
      break;
    }
  }
  lcd_clear();
}

void setup() {
  Serial.begin(9600);
  lcd_initialize();
  keypad_init();
  Serial.println("TTR Coil Winder");
  lcd_print(F("NAMOTCHIK PROVOLOKI"));
  pinMode(StepPinOne, OUTPUT);
  pinMode(DirPinOne, OUTPUT);
  pinMode(StepPinTwo, OUTPUT);
  pinMode(DirPinTwo, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  delay(1000);
  lcd_clear();
  getKeyinput();
  home();
  calculate();
  if (windatangle == true) {
    windTheta(turns, guid_distance);
  } else {
    wind(turns);
  }
}

void loop() {}
