#include "ComwithGY25.h"
#include "Display.h"

//#define Serial1_DEBUG 1
#define POS_DEBUG 1
//#define DATA_OUTPUT 1

double x = 0;//涓偣浣嶇疆提供给过过过
double y = 0;
double p = 0;

long int x_step = 0;
long int y_step = 0;

long int x_step_total = 0;
long int y_step_total = 0;

//鎺ョ嚎淇℃伅
static const int INT0A = PB0;//INT0
static const int INT0B = PB1;
static const int INT1A = PA6;//INT1
static const int INT1B = PA7;

//鐗╃悊淇℃伅
const double wheel_d = 38;//杞洿寰?38mm
const int x_line = 1000;
const int y_line = 1000;

//鐢ㄤ簬涓插彛杈撳嚭
double _x = 0;
double _y = 0;

double x2 = 0;
double y2 = 0;
int a = 0;

void blinkX();
void blinkY();

//void POS_begin();
//void POS_clear();
//void POS_refresh();

long int _millis = 0;

// The setup() function runs once each time the micro-controller starts
void setup()
{
  GY25.begin();
  dp.begin();
  POS_begin();
  Serial1.begin(115200);
}
// Add the main program code into the continuous loop() function
void loop()
{
  GY25.refresh();

  if (millis() - _millis >= 5)
  {
    POS_refresh();

    dp.displayXYP(x, y, p);

#ifdef DATA_OUTPUT
    Serial1.print("x:");
    Serial1.print(x);
    Serial1.print("y:");
    Serial1.print(y);
    Serial1.print("p:");
    Serial1.print(p);
#endif

#ifdef Serial_DEBUG
    Serial1.print("X_step:");
    Serial1.print(x_step_total);
    Serial1.print("Y_step:");
    Serial1.print(y_step_total);
#endif

    Serial1.println();

    _millis = millis();
  }
}

void blinkX()
{
  int xstate = digitalRead(INT0B);
  if (xstate == HIGH)
  {
    x_step++;
    x_step_total ++;
  }
  else
  {
    x_step--;
    x_step_total --;
  }
}

void blinkY()
{
  int ystate = digitalRead(INT1B);
  if (ystate == HIGH)
  {
    y_step++;
    y_step_total ++;
  }
  else
  {
    y_step--;
    y_step_total --;
  }
}

void POS_begin()
{
  pinMode(INT0A, INPUT_PULLUP);
  pinMode(INT1A, INPUT);

  pinMode(INT0B, INPUT_PULLUP);
  pinMode(INT1B, INPUT);

  attachInterrupt(digitalPinToInterrupt(INT0A), blinkX, FALLING);
  attachInterrupt(digitalPinToInterrupt(INT1A), blinkY, FALLING);
}

void POS_clear()
{
  x_step = 0;
  y_step = 0;

  x = 0;
  y = 0;
  p = 0;
}

void POS_refresh()
{
  //缁熻鍒锋柊鏃堕棿鍐? fff国家

  _x = double(x_step) / x_line * PI * wheel_d;
  x_step = 0;//琛岃蛋鐨勮窛绂? 鍗曚綅mm锛?
  _y = double(y_step) / y_line * PI * wheel_d; //琛岃蛋鐨勮窛绂? 鍗曚綅mm锛?
  y_step = 0;
  p = GY25.YPR[0] / 180.00 * PI;

  //鍒樺睍楣忕殑绠楁硶
  x2 += (_y) * sin(p + PI / 4) - (_x) * cos(p + PI / 4) ;//
  y2 += (_y) * cos(p + PI / 4) + (_x) * sin(p + PI / 4);

  x = x2;
  y = y2;
  p = p / PI * 180;

#ifdef POS_DEBUG
  Serial1.print("x_step_total:");
  Serial1.print(x_step_total);
  Serial1.print("y_step_total:");
  Serial1.print(y_step_total);

  Serial1.println();
#endif
}

void Serial1Event()
{
  if (Serial1.available() > 0)
  {
    String str = Serial1.readStringUntil('\n');
    str.toLowerCase();
    str = str.substring(0, str.length() - 1);
    //Serial1.println(str);
    //Serial1.println(str.length());

    if (str == "clear") {
      GY25.correctYaw();
      delay(3000);
      GY25.autoMode();
    }
    else
    {
      //Serial1.println("1");
    }
    if (str == "clearall") {
      GY25.correctYaw();
      delay(3000);
      GY25.autoMode();
      x2 = 0;
      y2 = 0;
    }
    else
    {
      //Serial1.println("2");
    }
  }
}