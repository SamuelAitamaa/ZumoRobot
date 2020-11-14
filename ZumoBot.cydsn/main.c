/**
* @mainpage ZumoBot Project
* @brief    You can make your own ZumoBot with various sensors.
* @details  <br><br>
    <p>
    <B>General</B><br>
    You will use Pololu Zumo Shields for your robot project with 
    CY8CKIT-059(PSoC 5LP) from Cypress semiconductor. This library has basic 
    methods of various sensors and communications so that you can make what you 
    want with them. <br> 
    <br><br>
    </p>
    
    <p>
    <B>Sensors</B><br>
    &nbsp;Included: <br>
        &nbsp;&nbsp;&nbsp;&nbsp;LSM303D: Accelerometer & Magnetometer<br>
        &nbsp;&nbsp;&nbsp;&nbsp;L3GD20H: Gyroscope<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Reflectance sensor<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Motors
    &nbsp;Wii nunchuck<br>
    &nbsp;TSOP-2236: IR Receiver<br>
    &nbsp;HC-SR04: Ultrasonic sensor<br>
    &nbsp;APDS-9301: Ambient light sensor<br>
    &nbsp;IR LED <br><br><br>
    </p>
    
    <p>
    <B>Communication</B><br>
    I2C, UART, Serial<br>
    </p>
*/

#include <project.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "LSM303D.h"
#include "IR.h"
#include "Beep.h"
#include "mqtt_sender.h"
#include <time.h>
#include <sys/time.h>
#include "serial1.h"
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846;

/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. 
 **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

/*  Smart IoT-devices        TX00DJ98-3009
 __  __ ___ _____ ____  _____ ___  _____ _    _  ___
|  \/  | _ |_   _|  _ \/  _  \ _ \/  _  \ |  | |/ _ \
| \__/ | _|  | | | |/ /| |_| |  _/| |_| | |__| |  _  |
|_|  |_|___| |_| |_|\_\\_____/_|  \_____/____|_|_| |_|
 _____ _  __ _  __ _  ___  ___  __    ____
|_   _| \/ /| |/ /_ |/ _ \/ __\/_ |__|    \
  | |  |  | |   | | |\__  \__ \ | |__| |\ |
  |_| /_/\_\|_|\_\|_|/___/____/ |_|  |____/
 _____ ___  ___  __  __       __  __      __
|_   _| _ |/ _ \|  \/  |     / / / /   _/   /
  | | | _||  _  | \__/ |  __/ /_/ /_ /__   /
  |_| |___|_| |_|_|  |_| /_  __  __/   /  /
                        __/ /_/ /_    /  /
    Samuel Aitamaa     /_  __  __/   /  /
     Jere Lampola       / / / /     /  /
    Tuomas Rajala      /_/ /_/     /__/

MORE OR LESS PROUDLY PRESENTS THEIR CODE */

/* SOME SYNTAX REMINDERS

Tab is equal to 2 spaces.

Multi-line comments should be limited to 80 characters in width, end-of-line
comments may exceed to 100 characters, but preferentially stay within 80 char
limit.

Curly brackets open after function/loop/etc. declaration on the same line, and
end at Col same as the first character on the line it was started at.

Function and variable names are written in camelCase.

Arithmetics are written "loosely", with empty space on each side, example:
"d = a + b * c" instead of "d=a+b*c". "d = a + b*c" is also ok for clarity.
*/
void aw2_morseCode_short();
void aw2_morseCode_shorter();
void aw2_morseCode_long();
void aw2_speedTest();
void aw2_batteryLow_short();
void aw2_batteryLow_long();

void aw3_sensorlessTrack();
void aw3_obstacleUltra();
void aw3_bumpAndTurn();
void aw3_bumpAndTurnZeroDelay();

void aw4_stopAtLines();
void aw4_stopAtLines_short();
void aw4_turnAtIntersections();
void aw4_turnAtIntersections_short();
void aw4_turnAtIntersections_short2();
void aw4_followLine();

void aw5_reportTimeMQTT();
void aw5_reportUltraAndTurn();
void aw5_reportUltraAndTurn_short_silly();
void aw5_reportIntersection();
void aw5_reportIntersection_short_silly();

void rp_sumoWrestling();
void rp_lineFollower();
void rp_maze();

void blinkMorse();
void ledOnFor();
float ADCtoBatteryRatio(float R1,float R2,float V_ref,int ADC_res);
void turn90deg();
void tankTurn();
void playMusic();
bool overLine();

bool allSensorsBlack();
bool anySensorBlack();
int blackSensors();

bool inIntersection();
int followLine();
void moveToNextLine();
void moveToStartLine();

TickType_t startWrestling();
int detectZumoHit();
void reportZumoHit();
void wrestlingEnd();
void chaseObject();
float turnToCenter();
void goToCenter();
TickType_t respondToHit();

void startComponents();
void moveToStartLineAndWait();
TickType_t startLineFollowing();
void moveToFinishLine();
void reportMQTT();

void accTest();
void refTest();
void irTest();
void mqttTest();
void irMsgTest();

void zmain(void){
  /*
  ========== ASSIGNMENTS =========
  ------------ Week 2 ------------
  aw2_morseCode_short();
  aw2_morseCode_shorter();
  aw2_morseCode_long();
  aw2_speedTest();
  aw2_batteryLow_short();
  aw2_batteryLow_long();

  ------------ Week 3 ------------
  aw3_sensorlessTrack();
  aw3_obstacleUltra();
  aw3_bumpAndTurn();
  aw3_bumpAndTurnZeroDelay();
  
  ------------ Week 4 ------------
  aw4_stopAtLines();
  aw4_stopAtLines_short();
  aw4_turnAtIntersections();
  aw4_turnAtIntersections_short();
  aw4_followLine();

  ------------ Week 5 ------------
  aw5_reportTimeMQTT();
  aw5_reportUltraAndTurn();
  aw5_reportUltraAndTurn_short_silly();
  aw5_reportIntersection();
  aw5_reportIntersection_short_silly();

  ======== Robot projects ========
  rp_sumoWrestling();
  rp_lineFollower();
  rp_maze();
    
  =========== Testing: ===========
  accTest();
  refTest();
  irTest();
  mqttTest();
  irMsgTest();
  */
    
  // COPY A FUNCTION YOU WANT TO BE EXECUTED (FROM THE LIST ABOVE) UNDER HERE:
  //rp_lineFollower();
  // rp_sumoWrestling();
  // chaseObject();
  goToCenter();

 /*
  // == BUTTON PRESS TESTING ==
  TickType_t start = xTaskGetTickCount();
  struct accData_ data;
  LSM303D_Start();

  while(true){
    while(SW1_Read()){};
    print_mqtt("Zumo035/start","",1);
    vTaskDelay(500);
    motor_start();
    motor_forward(50,0);
    while(true){//xTaskGetTickCount() - start < 10000
      LSM303D_Read_Acc(&data);
      if(data.accX < -2000){
        break;
      }
      //print_mqtt("Zumo035/bump","%d %d",data.accX,data.accY);

    }
    motor_forward(0,0);
    motor_stop();
  }
*/

  /*[[[[[FOR YOUR OWN SAFETY, DO NOT REMOVE THIS]]]]]
    [[[[[[[[[*/while(true)vTaskDelay(200);/*]]]]]]]]]
    [[[[[[[[[THANK YOU FOR YOUR CO-OPERATION]]]]]]]]]*/
}

/*//////////////////////////// GROUP ASSIGNMENTS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*\
================================== WEEK TWO ====================================
Assignment 3 requires Zumo robot. Other assignments work also with the plain
processor board. If you get stuck with the assignments ask the instructor or
your peers for help.

---------------------------------- ASSIGNMENT 1 --------------------------------
Write a program that blinks S-O-S morse code when uses presses the button. The
code is: dot, dot, dot, dash, dash, dash, dot, dot, dot, (long pause if code
repeats). The length of dash is three times the length of a dot. The off time
between dots and dashes is one dot long. For example, if dot lenght is 0.5
seconds the sequence is 0.5s ON, 0,5 s OFF, 0,5 ON, 0,5 s OFF, 0.5 s ON, 0.5 s
OFF, 1.5 ON, 0.5 s OFF, 1.5 s ON, …

After the sequence the program goes back to wait for another button press to
start S-O-S again.
--------------------------------------------------------------------------------
TEAM'S NOTES ABOUT THE ASSIGNMENT:
- In Morse code, length of silence between each letter is 3 time units, but in
  the assignment it was only one (0.5s). In the short version we went with it,
  but in the long version we followed the specifications of Morse code.
--------------------------------------------------------------------------------
*/

void aw2_morseCode_short(){
/* SHORT VERSION :
- All the code is within zmain (no new functions written or used)
- Does only what was told in assignment, nothing more nothing less
- Isn't helpful or re-usable for anything else than what's in the assignment
- Uses less memory than the long version
________________________________________________________________________________

(*) EXPLANATION FOR vTaskDelay(500 + i / 3 % 2 * 1000) :
Assignment was to have 3 short signals, then 3 long signals and then 3 short
signals again. Short signal is 0.5 and long 1.5 seconds. There was also a 0.5
second pause between each signal but we can ignore that here.

We want base 0.5 seconds (=500ms) for each signal and to add 1000ms for the
middle three.

We'll make a for-loop with int i starting from 0 and loop as long as i is less
than 9. Note that because i is an integer, if it's divided, the answer is
truncated towards zero (so floored within this application)(**).

- There are 9 iterations, starting from 0   : i         : 012345678
- Divide the iterator with 3                : i / 3     : 000111222 (**)
- Calculate i / 3 modulo 2                  : i / 3 % 2 : 000111000

Now in each loop we'll add i/3%2 times 1000ms to our base 500ms to have what
was wanted for the signal durations:
500ms , 500ms , 500ms , 1500ms , 1500ms , 1500ms , 500ms , 500ms , 500ms
________________________________________________________________________________

(***) EXPLANATION FOR vTaskDelay(500 + i / 8 * 1000) :
Assignment also had condition that "long pause if code repeats", so adding long
pause in the end will kind of accomplish what was wanted.
    
Similarly to previous explanation, we take advantage of integer mathematics:
    
- There are 9 iterations, starting from 0   : i     : 012345678
- Divide the iterator with 8                : i / 8 : 000000001 (**)
    
Now every loop will have 500ms delay in the end, but the final loop will get 
500ms + 1000ms delay.
________________________________________________________________________________

These work only for this specific application and are useless if we wanted to
blink any other Morse code with our BatteryLed.
*/
  while(true){                              // loop forever
    if(SW1_Read() == 0){                    // if button is pressed:
      for(int i = 0;i < 9;i++){             // do the following (9 loops):
        BatteryLed_Write(1);                // - turn BatteryLed on
        vTaskDelay(500 + i / 3 % 2 * 1000); // - wait for 500ms or 1500ms(*)
        BatteryLed_Write(0);                // - turn BatteryLed off
        vTaskDelay(500 + i / 8 * 1000);     // - wait for 500ms or 1500ms(***)
      }
    }
  }
}

void aw2_morseCode_shorter(){
/* SHORTER VERSION:
For the sake of writing shorter code for this assignment (also resulting in less
readable code), here's a silly snippet:
________________________________________________________________________________
    
(*4) EXPLANATION FOR vTaskDelay(500+(i/6%2*(i+1)%2+i/17)*1000) :
Summarizing the main goals for the assignment:
- Led on for 500ms each signal, except 1500ms for middle three signals
- 500ms pause between each signal, except 1500ms in the end
    
There are 9 signals, and after each signal there is a pause. Also there is a
"pause" in each signal. So total number of loops is 9 * (1 + 1) = 18.

Essentially what we want is 500ms for every event, except for three "LedOn"s
in the middle, and the final wait. Listing the durations it looks like this:
500,500,500,500,500,500,1500,500,1500,500,1500,500,500,500,500,500,500,1500.
Having base 500, we are only interested in adding 1000 to each base 500, so 
calculating each (duration-500) and treating 1000 as 1, we get a wanted sequence:
    
500,500,500,500,500,500,1500,500,1500,500,1500,500,500,500,500,500,500,1500
  0,  0,  0,  0,  0,  0,1000,  0,1000,  0,1000,  0,  0,  0,  0,  0,  0,1000
  0,  0,  0,  0,  0,  0,   1,  0,   1,  0,   1,  0,  0,  0,  0,  0,  0,   1
    
Truncating this we get:  000000101010000001 -----------------------------------+
                                                                               |
To get there with for loop, we can think it as these seven steps:              |
1) i                  :  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17  |
2) i/6                :  0  0  0  0  0  0  1  1  1  1  1  1  2  2  2  2  2  2  |
3) i/6%2              :  0  0  0  0  0  0  1  1  1  1  1  1  0  0  0  0  0  0  |
4) (i+1)%2            :  1  0  1  0  1  0  1  0  1  0  1  0  1  0  1  0  1  0  |
5) i/6%2*(i+1)%2      :  0  0  0  0  0  0  1  0  1  0  1  0  0  0  0  0  0  0  |
6) i/17               :  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  |
7) i/6%2*(i+1)%2+i/17 :  0  0  0  0  0  0  1  0  1  0  1  0  0  0  0  0  0  1  |
                                                                               |
And we get what we want: 000000101010000001 <----------------------------------+
    
Steps explained:
1) We have iterator i from 0 to 17.
2) Divide i with 6 to divide the sequence in three parts.
3) Isolate the middle part with modulo 2.
4) Get alternating 0 and 1. Add 1 to i so first result will be 1.
5) Multiply what we got from 3) with 4). (kinda like AND-gate)
6) Get only the last iteration.
7) Finally add 6) to 5). (basically OR-gate here)
    
Now we have what we wanted for each loop. We'll have base 500ms and in each
iteration i, i/6%2*(i+1)%2+i/17 times 1000ms is added.
________________________________________________________________________________
    
(*5) EXPLANATION FOR BatteryLed_Write((i+1)%2) :
In the built-in function BatteryLed_Write(), argument 1 turns the led on, and 0
turns the led off. i%2 would alternate between turning it off and on, starting
with turning it off, so we use (i+1)%2 instead to first turn it on.
________________________________________________________________________________
    
Again, same things as stated in previous short version will apply: this piece of
code works for this assignment and only for this assignment. If one was trying 
to re-use this for anything else than what was described in the assignment, they
would basically have to re-write the whole code. But this is why we have the 
"smarter" long version there too.
    
(Side note: GCC actually compiles longer assembly code for this than for the
 previous short version: aw2_morseCode_short)
*/
  while(true){                                   // loop forever
    while(SW1_Read());                           // wait for button press...
    for(int i=0;i<18;i++){                       // loopity loop:
      BatteryLed_Write((i+1)%2);                 // (*5)
      vTaskDelay(500+(i/6%2*(i+1)%2+i/17)*1000); // (*4)
    }
  }
}

void aw2_morseCode_long(){
/* LONG VERSION :
- Uses our own written functions outside zmain
- Does what is told in the assignment*, but there are extra features in the code
  so that it is more versatile and applicable for future use if ever necessary
- Uses more memory than the short version
- * in the assignment the pause between letters is told to be 0.5s, but as per
  Morse code specifications it should be 1.5s (when 0.5s is the length of dot).
  We did the code according to specifications.
*/
  while(true){                         // loop forever
    if(SW1_Read() == 0){               // if button is pressed:
      blinkMorse("... --- ...",500,1); // repeat "SOS" once with timeUnit 500ms
    }
  }
}

/*-------------------------------- ASSIGNMENT 2 --------------------------------
Write a program that asks user to enter his/her age. The program must measure
the time it takes for the user to enter the answer. The program must reply
according to the following table:

+---------------------+-------------+------------------+-----------------------+
| Age / Response time | Less than 3 | 3 - 5 seconds    | Over 5 seconds        |
|                     | seconds     |                  |                       |
+---------------------+-------------+------------------+-----------------------+
| 21 or under         | Super fast  | So mediocre      | My granny is          |
|                     | dude!       |                  | faster than you!      |
+---------------------+-------------+------------------+-----------------------+
| 22 - 50             | Be quick or | You're so        | Have you been smoking |
|                     | be dead     | average.         | something illegal?    |
+---------------------+-------------+------------------+-----------------------+
| Over 50             | Still going | You are doing    | Do they still allow   |
|                     | strong      | ok for your age. | you to drive?         |
+---------------------+-------------+------------------+-----------------------+

You can use C standard library functions for input and output. Note that you
must run Putty to see the output and to enter input.

The time can be measured by taking timestamps after the question is printed and
when user has entered the answer. The time response time is last timestamp -
first time stamp. You get the timestamp by calling xTaskGetTickCount(). The
return timestamp type is TickType_t. You need two variables of this type to
calculate the response time. The resolution of timestamp is millisecond so your
calculation result will give the response time in milliseconds.
--------------------------------------------------------------------------------
*/
void aw2_speedTest(){
/*
(*) EXPLANATION FOR messages[age>21?age>50?2:1:0][time>2?time>5?2:1:0] :

Array "messages" is two-dimensional, just like the table in assignment. Rows are
based on the entered age and columns on the time it took for user to answer. To
access the wanted message we need to input column and row number.

----------------- THESE TWO PIECES OF CODE: -----------------

1|  age>21?age>50?2:1:0               1|  time>2?time>5?2:1:0

----------------------- ARE SHORT FOR: ----------------------

1|  if(age > 21){                     1|  if(time > 2){
2|    if(age > 50){                   2|    if(time > 5){
3|       return 2;                    3|       return 2;
4|    } else {                        4|    } else {
5|      return 1;                     5|      return 1;
6|    }                               6|    }
7|  } else {                          7|  } else {
8|    return 0;                       8|    return 0;
9|  }                                 9|  }

------- THESE WOULD ALSO DO THE SAME (to some extent): ------

1|  ((age+7)/29>2)?2:(age+7)/29       1|  (time/3>2)?2:time/3

*/
  char *messages[3][3] = {{
    "Super fast dude!",
    "So mediocre",
    "My granny is faster than you!"
    },{
    "Be quick or be dead",
    "You're so average.",
    "Have you been smoking something illegal?"
    },{
    "Still going strong",
    "You are doing ok for your age.",
    "Do they still allow you to drive?"
  }};
  int age = 0;
  float time = 0;

  while(true){    
    printf("\nEnter your age: \n");                // ask user for age,
    TickType_t start = xTaskGetTickCount();        // and set up for time taking
    scanf("%d", &age);                             // store input to age
    time = (xTaskGetTickCount() - start) / 1000.0; // calculate answering time
    
    if(age == 0){ // if age is 0 (or possibly input wasn't a valid integer)
        int c;                              // pointer for clearing input buffer
        while((c=getchar())!='\n'&&c!=EOF); // clear the buffer
        continue;                           // skip printing (and start again)
    }
    printf("%s\n",messages[age>21?age>50?2:1:0][time>2?time>5?2:1:0]); // (*)
  }
}

/*-------------------------------- ASSIGNMENT 3 --------------------------------
Write a program that measures the battery voltage of the robot, outputs it to
the terminal, and alerts user by blinking the on board led if battery voltage is
below 4.0 V. User can acknowledge the alert by pressing the button. If the
voltage is high enough the blinking stops when button is pressed.

  +--------------+
  |              |
  |              v
  |    +-------------------+
  |    |     Read ADC      | <------------------------------+
  |    +-------------------+                                |
  |              |                                          |
  |              v                                          |
  |    +-------------------+                                |
  |    | Convert to volts  |                                |
  |    +-------------------+                                |
  |              |                                          |
  |              v                                          |
  |             / \                                         |
  |            /   \                                        |
  |           /     \                                       |
  |          / Under \ YES                                  |
  |         <  4.0 V? >-----------------------+             |
  |          \       /                        |             |
  |           \     /                         v             |
  |            \   /                 +-----------------+    |
  |             \ /             +--->|    Blink led    |    |
  |              | NO           |    +-----------------+    |
  |              v              |             |             |
  |    +-------------------+    |             v             |
  |    | Sleep for a while |    |            / \            |
  |    +-------------------+    |           /   \           |
  |              |              |          /     \          |
  +--------------+              |         / Button\  YES    |
                                |        < pressed?>--------+
                                |         \       /
                                |          \     /
                                |           \   /
                                |            \ /
                                |             | NO
                                +-------------+

Robot must be powered from batteries in order to measure the voltage. You can
easily test your program by switching the battery power off. When battery is
switched off the voltage drops quickly below 4.0 V. If the processor board is
connected to a USB port the CPU will powered by USB and your program keeps
running and detects the voltage drop. Switching battery back on will bring the
measured voltage back to normal level and you can acknowledge the alarm.
--------------------------------------------------------------------------------
*/

void aw2_batteryLow_short(){
  ADC_Battery_Start(); // start built-in battery measurements
  BatteryLed_Write(0); // turn led off first

  int16 ADC_val = 0; // ADC value will be set here
  float V_bat = 0.0; // battery voltage will be calculated here

  float ratio = 0.0018315018315018315; // pre-calculated ratio (0.0[018315])

  while(true){
    ADC_Battery_StartConvert();
    if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)){
      ADC_val = ADC_Battery_GetResult16(); // get ADC result
      V_bat = ADC_val * ratio;             // calculate original voltage

      printf("%d %f\n",ADC_val, V_bat); // print ADC result and voltage

      if(V_bat < 4){       // if battery is under 4 V
        while(SW1_Read()){ // repeat until button is pressed:
          ledOnFor(500);   // - turn led on for 500ms
          vTaskDelay(500); // - wait 500ms
        }
      }
      vTaskDelay(2000); // wait for 2 seconds before next measurement
    }
  }
}

void aw2_batteryLow_long(){
  ADC_Battery_Start(); // start built-in battery measurements
  BatteryLed_Write(0); // turn led off first

  int16 ADC_val = 0; // ADC value will be set here
  float V_bat = 0.0; // battery voltage will be calculated here

  float R1 = 5.0;     // resistor 1
  float R2 = 10.0;    // resistor 2
  float V_ref = 5.0;  // reference voltage
  int16 ADC_res = 12; // ADC value resolution (bits)

  float ratio = ADCtoBatteryRatio(R1,R2,V_ref,ADC_res);
  // calculate ratio between ADC and battery voltage to get a multiplier

  while(true){
    ADC_Battery_StartConvert();
    if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)){
      ADC_val = ADC_Battery_GetResult16(); // get ADC result
      V_bat = ADC_val * ratio;             // calculate original voltage

      printf("%d %f\n",ADC_val, V_bat); // print ADC result and voltage

      if(V_bat < 4){       // if battery is under 4 V
        while(SW1_Read()){ // repeat until button is pressed:
          ledOnFor(500);   // - turn led on for 500ms
          vTaskDelay(500); // - wait 500ms
        }
      }
      vTaskDelay(2000); // wait for 2 seconds before next measurement
    }
  }
}

/*================================= WEEK THREE =================================
---------------------------------- ASSIGNMENT 1 --------------------------------
Robot control without sensors. Make the robot run the track that is taped to
class room floor. Robot must wait for user button to be pressed before it starts
running. Start from "T" and try to hit the box as close as possible. You may
need to balance the motors for example, make one motor run slightly faster than
the other, to make the robot run straight.

   +-------------------------+
   |                         |
   |                         |
   |                         |
   |          +---+          |
   |          +-+-+          |
   |            |            |
   |            |            |
   |             \           |
   |              \          |
   |  Start         \        |
   |/  here           --__   |
 --+--                     --+

--------------------------------------------------------------------------------
*/

void aw3_sensorlessTrack(){
  while(true){            // loop forever
    if (SW1_Read() == 0){ // start if button is pressed
      vTaskDelay(500);    // wait for 0.5 seconds
      motor_start();      // start your engines!
 
      motor_forward(100, 3700);  // move straight
      turn90deg(1);              // first right turn
      motor_forward(100, 3100);  // move straight
      turn90deg(1);              // second right turn
      motor_forward(100, 3500);  // move straight
      turn90deg(1);              // final turn
      motor_turn(110, 50, 1000); // curve
      motor_forward(100, 1500);  // final staight bit

      motor_forward(0, 0); // stop
      motor_stop();        // turn the engines off...
    }
    vTaskDelay(200); // give robot some breathing space
  }
}

/*-------------------------------- ASSIGNMENT 2 --------------------------------
Robot control with ultrasonic distance sensor. Write a program that runs forward
and checks periodically the distance sensor reading. If an obstacle is closer
than 10 cm robot beeps, reverses, and turns left while reversing. Then robot
continues to run forward and check for obstacles again.
--------------------------------------------------------------------------------
*/

void aw3_obstacleUltra(){
  Ultra_Start();  // start ultrasonic sensors
  vTaskDelay(50); // give ultrasonic sensor some time to adjust
  
  motor_start();        // start your engines!
  motor_forward(100,0); // start moving forward

  while(true){                    // loop forever
    if(Ultra_GetDistance() < 10){ // if something is closer than 10 cm away
      motor_forward(0,0);         // - stop moving forward
      Beep(50, 100);              // - beep!
      motor_backward(0,0);        // - start moving backwards
      motor_turn(50, 150, 1000);  // - turn left while reversing
      motor_forward(100,0);       // - start moving forward again
    }
  }
}

/*-------------------------------- ASSIGNMENT 3 --------------------------------
Robot control with accelerometer. Write a program that runs robot forward and
makes random turns. Check the accelerometer value periodically. If the robot
hits an obstacle make the robot reverse, make a random turn 90 degrees left or
right, and start forward again.

Start with the accelerometer example and test the magnitude of sensor values by
bumping the robot from different sides and angles to find sensor value threshold
for hit/not hit.
--------------------------------------------------------------------------------
*/

void aw3_bumpAndTurnZeroDelay(){
  srand(time(0));  // initialize rand()
  motor_start();   // start your engines!
  LSM303D_Start(); // start accelerometer

  struct accData_ data; // place for X,Y and Z acceleration data
  int nextTurn = 2000;  // first turn after 2000 loops

  bool reversing = false;    // true when reversing
  int reverseDuration = 200; // for how long should we reverse
  TickType_t reverseEnd = 0; // when we should stop reversing
  
  bool turning90 = false;   // true when turning
  int turn90Duration = 200; // for how long should we turn
  TickType_t turn90End = 0; // when we should stop turning

  while(true){              // loop forever
    if(SW1_Read() == 0){    // start if button is pressed
      vTaskDelay(500);      // wait for 0.5 seconds
      motor_forward(100,0); // start moving forward
      while(true){
        LSM303D_Read_Acc(&data); // read accelerometer data
        if(data.accX < -4000){   // if robot bumps something in front of it
          turning90 = false;     // stop turning
          turn90End = 0;         // reset turning timer as well
          motor_backward(100,0); // start reversing
          reversing = true;      // tell that we are reversing now
          reverseEnd = xTaskGetTickCount() + reverseDuration; // set timer
        }
        
        if((xTaskGetTickCount() > reverseEnd) && reversing){
          // if timer went over the set time (and we are reversing)
          reversing = false;         // stop reversing
          reverseEnd = 0;            // reset reverse timer as well
          turning90 = true;          // tell that we are turning
          
          // do a random tank turn
          int dir = rand()%2;       // select randomly between 0 and 1
          MotorDirLeft_Write(!dir); // turn left motor one way,
          MotorDirRight_Write(dir); // and right motor the opposite
          PWM_WriteCompare1(200);   // set motor speeds
          PWM_WriteCompare2(200);
        
          turn90End = xTaskGetTickCount() + turn90Duration; // set turn timer
        }
        
        if((xTaskGetTickCount() > turn90End) && turning90){
          // if timer went over the set time (and we are turning)
          turning90 = false;    // stop turning
          turn90End = 0;        // reset turning timer
          motor_forward(100,0); // start moving forward again
        }
    
        if(nextTurn<0 && !reversing && !turning90){ // if we should turn again
          motor_turn(rand()%80+50,rand()%80+50,0);  // turn randomly
          nextTurn = rand()%1000 + 1000;            // set when to turn next
        }
        nextTurn--;
      }
    }
  }
}

void aw3_bumpAndTurn(){
  srand(time(0));  // initialize rand()
  motor_start();   // start your engines!
  LSM303D_Start(); // start accelerometer

  struct accData_ data; // place for X,Y and Z acceleration data
  int nextTurn = 2000;  // first turn after 200 loops

  while(true){              // loop forever
    if(SW1_Read() == 0){    // start if button is pressed
      vTaskDelay(500);      // wait for 0.5 seconds
      motor_forward(100,0); // start moving forward
      while(true){
        LSM303D_Read_Acc(&data);   // read accelerometer data
        if(data.accX < -4000){     // if robot bumps to something in front of it
          motor_backward(100,200); // reverse for a bit
          turn90deg(rand()%2);     // turn randomly left or right
          motor_forward(100,0);    // start moving forward again
        }
    
        if(nextTurn < 0){ // if it's time to take a random turn
          motor_turn(rand()%80+50,rand()%80+50,0); // turn randomly
          nextTurn = rand()%1000 + 1000;           // set next turn randomly
        }
        nextTurn--;
      }
    }
  }
}

/*================================= WEEK FOUR ==================================
---------------------------------- ASSIGNMENT 1 --------------------------------
When user button, the button in the centre of PSoC board, is pressed the robot
drives to the first line. On the first line the robot waits for a command from
IR-remote. When any IR command is received the robot starts to run forward and
stops on the fourth line.

         +-+
         | |         Stop on this line
 +-------+ +-------+/
 +-------+ +-------+
         | |
         | |
         | |
 +-------+ +-------+
 +-------+ +-------+
         | |
 +-------+ +-------+
 +-------+ +-------+
         | |
         | |
         | |         Start line. Wait for IR-signal
 +-------+ +-------+/
 +-------+ +-------+
         | |
         +-+

--------------------------------------------------------------------------------
*/

void aw4_stopAtLines(){
  IR_Start();

  uint32_t IR_val; 
  struct sensors_ ref;
  struct sensors_ dig;
  while(true){
  if (SW1_Read() == 0) {
    vTaskDelay(500);
  //start the reflectance sensors, set the thresholds
    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
    reflectance_read(&ref);
    reflectance_digital(&dig);

    /*
    print out each period of reflectance sensors
    printf("%5d %5d %5d %5d %5d %5d\r\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);

    read digital values that are based on threshold. 0 = white, 1 = black
    when blackness value is over threshold the sensors reads 1, otherwise 0

    print out 0 or 1 according to results of reflectance period
    printf("%5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);
    */

    // move to the first line
    motor_start();
    motor_forward(50,0);
    while((dig.l3  == 0) || (dig.l2 == 0) || (dig.l1 == 0) || (dig.r1 == 0) || (dig.r2 == 0) || (dig.r3 == 0)) {
      reflectance_digital(&dig);
    }
    motor_stop();
    vTaskDelay(500);
    // wait for the IR signal
    IR_flush();
    IR_wait();
    int lines = 0;
    bool line;
    // get the signal
      motor_start();
      motor_forward(50,0);
      while (true) {
        reflectance_digital(&dig);
        line = false;
        while((dig.l3  == 1) && (dig.l2 == 1) && (dig.l1 == 1) && (dig.r1 == 1) && (dig.r2 == 1) && (dig.r3 == 1)) {
          line = true;
          reflectance_digital(&dig);
        }
        if (line) {
          lines++;
        }

        if (lines == 4) {
          motor_stop();
        }
      }
  }
}
}

void aw4_stopAtLines_short(){
/*
(*) EXPLANATION FOR while(i%2^(dig.l3&dig.l2&dig.l1&dig.r1&dig.r2&dig.r3) :
+---------+---------+ +--------------------------------------------------+
| & = AND | ^ = XOR | | X = dig.l3&dig.l2&dig.l1&dig.r1&dig.r2&dig.r3     \
+---------+---------+ | X == 0 : we aren't over a line                     \
+---+-----+---------+ | X == 1 : we are over a line                         \
|   |     | (i%2)^X | +------------------------------------------------------+
| i | i%2 +---------+  Robot  .-----------------------------------------------.
|   |     | X=0 X=1 |    v   ( Before next for-loop, while-loop needs to break )
+---+-----+---------+    |    '-----------------------------------------------'
| 0 |  0  |  0   1  |    |    Once X changes to 0, the while-loop breaks <--.
| 1 |  1  |  1   0  | ---+--- Once X changes to 1, the while-loop breaks <-. \ 
| 2 |  0  |  0   1  |    |    Once X changes to 0, the while-loop breaks    \ \
| 3 |  1  |  1   0  | ---+---                     .                          | |
| 4 |  0  |  0   1  |    |                        .                          | |
| 5 |  1  |  1   0  | ---+---                     .                          | |
| 6 |  0  |  0   1  |    |    Once X changes to 0, the while-loop breaks     | |
| 7 |  1  |  1   0  | ---+--- Once X changes to 1, the while-loop breaks <-. | |
+---+-----+---------+                                                      | | |    
+---------------------------------------------------------------+          | | |
| Now, when the last line is detected, there are no more loops  +----------' | |
| left and we will continue in the code to stop the motor etc.  |            | |
+---------------------------------------------------------------+            | |
+--------------------------------------------------------------+            /  |
| After this break we wait for IR-signal ... if(i == 1) ...    +-----------'   |
+--------------------------------------------------------------+               |
+-------------------------------------------------------------+               /
| This first loop is basically skipped, because we aren't     +--------------'
| starting from over a line.                                  |
+-------------------------------------------------------------+
*/
  uint32_t IR_val; 
  struct sensors_ dig;
    
  IR_Start();
  IR_flush();
  reflectance_start();
  motor_start();
  reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
  reflectance_digital(&dig);

  while(true){                   // loop forever
    if (SW1_Read() == 0) {       // if button is pressed:
      vTaskDelay(500);           // - wait for a bit
      motor_forward(50,0);       // - start moving
      for(int i=0;i<8;i++){      // 8 loops, 2 for each line:
        reflectance_digital(&dig);   // measure reflectance before while-loop
        while((i%2)^(dig.l3&dig.l2&dig.l1&dig.r1&dig.r2&dig.r3)){ // (*)
          reflectance_digital(&dig); // measure reflectance in each while-loop
        }                            // once while-loop breaks: we can continue
        if(i == 1){            // if we detect the first line
          motor_forward(0,0);  // - stop
          IR_wait();           // - wait for signal
          motor_forward(50,0); // - start moving again
        }                            // move on to the next for-loop
      }                   // when the 4th line is detected, for-loop ends
      motor_forward(0,0); // and the robot stops moving
    }
  }
}

/* BEEN THINKIN', WHICH WOULD BE THE NICEST SOLUTION FOR WAITING A BUTTON PRESS:
 +--------- THIS ----------+  +--------- THIS ---------+  +----- OR THIS ------+
1| void fun(){               1| void fun(){              1| void x(){
2|   while(true){            2|   while(true){           2|   while(SW1_Read())
3|    if(SW1_Read() == 0){   3|     while(SW1_Read());   3|   .
4|       .                   4|     .                    4|   .
5|       .                   5|     .                    5|   .
6|       .                   6|     .                    6| }
7|     }                     7|   }                      
8|   }                       8| }                        *This repeats only once
9| }                                                      before finishing the
                                                          function x().
*/

/*-------------------------------- ASSIGNMENT 2 --------------------------------
When user button is pressed the robot drives to the first line. On the first
line the robot waits for a command from IR-remote. When any IR command is
received the robot starts to run forward. When the robot comes to first
intersection it turns left and then takes the next two intersections to the
right. The robot stops on the fourth intersection.

      +-+               +-+ Stop here
      | |               | |/
 +----+ +---------------+ +----+
 +----+ +---------------+ +----+
      | |\              | |
      | | Turn right    | |
      | |               | |
      | |               | |
      | | Turn right    | |
      | |/              | |
 +----+ +---------------+ +----+
 +----+ +------+ +------+ +----+
      | |      | |\     | |
      +-+      | | Turn +-+
               | | left
               | |      Start line. Wait for IR-signal
          +----+ +----+/
          +----+ +----+
               | |
               +-+

--------------------------------------------------------------------------------
*/

void aw4_turnAtIntersections(){
    
}

void aw4_turnAtIntersections_short(){
  uint32_t IR_val; 
  struct sensors_ dig;
    
  IR_Start();
  IR_flush();
  reflectance_start();
  motor_start();
  reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
  reflectance_digital(&dig);

  while(true){
    if (SW1_Read() == 0) {
      vTaskDelay(500);
      motor_forward(50,0);

      while(!(dig.l3&dig.l2&dig.l1&dig.r1&dig.r2&dig.r3)){
        reflectance_digital(&dig);
      }
      motor_forward(0,0);
      IR_wait();
      vTaskDelay(500);
      motor_forward(50,0);
      while(dig.l3&dig.l2&dig.l1&dig.r1&dig.r2&dig.r3){
        reflectance_digital(&dig);
      }
      for(int i = 1; i < 4; i++){
        while (true) {
          reflectance_digital(&dig);
          if (dig.l3 & dig.l2 & dig.l1 & dig.r1 & dig.r2 & dig.r3) {
            break;
          }
        }

        vTaskDelay(500);
        turn90deg(abs((i/2)-1));
        motor_forward(50, 0);
      }
      while(true){
        reflectance_digital(&dig);
        if (dig.l3 & dig.l2 & dig.l1 & dig.r1 & dig.r2 & dig.r3) {
          break;
        }
      }
      motor_stop();
    }
  }
}

/*-------------------------------- ASSIGNMENT 3 --------------------------------
When user button is pressed the robot drives to the first line. On the first
line the robot waits for a command from IR-remote. When any IR command is
received the robot starts to run forward following the line and stops on the
first line.

 +-+  Stop on this line
 | |/
 | +------------ _
 | +------------ _ -
 | |               \ \
 +-+               | |
                   | |
                _ / /
           _ -- _ -
         - _ --
       / /
       | |
       | |      Start line. Wait for IR-signal
  +----+ +----+/
  +----+ +----+
       | |
       +-+

--------------------------------------------------------------------------------
TEAM'S NOTES ABOUT THE ASSIGNMENT:
Reflectance sensors aren't evenly distributed.

  L_3           L_2         L_1       R_1         R_2           R_3
   o-------------o-----------o---------o-----------o-------------o
   |             |           |         |           |             |
   |             |           |<-- a -->|           |             |
   |             |                                 |             |
   |             |<-------------- b -------------->|             |
   |                                                             |
   |<---------------------------- c ---------------------------->|
                       
                               WEIGHTS:

  L_3           L_2         L_1   |   R_1         R_2           R_3
   o-------------o-----------o----+----o-----------o-------------o
   |             |           |    |    |           |             |
-(c/2)        -(b/2)      -(a/2)  |   a/2         b/2           c/2

We calculate the center of line by giving each measurement a weight according to
its distance from the middle of the robot, sum the products and divide with the
number of sensors.

+------------+----------------------------+
|  Measured  |          Weights           |
+------------+----------------------------+
| a =  9.5mm | L_1 / R_1 : - 4.75 /  4.75 |
| b = 32.0mm | L_2 / R_2 : -16.00 / 16.00 |
| c = 59.0mm | L_3 / R_3 : -29.50 / 29.50 |
+------------+----------------------------+

Because the contrast of the track isn't guaranteed to be consistent, we check
the maximum value, and use it as reference for others.

--------------------------------------------------------------------------------
*/

void aw4_followLine(){
  uint32_t IR_val; 
  struct sensors_ ref;
  struct sensors_ dig;
  float maxSpeed = 255;
    
  float val_l = 0;
  float val_r = 0;
  float val   = 0;
  float max_l = 0;
  float max_r = 0;
  float max   = 0;
    
  IR_Start();
  reflectance_start();
  motor_start();
  reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
  reflectance_read(&ref);
  reflectance_digital(&dig);

  while(SW1_Read()){};
  vTaskDelay(1000);
  motor_forward(50,0);

  while(!(dig.l3&dig.l2&dig.l1&dig.r1&dig.r2&dig.r3)){
    reflectance_digital(&dig);
  }
  motor_forward(0,0);
  IR_wait();
  motor_forward(50,0);
  while(dig.l3&dig.l2&dig.l1&dig.r1&dig.r2&dig.r3){
    reflectance_digital(&dig);
  }

  while(true){
    reflectance_read(&ref);
    
    val = ((-29.5*ref.l3)+(-16.0*ref.l2)+(-4.75*ref.l1)+4.75*ref.r1+16.0*ref.r2+29.5*ref.r3)/6.0;
    
    /*
    max_l = ref.l3>ref.l2?ref.l3>ref.l1?ref.l3:ref.l1:ref.l2;
    max_r = ref.r3>ref.r2?ref.r3>ref.r1?ref.r3:ref.r1:ref.r2;
    max = max_l>max_r?max_l:max_r;
    
    val_l = 29.5*(ref.l3/max)+16.8*(ref.l2/max)+4.75*(ref.l1/max);
    val_r = 29.5*(ref.r3/max)+16.8*(ref.r2/max)+4.75*(ref.r1/max);
    val = (val_r-val_l)/6;
    */
    int pwm1 = maxSpeed+val/50;
    int pwm2 = maxSpeed-val/50;
    
    if(pwm1>255){pwm1=255;}
    if(pwm2>255){pwm2=255;}
    if(pwm1<0){pwm1=0;}
    if(pwm2<0){pwm2=0;}
    
    PWM_WriteCompare1(pwm1);
    PWM_WriteCompare2(pwm2);
    
    reflectance_digital(&dig);
    if(dig.l3&dig.l2&dig.l1&dig.r1&dig.r2&dig.r3){
      break;
    }
    //if((dig.l3==0)&(dig.l2==0)&(dig.l1==0)&(dig.r1==0)&(dig.r2==0)&(dig.r3==0)){
    //  break;
    //}
  }
  motor_stop();
}

/*================================= WEEK FIVE ==================================
The following assignments require that you have a WiFi network set up, an MQTT
broker, and a client that can subscribe messages that your robot sends.
---------------------------------- ASSIGNMENT 1 --------------------------------
Write a program that asks the user to enter the current time (hours and
minutes). When the user has entered the time, the program enters an infinite
loop and sends the current time over MQTT every time when the user presses the
PSoC user button.

Use real time clock for time keeping. See Zumo library documentation and
examples for details.
--------------------------------------------------------------------------------
*/

void aw5_reportTimeMQTT(){
  char* topic = "Zumo035/time" ;
  char* time = "";
  int hour = 0;
  int minute = 0;
    
  RTC_Start();
  RTC_TIME_DATE now;

  while(SW1_Read()){};  // wait for button press

  printf("Input hour:\n");
  scanf("%d",&hour);
  now.Hour = hour;
  printf("Input minutes:\n");
  scanf("%d",&minute);
  now.Min = minute;
  now.Sec = 0;

  RTC_WriteTime(&now);

  while(true){
    RTC_DisableInt();
    now = *RTC_ReadTime();
    RTC_EnableInt();
    print_mqtt(topic,"%02d:%02d.%02d",now.Hour,now.Min,now.Sec);
    vTaskDelay(1000);
  }
}

/*-------------------------------- ASSIGNMENT 2 --------------------------------
Write a program that uses ultrasonic sensor to detect obstacles. When an
obstacle is detected the robot reverses and makes a random 90 degree turn to
left or right. The turn direction (left/right) is sent to topic <robot serial
number>/turn.
--------------------------------------------------------------------------------
*/

void aw5_reportUltraAndTurn(){

}

void aw5_reportUltraAndTurn_short_silly(){
//.---- INITIALIZE VARIABLES -----.
  ; char* topic = "Zumo035/turn"  ;
  ; char* data  = ""              ;
  ; int   dir   = 0               ;
  ; srand(time(0)) // seed rand() ;
//'-------------------------------'
  
//.------- START AND SETUP ROBOT SYSTEMS -------.
  ; Ultra_Start();  // start ultrasonic sensors ;
  ; motor_start();  // start your engines!      ;
//'---------------------------------------------'
  
//.--- WAIT FOR BUTTON PRESS BEFORE DOING ANYTHING ---.
  ; while(SW1_Read()){}                               ;
  ; vTaskDelay(500)                                   ;
//'---------------------------------------------------'
  
//.------------ PART OF FUNCTION BODY THAT DOES THE ASSIGNMENT --------------.
  ; while(true){                          // loop forever:                   ;
  ;   motor_forward(50,0);                // start moving forward            ;
  ;   while(Ultra_GetDistance()>10){};    // move until something gets close ;
  ;   motor_backward(50,500);             // reverse a bit                   ;
  ;   dir = rand()%2;                     // get random direction:           ;
  ;   turn90deg(dir);                     // - turn that way (left/right)    ;
  ;   data = dir == 1 ? "left" : "right"; // tell which way we turned        ;
  ;   print_mqtt(topic,"%s",data);        // send turn direction             ;
  ; }                                     // (start over)                    ;
//'--------------------------------------------------------------------------'
}

/*-------------------------------- ASSIGNMENT 3 --------------------------------
Write a program that runs the robot to an intersection. When the intersection is
reached the robot waits for IR-remote command. After receiving the IR-command
the robot runs forward until it sees another intersection. The robot stops at
the intersection and sends the elapsed time (from IR-command to next
intersection) to topic <robot serial number>/lap.
--------------------------------------------------------------------------------
*/

void aw5_reportIntersection(){

}

void aw5_reportIntersection_short_silly(){
//.---------------- INITIALIZE VARIABLES -----------------.
  ; struct sensors_ dig   = {0,0,0,0,0,0}                 ;
  ; TickType_t      start = 0                             ;
  ; int             time  = 0                             ;
  ; char*           topic = "Zumo035/elapsedMilliseconds" ;
//'-------------------------------------------------------'

//.-------------- START AND SETUP ROBOT SYSTEMS ---------------.
  ; IR_Start()                                                 ;
  ; IR_flush()                                                 ;
  ; reflectance_start()                                        ;
  ; reflectance_set_threshold(9000,9000,11000,11000,9000,9000) ;
  ; reflectance_digital(&dig)                                  ;
  ; motor_start()                                              ;
//'------------------------------------------------------------'

//.------------ PART OF FUNCTION BODY THAT DOES THE ASSIGNMENT -------------.
  ; while(true){                        // loop forever:                    ;
  ;   IR_wait();                        // wait for IR signal               ;
  ;   motor_forward(50,0);              // move forward                     ;
  ;   start=xTaskGetTickCount();        // start taking time                ;
  ;   for(int i=0;i<2;i++){             // move until next line is detected ;
  ;     while((i%2)^overLine(dig));     //  .                               ;
  ;   }                                 //  .                               ;
  ;   time=(xTaskGetTickCount()-start); // stop taking time                 ;
  ;   motor_forward(0,0);               // stop moving                      ;
  ;   print_mqtt(topic,"%d",time);      // send elapsed time over MQTT      ;
  ; }                                   // (start over)                     ;
//'-------------------------------------------------------------------------'
}

/*////////////////////////////// Robot projects \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*\
--------------------------------- MQTT topics ----------------------------------
Each robot has a unique serial number. The serial number is on a sticker on the 
battery box cover. The serial number is “Zumo” and three digits, for example 
Zumo018. The robot serial number must be used as the top-level topic on all data
that the robot publishes to the broker. Do not use spaces in the top-level 
topic. Subtopics to use depend on the project in question. See project 
descriptions for details on what to send.

All data must be sent as text without linefeeds. If you send multiple values 
separate them with spaces. MQTT is a packet protocol so there is no need to send
linefeeds with the data. Sending linefeeds in the competition is considered an 
error that lowers your project grade.

================================ Sumo wrestling ================================
Robot must drive along the line to the edge of the sumo ring and wait for start 
signal. The start signal is given with an IR remote. Three robots wrestle at a 
time and one starting place is left empty at random. The robot tries to push 
other robots out of the ring. The last robot in the ring is the winner. Robot is
considered to be out of the ring if the back wheels cross the line. Instructors 
will judge who is out and who is not.

\                                     /
  \          __---------__          /
    \    _ --             -- _    /
      \ /                     \ /
      /                         \
    /                             \
   |                               |
  |                                 |
 |                                   |
 |                                   |      Start:
 |                                   |    / Robot drives to the edge of
 |                                   |   /  the ring and waits for a start
  |                                 |   /   signal. The start signal is
   |                               |   /    given with an IR remote
    \                             /   /
      \                         /    /
      / \_                   _/ \   /
    /      --__         __--      \/
  /            ---------            \
/                                     \

Robot must send the following data over MQTT connection:
- Ready indicator when robot stops on the start line and starts to wait for IR 
  start signal:
 > Subtopic: ready
 > Data: “zumo”
- Start time (systick based time stamp = number of milliseconds since the robot 
  code started)
 > Subtopic: start
- Stop time (when the user button is pressed robot stops and sends stop time 
  stamp)
 > Subtopic: stop
- Run time (from start to finish, number of milliseconds between start and stop)
 > Subtopic: time
- Timestamp of each hit it gets (or gives)
 > Subtopic: hit
 > Bonus (optional): send also the direction of the hit (calculate hit angle)

                                               _   _                 0       
For example, Zumo028 sends:                _ -|_|-|_|- _             |      45
Zumo028/ready zumo  Zumo028/ready zumo    | |'-|-|-|-'| |         _ -+- _  /
Zumo028/start 2301  Zumo028/start 2301    | |' | _ | '| |       /    |   /\
Zumo028/hit 3570    Zumo028/hit 3570 75   | |' ||_|| '| |      |     | /   |
Zumo028/hit 4553    Zumo028/hit 4553 185  | |' | o | '| | 270-|------+------|-90
Zumo028/hit 4988    Zumo028/hit 4988 25   |_|'-|. .|-'|_|      |   / |     |
Zumo028/stop 5210   Zumo028/stop 5210 \     |__|---|__|         \/_  |  _ /
Zumo028/time 2909   Zumo028/time 2909  \       |_-_|           /    -+- 
     /              (with hit angles)   \       |_|         225      |
    /                                    \                          180
   /                                      \
This is topic/subtopic. It will be   This is data. The space between topic and 
displayed automatically in the log   data is added automatically in the log. Use 
                                     spaces only to separate data values if you 
                                     send multiple values.
--------------------------------------------------------------------------------
TEAM'S NOTES ABOUT THE PROJECT:
- When calculating the hit direction in XY-plane, we can completely ignore Z-
  values of accelerometer (it's only [slightly] relevant when calculating hit
  force). For direction calculation we use arc tangent and convert radians to
  degrees we multiply by 180 / PI. We also need to have an if-statement, because
  if Y-value is negative, 180 degrees needs to be added to the hit angle. 
  <math.h> needs to be included for atan() and PI defined for, well, pi... of
  course we could use precalculated 180/3.14159265358979323846, but for clarity
  we use defined PI here.

  1| float hitAngle = atan(y / x) * 180 / PI;
  2| if(y < 0){
  3|   hitAngle += 180;
  4| }

Applications for sensors:
ULTRASONIC:
- Detect opponents.
- Spinning and using ultrasonic sensor could be used as a "radar" and act based
  on it. Here are some thoughts:
  1) lock on an opponent when one is detected
  2) dodge at the last moment when an opponent advances
  3) track number of opponents left

ACCELEROMETER:
- Detect hits and calculate hit directions.

GROUND SENSORS:
- Detect edge of the ring.

--------------------------------------------------------------------------------
*/

void rp_sumoWrestling(){
  struct sensors_ ref, dig = {0,0,0,0,0,0};
  struct accData_ acc      = {0,0,0};
  TickType_t start         = 0;
  TickType_t lastHitTime   = 0;
  TickType_t dodgeEndTime  = INFINITY;
  TickType_t atCenterTime  = INFINITY;
  bool runningToEdge       = false;
  bool headedForCenter     = false;
  bool atCenter            = false;
  bool enemyDetected       = false;
  int hit                  = 361;
  int defSpeed             = 200;
  int atkSpeed             = 255;

  startComponents("zumo");  
  moveToStartLineAndWait("zumo",dig);
  start = startWrestling(dig);
  while(SW1_Read()){
    hit = detectZumoHit(acc);
    
    if(hit<361.0 && xTaskGetTickCount() - lastHitTime > 200 && !enemyDetected){
      if(headedForCenter){
        headedForCenter = false;
        runningToEdge = true;
        motor_forward(defSpeed,0);
      }
      if(!runningToEdge){
        dodgeEndTime = respondToHit(hit);
      }
      reportZumoHit(hit);
      lastHitTime = xTaskGetTickCount();
      hit = 361.0;
    }
    
    if(dodgeEndTime < xTaskGetTickCount()){
      atCenter = false;
      if(Ultra_GetDistance() < 10){
        enemyDetected = true;
        motor_forward(atkSpeed,0);
      } else {
        runningToEdge = true;
        motor_forward(defSpeed,0);
      }
      dodgeEndTime = INFINITY;
    }
    
    if(enemyDetected){
      while(Ultra_GetDistance() < 15 && blackSensors(dig) < 2){
        vTaskDelay(20);
      }
      enemyDetected = false;
      runningToEdge = true;
      motor_forward(defSpeed,0);
    }
    
    if(blackSensors(dig) > 1){
      while(turnToCenter(ref)>100&&blackSensors(dig)>0&&blackSensors(dig)<6){};
      runningToEdge = false;
      headedForCenter = true;
      atCenterTime = xTaskGetTickCount() + 1500; // adjust
      motor_backward(defSpeed,0);
    } else if(atCenterTime < xTaskGetTickCount()){
      runningToEdge = false;
      headedForCenter = false;
      atCenter = true;
      atCenterTime = INFINITY;
      tankTurn(150,1);
    } else if(atCenter && Ultra_GetDistance() < 10){
      enemyDetected = true;
      atCenter = false;
      motor_forward(atkSpeed,0);
    }
  }
  wrestlingEnd(start);
}
    
    /*
     if(atCenter){
      // attackMode
    } else {
      // defenseMode
      motor_forward(255,0);
    }
    
    if(atCenterTime < xTaskGetTickCount()){
      runningToEdge = false;
      atCenter = true;
      atCenterTime = INFINITY;
    }
    
    if(dodgeEndTime < xTaskGetTickCount()){
      runningToEdge = true;
      dodgeEndTime = INFINITY;
    }
    
    if(hit<361.0 && xTaskGetTickCount() - lastHitTime > 200){
      if(headingToCenter){
        headingToCenter = false;
        runningToEdge = true;
      }
      if(!runningToEdge){
        dodgeEndTime = respondToHit(hit);
      }
      reportZumoHit(hit);
      lastHitTime = xTaskGetTickCount();
      hit = 361.0;
    }
    
    if(runningToEdge && blackSensors(dig)>0){
      while(turnToCenter(ref)>100&&blackSensors(dig)>0&&blackSensors(dig)<6){};
      runningToEdge = false;
      headingToCenter = true;
      atCenterTime = xTaskGetTickCount() + 1500;
    }
    */
    
    /*
    if(atCenterTime < xTaskGetTickCount()){
      // attackMode
      runningToEdge = false;
      atCenter = true;
      atCenterTime = INFINITY;
    } else {
      // defenseMode
      motor_forward(255,0);
      if(dodgeEndTime < xTaskGetTickCount()){
        runningToEdge = true;
        dodgeEndTime = INFINITY;
      }
    
      if(hit<361.0 && xTaskGetTickCount() - lastHitTime > 200){
        if(headedForCenter){
          headedForCenter = false;
          runningToEdge = true;
        }
        if(!runningToEdge){
          dodgeEndTime = respondToHit(hit);
        }
        reportZumoHit(hit);
        lastHitTime = xTaskGetTickCount();
        hit = 361.0;
     }
    
      if(runningToEdge && blackSensors(dig)>0){
        while(turnToCenter(ref)>100&&blackSensors(dig)>0&&blackSensors(dig)<6){};
        runningToEdge = false;
        headedForCenter = true;
        atCenterTime = xTaskGetTickCount() + 1500;
      }
    }
    */

void goToCenter(struct sensors_ dig, struct sensors_ ref){
    while(turnToCenter(ref)>100&&blackSensors(dig)>0&&blackSensors(dig)<6){};
    motor_backward(150,500);
    turn90deg(0);
    turn90deg(0);
    motor_forward(150,0);
}

void goToCenter2(){ // struct sensors_ dig, struct sensors_ ref
  struct sensors_ ref, dig;
  startComponents("zumo");
  IR_wait();
  while(true){
    motor_forward(150,0);
    reflectance_digital(&dig);
    while(!(dig.l3|dig.l2|dig.l1|dig.r1|dig.r2|dig.r3)){
      reflectance_digital(&dig);
    };
    while(turnToCenter(ref)>100&&blackSensors(dig)>0&&blackSensors(dig)<6){};
    motor_backward(150,500);
    turn90deg(0);
    turn90deg(0);
    motor_forward(150,0);
  }
}

TickType_t respondToHit(int hit){
  TickType_t dodgeStart = xTaskGetTickCount();
  int delay = 0;
  if(hit<45){
    motor_backward(0,0);
    motor_turn(255,50,0);
    delay = 500;
  } else if (hit<90){
    motor_backward(0,0);
    motor_turn(255,50,0);
    delay = 350;
  } else if (hit<135){
    motor_forward(0,0);
    motor_turn(255,50,0);
    delay = 400;
  } else if (hit<180){
    motor_forward(0,0);
    motor_turn(255,50,0);
    delay = 500;
  } else if (hit<225){
    motor_forward(0,0);
    motor_turn(50,255,0);
    delay = 500;
  } else if (hit<270){
    motor_forward(0,0);
    motor_turn(50,255,0);
    delay = 400;
  } else if (hit<315){
    motor_backward(0,0);
    motor_turn(50,255,0);
    delay = 350;
  } else if (hit<360){
    motor_backward(0,0);
    motor_turn(50,255,0);
    delay = 500;
  }
  return dodgeStart + delay;
}

TickType_t startWrestling(struct sensors_ dig){
  TickType_t start = xTaskGetTickCount();
  print_mqtt("Zumo035/start","%d",start);
  reportMQTT("start",start);
  motor_forward(50,0);
  while(overLine(dig)){};
  return start;
}

int detectZumoHit(struct accData_ acc){
  double hit = 361.0;
  LSM303D_Read_Acc(&acc);
  if(sqrt(pow(acc.accX,2.0)+pow(acc.accY,2.0)) > 6000.0){
    hit = atan((double)acc.accX/(double)acc.accY)*57.29577951308232+90.0;
    if(acc.accY < 0){
      hit += 180.0;
    }
  }
  return (int)hit;
}

void reportZumoHit(int hitDir){
  print_mqtt("Zumo035/hit","%d %d",xTaskGetTickCount(), hitDir);
}

void wrestlingEnd(TickType_t start){
  motor_forward(0,0);
  TickType_t stop = xTaskGetTickCount();
  print_mqtt("Zumo035/stop","%d",stop);
  motor_stop();
  print_mqtt("Zumo035/time","%d",stop-start);
}

void chaseObject(){
  Ultra_Start();
  motor_start();
  IR_Start();
  IR_flush();
  IR_wait();
  TickType_t turnStart = 0;
  while(true){
    motor_forward(50,0);
    while(Ultra_GetDistance()<15){vTaskDelay(20);};
    MotorDirLeft_Write(1);
    MotorDirRight_Write(0);
    PWM_WriteCompare1(100);
    PWM_WriteCompare2(100);
    while((Ultra_GetDistance()>15)){vTaskDelay(20);};
    //for(int i = 0; i<100; i++){
      //turnStart = xTaskGetTickCount();
     // motor_turn((i%2)*255,((i+1)%2)*255,0);
     // while((Ultra_GetDistance()>35)){};
      //if(Ultra_GetDistance()<35){
       // break;
     // }
   // }
    //printf("%d\n",thisTurn); (xTaskGetTickCount() - turnStart < (uint)i*20) && 
  }
}

/*
void rp_sumoWrestling(){
  struct sensors_ ref;
  struct sensors_ dig;
    
  IR_Start();
  reflectance_start();
  motor_start();

  reportReady("zumo");
  IR_wait();
  zumo_start = xTaskGetTickCount();
  reportStart(zumo_start);
  motor_forward(50,0);
  while(SW1_Read()){
    detectZumoHit();
    if(zumo_hit){
      reportZumoHit();
      zumo_hit = false;
    }
  }

  motor_stop();
  reportStop(xTaskGetTickCount());
  reportTime(xTaskGetTickCount() - zumo_start);
}
*/

/*============================== Line follower =================================
Robot follows the black line using reflective sensors in the front of the robot.
Track total length is about 10 meters. Robot has six reflective sensors. The 
center sensors are used mainly to keep the robot on the track and side sensors 
to detect intersections or tight curves on the track. The robot must follow the 
start and stop rules stated below. The intersections are in a 90-degree angle on
a straight line.

The competition track is 2 cm wide.

                                 _ ----- _
                               /           \
                              |          __/
                             |        _--
                              -_      \__
                                \     ___/
                                |    |___
                                 |    ___|
                                 |   /___
Hint: Implement line            |      __/
following first. When your      |     |
robot is able to follow        /       \
the line then implement       |         |
start and stop logic.         \         |
                                \    _   \
                                 |  |  \  \
                                /   |  |   |
                               /    |   \_ /
                               \     |
                                 \    \_
                                  |    _|
                                /     /
                              /      |
                              \___   \
                                  |   \_
                             --__-      --__
                            |               |
                             \__        ____/
         Finish:                 \     /_
         Robot stops on the       |       -       Start:
         second line.       \     |         \   / Robot drives to the line and
                             \ ---+---       | /  waits on the line for a start
                              \   |          |/   signal. The start signal is
                               ---+---    ---+--- given with an IR remote
                                  |          |                              

Robot must send the following data over MQTT connection:
- Ready indicator when robot stops on the start line and starts to wait for IR 
  start signal:
 > Subtopic: ready
 > Data: “line”
- Start time (systick based time stamp = number of milliseconds since the robot 
  code started)
 > Subtopic: start
- Stop time (when the robot stops on the finish line it sends stop time stamp)
 > Subtopic: stop
- Run time (from start to finish, number of milliseconds between start and stop)
 > Subtopic: time
- Bonus (optional):
 - Timestamp if both centre sensors go off the track
  > Subtopic: miss
 - Timestamp when both centre sensors go back to the track after a miss
  > Subtopic: line
 - You must implement both timestamps to get a bonus

For example, Zumo028 sends:
Zumo028/ready line  Zumo028/ready line
Zumo028/start 1210  Zumo028/start 1210
Zumo028/stop 54290  Zumo028/miss 7912
Zumo028/time 53080  Zumo028/line 8012
                    Zumo028/miss 32103
                    Zumo028/line 32630
                    Zumo028/miss 53223
                    Zumo028/line 53933
                    Zumo028/stop 54290
                    Zumo028/time 53080
--------------------------------------------------------------------------------
*/


/*
void rp_lineFollower(){
  struct sensors_ ref;
  struct sensors_ dig;
  TickType_t start;

  startComponents("line");  
            // IR_Start();
            // reflectance_start();
            // reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
            // motor_start();
  moveToStartLineAndWait();
            // wait for button press
            // move until line is detected
            // stop
            // reportReady("line");
            // IR_wait();
  start = startLineFollowing("line",200);
            // motor_start();
            // TickType_t start = xTaskGetTickCount();
            // reportStart(line_start);
            // motor_forward(50,0);
            // while(overLine(dig)){};
            // return start;
  while(!overLine(dig)){
    followLine(ref);
  }
  moveToFinishLine();
            // motor_stop();
            // reportStop(xTaskGetTickCount());
            // reportTime(xTaskGetTickCount() - line_start);
}
*/

void rp_lineFollower(){
  struct sensors_ ref, dig;
  TickType_t start;
  int line = 0;

  startComponents("line");  
  moveToStartLineAndWait("line",dig);
  start = startLineFollowing(dig);
  while(!overLine(dig)){
    line = followLine(ref,dig,line);
  }
  moveToFinishLine(start);
}

void startComponents(char *project){
  IR_Start();
  reflectance_start();
  reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
  motor_start();
  if(strncmp(project,"line",4)){
    LSM303D_Start();
    Ultra_Start();
  }
}

void moveToStartLineAndWait(char *project, struct sensors_ dig){
  while(SW1_Read()){};
  vTaskDelay(500);
  motor_forward(50,0);
  while(!overLine(dig)){};
  motor_forward(0,0);
  print_mqtt("Zumo035/ready","%s",project);
  IR_wait();
}

TickType_t startLineFollowing(struct sensors_ dig){
  TickType_t start = xTaskGetTickCount();
  print_mqtt("Zumo035/start","%d",start);
  motor_forward(50,0);
  while(overLine(dig)){};
  return start;
}

void moveToFinishLine(TickType_t start, struct sensors_ dig){
  motor_forward(50,0);
  while(overLine(dig)){};
  while(!overLine(dig)){};
  TickType_t stop = xTaskGetTickCount();
  print_mqtt("Zumo035/stop","%d",stop);
  motor_stop();
  print_mqtt("Zumo035/time","%d",stop-start);
}

/*================================== Maze ======================================
Robot finds a path through a grid by following lines and taking turns at 
intersections to avoid obstacles.

                                                End:
                                              / Robot has run straight for a
     0,13                                    /  while without seeing an
          \ |                              |/   intersection
           \|                              |     
        +---+---+                      +---+---+
        |   |   |                      |   |   |
    +---+---+---+---+     0,5      +---+---+---+---+
    |   |   |   |   |    /         |   |   |   |   |
+---+---+---+---+---+---/      +---+---+---+---+---+---+
|   |   |   |   |   |  /|      |   |   |   |   |   |   |  Obstacles:
+---+---+---+---+---+-/-+      +---+---+---+---+---+---+/ There are three bricks
|   |   |   |   |   |/  |      |  -------  |   |   |   /  at random locations
+---+---+---+---+---/---+      +-|       |-+---+---+--/+
|   |   |   |   |  /|   |      |  -------  |   |   | / |
+---+---+---+---+-/-+---+      +---+---+---+---+---+/--+
|   |   |   |   |/  |   |      |   |   |   |   |   /   |
+---+---+---+---/---+---+      +---+---+---+---+--/+---+
|   |   |   |  /|   |   |      |   |   |  -------/ |   |
+---+---+---+-/-+---+---+      +---+---+-|       |-+---+  Start:
|   |   |   |/  |   |   |      |   |   |  -------  |   |/ Robot drives to the
+---+---+---+---+---+---+      +---+---+---+---+---+---/  line and waits on the
|   |   |   |   |   |   |      |   |   |   |   |   |  /|  line for a start
+---+---+---+---+---+---+      +---+---+---+---+---+-/-+  signal. The start
|   |   |   |   |   |   |      |   |   |   |   |   |/  |  signal is given with
+---+---+---+---+---+---+      +---+---+---+---+---/---+  an IR remote
|   |   |   |   |   |   |      |   |  -------  |  /|   |
+---+---+---+---+---+---+      +---+-|       |-+-/-+---+
|  -3,0 |   |   |   |   |  3,0 |   |  -------  |/  |   |
+-/-+---+---+---+---+---+ /    +---+---+---+---/---+---+
|/  |   |   |   |   |   |/     |   |   |   |  /|   |   |
+---+---+---+---+---+---+      +---+---+---+-/-+---+---+
            |\                             |/
         ---+-\-                        ---+---
            |  0,0                         |


            |                          |
            |                          |
        +---+---+                  +---+---+
        |   |   |                  |   |   |           Obstacles:
    +---+---+---+---+          +---+---+---+---+     / Never on the last full  
    |   |   |   |   |          |   |   |   |   |    /  "row" or after that. 
+---+---+---+---+---+---+  +---+---+---+---+---+---+
|   |  ---  |   |   |   |  |   |   |   |  -------  |
+---+-|   |-+---+---+---+  +---+---+---+-|       |-+
|   | |   | |   |   |   |  |   |   |   |  -------  |
+---+-|   |-+---+---+---+  +---+---+---+---+---+---+
|   |  ---  |   |   |   |  |   |   |   |   |   |   |
+---+---+---+---+---+---+  +---+---+---+---+---+---+
|   |   |   |   |   |   |  |   |   |   |   |   |   |
+---+---+---+---+---+---+  +---+---+---+---+---+---+
|   |   |  -------  |   |  |   |  -------  |   |   |
+---+---+-|       |-+---+  +---+-|       |-+---+---+
|   |   |  -------  |   |  |   |  -------  |   |   |
+---+---+---+---+---+---+  +---+---+---+---+---+---+
|   |   |   |   |   |   |  |   |   |   |   |   |   |
+---+---+---+---+---+---+  +---+---+---+---+---+---+
|   |   |   |   |   |   |  |   |  ---  |   |   |   |
+---+---+---+---+---+---+  +---+-|   |-+---+---+---+
|   |  -------  |   |   |  |   | |   | |   |   |   |
+---+-|       |-+---+---+  +---+-|   |-+---+---+---+   Obstacles:
|   |  -------  |   |   |  |   |  ---  |   |   |   | / Never on the first or
+---+---+---+---+---+---+  +---+---+---+---+---+---+/  second "row"
|   |   |   |   |   |   |  |   |   |   |   |   |   |
+---+---+---+---+---+---+  +---+---+---+---+---+---+
            |                          |
         ---+---                    ---+---
            |                          |   Bricks are never oriented diagonally.

Robot must send the following data over MQTT connection:
- Ready indicator when robot stops on the start line and starts to wait for IR 
  start signal:
 > Subtopic: ready
 > Data: “maze”
- Start time (systick based time stamp = number of milliseconds since the robot 
  code started)
 > Subtopic: start
- Stop time (when the robot stops on the finish line it sends stop time stamp)
 > Subtopic: stop
- Run time (from start to finish, number of milliseconds between start and stop)
 > Subtopic: time
- Bonus (optional):
 - send coordinates of each intersection the robot visits
  > Subtopic: position

For example, Zumo028 sends:
Zumo028/ready maze  Zumo028/ready maze
Zumo028/start 1210  Zumo028/start 1210
Zumo028/stop 24290  Zumo028/position 0 0
Zumo028/time 23080  Zumo028/position 0 1
                    Zumo028/position 0 2
                    Zumo028/position 0 3
                    Zumo028/position 0 4
                    Zumo028/position 0 5
                    Zumo028/position -1 5
                    Zumo028/position -2 5
                    Zumo028/position -2 6
                    Zumo028/position -2 7
                    ...
                    Zumo028/position 0 13
                    Zumo028/stop 24290
                    Zumo028/time 23080
--------------------------------------------------------------------------------
*/
/*
void mazeMain(){      
    //startComponents();
    moveToStartLine();
    //waitAndStart_maze();
    if(
    firstIntersections();
    mazeSolver2000();
    stopAndReport();
    maze_finished();
 
}
*/

/*

TickType_t maze_start    = 0;
TickType_t maze_ix_timer = 0;
int maze_x               = 0;
int maze_y               = 0;
int maze_dir             = 0;
int maze_ix_count        = 0;
int maze_ix[1000][3];
bool maze_bump         = false;

struct sensors_ ref;
struct sensors_ dig;
struct accData_ accData;

void rp_maze(){
  startComponents();
  moveToStartLine();
  waitAndStart("maze",100);
  moveToNextLine();
  while(!maze_finished()){
    if(!maze_bump){
      reportMazePosition();
      decideMazeTurn();
    } else {
      returnToPreviousIntersection();
    }
    while(!inIntersection() && !maze_finished() && !maze_bumped()){
      followLine();
    }
  }
  stopAndReport();
}

void startComponents(){
  IR_Start();
  reflectance_start();
  motor_start();
  LSM303D_Start();
}
*/

int* decideMazeTurn(int *xy, int dir, struct sensors_ dig){
  for(int i = 0;i<3;i++){
    turn90deg(0);
    reflectance_digital(&dig);
    if(Ultra_GetDistance()>15 && !(dig.l3|dig.l2|dig.l1|dig.r1|dig.r2|dig.r3)){
      break;
    }
  }
    
  int a = (((dir%4)/2)*2)-1;
  if(dir%2){
    xy[0] += a;
  } else {
    xy[1] += a; 
  }
  return xy;
}

void reportMQTT(char* subtopic, char* data){
  char* topic = strcat("Zumo035/",subtopic);
  print_mqtt(topic," %s",data);
}
/*
void reportMazePosition(){
  char* data;
  sprintf(data,"%d %d",maze_x,maze_y);
  reportMQTT("position",data);
}

void decideMazeTurn(){
  int[4] pastTurns = {0,0,0,0};
  for(int i = 0;i<maze_ix_count;i++){
    if(maze_ix[i][0] == maze_x && maze_ix[i][1] == maze_y){
      
    }
  }
}
*/

void moveToStartLine(){
  while(SW1_Read()){};  // start only after button press
  vTaskDelay(500);
  motor_forward(50,0);
  while(!overLine()){}; // move to line and stop
  motor_stop();
}

/*

void waitAndStart_maze(char* project, int speed){
  reportReady(project);
  IR_wait();
  maze_start = xTaskGetTickCount();
  reportStart(maze_start);
  motor_forward(speed,0);
}

void stopAndReport(){
  motor_stop();
  reportStop(xTaskGetTickCount());
  reportTime(xTaskGetTickCount() - maze_start);
}

bool inIntersection(struct sensors_ dig){
  reflectance_digital(&dig); 
  return (dig.l3&dig.l2&dig.l1&dig.r1&dig.r2&dig.r3)^true;
}

void decideMazeTurn(){
  while(true){
    start = xTaskGetTickCount();
    while(!overLine(dig)){};
    times[i] = (xTaskGetTickCount() - start);
    if(Ultra_GetDistance()<20){
      
    }

    x+=(( dir )%2)*((dir%2)+1-dir);
    y+=((dir-1)%2)*((dir%2)+1-dir);
  }
  
}
*/

void moveToNextLine(struct sensors_ dig){
  while(overLine(dig)){};
  while(!overLine(dig)){};
}

int followLine(struct sensors_ ref, struct sensors_ dig, int offLine){
  float maxSpeed = 200;
  float val      = 0;

  reflectance_read(&ref);
    
  val = ((-29.5*ref.l3)+(-16.0*ref.l2)+(-4.75*ref.l1)+4.75*ref.r1+16.0*ref.r2+29.5*ref.r3)/6.0;

  int pwm1 = maxSpeed+val/50;
  int pwm2 = maxSpeed-val/50;
    
  if(pwm1>255){pwm1=255;}
  if(pwm2>255){pwm2=255;}
  if(pwm1<0){pwm1=0;}
  if(pwm2<0){pwm2=0;}
    
  PWM_WriteCompare1(pwm1);
  PWM_WriteCompare2(pwm2);

  reflectance_digital(&dig);
  if((!dig.l1)&(!dig.r1)&(!offLine)){
    offLine = 1;
    print_mqtt("Zumo035/miss","%d",xTaskGetTickCount());
  } else if((!dig.l1)&(!dig.r1)&offLine){
    offLine = 0;
    print_mqtt("Zumo035/line","%d",xTaskGetTickCount());
  }
  return offLine;
}

float turnToCenter(struct sensors_ ref){
  float maxSpeed = 255;
  float val      = 0;
  float ratio    = 50;
  int pwm        = 0;

  reflectance_read(&ref);
    
  val = ((-29.5*ref.l3)+(-16.0*ref.l2)+(-4.75*ref.l1)+4.75*ref.r1+16.0*ref.r2+29.5*ref.r3)/6.0;

  if(val<0){
    MotorDirLeft_Write(1);
    MotorDirRight_Write(0);
    pwm = -val*ratio;
  } else {
    MotorDirLeft_Write(0);
    MotorDirRight_Write(1);
    pwm = val*ratio;
  }

  if(pwm>255){pwm=255;}
    
  PWM_WriteCompare1(pwm);
  PWM_WriteCompare2(pwm);

  return val;
}

bool overLine(struct sensors_ dig){
  reflectance_digital(&dig); 
  return dig.l3&dig.l2&dig.l1&dig.r1&dig.r2&dig.r3;
}

bool allSensorsBlack(struct sensors_ dig){
  reflectance_digital(&dig);
  return dig.l3&dig.l2&dig.l1&dig.r1&dig.r2&dig.r3;
}

bool anySensorBlack(struct sensors_ dig){
  reflectance_digital(&dig);
  return dig.l3|dig.l2|dig.l1|dig.r1|dig.r2|dig.r3;
}

int blackSensors(struct sensors_ dig){
  reflectance_digital(&dig);
  return dig.l3+dig.l2+dig.l1+dig.r1+dig.r2+dig.r3;
}

/*///////////////////////////// OTHER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

float ADCtoBatteryRatio(float R1,float R2,float V_ref,int ADC_res){
  /* Function that calculates ratio between ADC value and battery voltage
  1 GENERAL EXPLANATION
  1.1  Zumo robot has a analog-to-digital converter (ADC) that returns 12-bit
       values. Values can be converted to voltage with following:

         V_ADC = V_ref * (ADC_val / ((2 ^ ADC_res) - 1)

       ADC in the robot is unable to measure voltage of four AA-batteries
       directly. To work around this, two resistors were set in the circuit,
       one before and one after ADC:

                 +----------->-----------+
                 |                       |
                 |                      |_| R1
             +   |                       |
               -----                     |
       Battery  ---                      +-----< Analog-to-digital converter
               -----                     |
                ---                      |
             -   |                      |_| R2
                 |                       |
                 |                       |
                 +-----------------------+

       Now the battery voltage can be calculated with:

         V_bat = V_ADC / (R2 / (R1 + R2))

  2 PARAMETERS EXPLAINED
  2.1 float R1 : Resistance of the resistor before ADC
  2.2 float R2 : Resistance of the resistor after ADC
  2.3 float V_ref : Reference voltage
  2.4 int ADC_res : Resolution of ADC

  3 RETURN
  3.1 Function returns a float value that is equal to the ratio between battery
      voltage and ADC result. This value can be used to convert ADC value to
      battery voltage by multiplying ADC value with it: ADC_val * ratio.

  4 FUNCTION CALL EXAMPLES
  ADCtoBatteryRatio(5.0,10.0,5.0,12); // Use this for Zumo robot
  ADCtoBatteryRatio(5.0,10.0,5.0,8);  // example with 8-bit ADC

  Parts of this explanation may be referenced with part number in parenthesis.
  */

  float resistance = R2 / (R1 + R2);     // overall resistance
  float resolution = (1 << ADC_res) - 1; // (2^ADC_res)-1 with left-shift

  return V_ref / resolution / resistance;
}

void ledOnFor(uint duration){
  /* Function that turns BatteryLed on for given amount of time.
  1 GENERAL EXPLANATION
  1.1  Zumo robot has a blue led light called BatteryLed here. There
       is a built-in function called BatteryLed_Write that allows
       code writer to turn the led on or off based on the input.
       Inputs and corresponding actions for BatteryLed_Write are:
  1.1.1  1 : turn the led on
  1.1.2  0 : turn the led off
  1.2  Zumo robot has a built in function called vTaskDelay that
       pauses the robot's task for given amount of time. The function
       has one input parameter and it'll set the duration of the delay
       in milliseconds.
  1.3  This function uses the functions explained in 1.1 and 1.2 to do
       the following:
         - Turn BatteryLed on.
         - Wait for given amount of time.
         - Turn BatteryLed off.

  2 PARAMETERS EXPLAINED
  2.1   uint duration : Amount of time in milliseconds to have the
                        led turned on for.
  2.1.1 duration example : 200

  3 LINKED FUNCTIONS
  3.1 BatteryLed_Write(int);
  3.2 vTaskDelay(int);

  4 FUNCTION CALL EXAMPLES
    ledOnFor(400);
    ledOnFor(60);

  Parts of this explanation may be referenced with part number in parenthesis.
  */

  BatteryLed_Write(1);  // turn BatteryLed on (1.1.1)
  vTaskDelay(duration); // wait for given amount of time (1.2)(2.1)
  BatteryLed_Write(0);  // turn BatteryLed off (1.1.2)
}

void blinkMorse(char message[], uint timeUnit, uint repeats){
  /* Function that blinks BatteryLed based on given message.

  1 GENERAL EXPLANATION:
  1.1   Function follows ITU-R recommendation "ITU-R M.1677-1(10/2009)"
        for International Morse code:
  1.1.1   A dash is equal to three dots.
  1.1.2   The space between the signals forming the same letter is equal to
          one dot.
  1.1.3   The space between two letters is equal to three dots.
  1.1.4   The space between two words is equal to seven dots.
  1.1.5   And: Dot is equal to one time unit.
  1.2  Zumo robot has a built in function called vTaskDelay that
       pauses the robot's task for given amount of time. The function
       has one input parameter and it'll set the duration of the delay
       in milliseconds. It is used to simulate what is defined in the
       specification as explained in 1.1.2, 1.1.3 and 1.1.4.

  2 PARAMETERS EXPLAINED:
  2.1     char msg[] : Message in Morse code.
  2.1.1     Four different characters are treated as part of Morse code:
  2.1.1.1     '.' : dot character represents short "dot"
  2.1.1.2     '-' : dash character represents longer "dash"
  2.1.1.3     ' ' : space represents end of character
  2.1.1.4     '|' : pipe represents end of word
  2.1.2     msg example: "-- --- .-. ... .|-.-. --- -.. ."
  2.2     uint timeUnit : Length of dot in milliseconds.
          Shouldn't be less than 250 milliseconds for visual-only
          representation of Morse code because slightest eye movement and
          blinking will lead in loss of data in receiving end. Audio-visual
          and audio-only representation can often have dot length of 60ms.
  2.2.1     timeUnit example: 500
  2.3     uint repeats : Number of times to repeat the message.
          hint: If math.h is included, INFINITY could be set for infinite loop
  2.3.1     repeats example: 2

  3 LINKED FUNCTIONS:
  3.1 ledOnFor(uint duration)
  3.2 vTaskDelay(int duration)

  4 FUNCTION CALL EXAMPLES
  blinkMorse("... --- ...",300,INFINITY); // #include <math.h> required
  blinkMorse("-- --- .-. ... .|-.-. --- -.. .",500,1);

  Parts of this explanation may be referenced with part number in parenthesis.
  */

  uint msgLen = strlen(message); // length of message
  uint location = 0;             // keep track of location within message
  uint gap = 0;                  // number of time units between signals

  for(uint i = 0;i<repeats * msgLen;i++){ // loop each character of each repeat
    location = i % msgLen;      // modulo is needed to get location in message
    switch(message[location]){  // use current location's character as switch
      case '.' :                // '.' represents dot in Morse code (2.1.1.1):
        ledOnFor(timeUnit);     // -> turn led on for 1 time unit (1.1.5)
        gap = 1;                // -> set gap to 1 time units (1.1.2)
        break;
      case '-' :                // '-' represents dash in Morse code (2.1.1.2):
        ledOnFor(timeUnit * 3); // -> turn led on for 3 time units (1.1.1)
        gap = 1;                // -> set gap to 1 time units (1.1.2)
        break;
      case ' ' :                // ' ' represents end of letter (2.1.1.3), so:
        gap = 3;                // -> set gap to 3 time units (1.1.3)
        break;
      case '|' :                // '|' represents end of word (2.1.1.4), so:
        gap = 7;                // -> set gap to 7 time units (1.1.4)
        break;
      default :                 // if some other character than ".- |":
        gap = 0;                // -> set gap to 0
        break;
    }

    if(location == msgLen - 1){ // if end of message:
      gap = 7;                  // set gap to same as end of word (1.1.4)
    }

    vTaskDelay(timeUnit * gap); // wait for gap*timeUnit before next loop (1.2)
  }
}

void playMusic(){
  int intNotes[] = {240,214,127,127,143,143,0,
                    191,180,107,107,120,120,0,
                    180,160,95,95,120,120,160,160,160};
  int delay = 100;
  int size = sizeof(intNotes) / sizeof(int);
  for(int i = 0;i<size;i++){
    Beep(delay,abs(intNotes[i]-255));
  }
}
/*
bool overLine(struct sensors_ dig){
  reflectance_digital(&dig); 
  return dig.l3&dig.l2&dig.l1&dig.r1&dig.r2&dig.r3;
}
*/

void irTest(){
  IR_Start();
  IR_flush();
  while(true){
    IR_wait();
    printf("IR signal received\n");
  }
}

void refTest(){
 
  struct sensors_ dig;
    
  reflectance_start();
  reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
  reflectance_digital(&dig);

  while(true){
    reflectance_digital(&dig);
    printf("%d,%d,%d,%d,%d,%d\n",dig.l3,dig.l2,dig.l1,dig.r1,dig.r2,dig.r3);
  }
}

void accTest(){
  struct accData_ data;
  float xAcc = 0;

  int loopNum = 0;

  while(true){
    LSM303D_Start();
    LSM303D_Read_Acc(&data);
    
    xAcc += data.accX;
    
    if(loopNum%10 == 0){
      
      xAcc /= 10;
      for(uint i=40;i>0;i--){
        if(-xAcc / 400 < i){
          printf(" ");
        } else {
          printf("<");
        }
      }
    
      for(uint i=0;i<40;i++){
        if(xAcc / 400 > i){
          printf(">");
        } else {
          break;
        }
      }
      xAcc = 0;
      printf("\n");
    }
    loopNum++;
  }
}

void mqttTest(){
  srand(time(0));  // initialize rand()
  while(true){
    while(SW1_Read()){};
    printf("Hello, world!,%d\n",rand()%2);// print to screen (PuTTy)
    print_mqtt("Zumo035/output", "asd");// Send message to topic
    vTaskDelay(500);
    while(!SW1_Read()){};
  }
}

void turn90deg(int dir){ // 1 = turn right, 0 = turn left
  int speed = 150;

  MotorDirLeft_Write(dir);
  MotorDirRight_Write(!dir);

  PWM_WriteCompare1(speed);
  PWM_WriteCompare2(speed);

  vTaskDelay(320);
}

void tankTurn(int speed, int dir){ // 1 = turn right, 0 = turn left
  MotorDirLeft_Write(dir);
  MotorDirRight_Write(!dir);

  PWM_WriteCompare1(speed);
  PWM_WriteCompare2(speed);

  vTaskDelay(320);
}

int filterSavitzkyGolay7(int a[]){
  return -2*a[6] + 3*a[5] + 6*a[4] + 7*a[3] + 6*a[2] + 3*a[1] - 2*a[0];
}

void irMsgTest(){
  IR_Start();
  uint32_t IR_val; 
  printf("\n\nIR test\n");
  IR_flush(); // clear IR receive buffer
  printf("Buffer cleared\n");
    
  // print received IR pulses and their lengths
  for(;;)
  {
    IR_flush();
    if(IR_get(&IR_val, portMAX_DELAY)) {
      int l = IR_val & IR_SIGNAL_MASK; // get pulse length
      int b = 0;
      if((IR_val & IR_SIGNAL_HIGH) != 0) b = 1; // get pulse state (0/1)

           
           printf("%d %d %d\r\n",b, l, IR_val);
        
      }
   }    
}

int* shiftArray(int* a){
  int size = sizeof(a) / sizeof(int);
  return a;
}

/* [] END OF FILE */