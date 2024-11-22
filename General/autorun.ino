#include "Nintendo.h"
/* This code uses the Nicohood Library
 * Use this code at your own risk
 * Code written by Simple Controllers and this code is open source.
 * Meaning its free to share, improve on, or anything you like!
 * Just remember to mention you used my code!
 * Version 2.0 I just suck at github
 */
//This makes the controller bidirection data line on pin number8
CGamecubeConsole GamecubeConsole(8);      //Defines a "Gamecube Console" sending data to the console on pin 8
Gamecube_Data_t d = defaultGamecubeData;   //Structure for data to be sent to console
 
//This is needed but you don't need a controller on pin 7
CGamecubeController GamecubeController1(7);

static unsigned long frameDuration = 1000 / 30; // A "frame" in this case will be 30FPS as the arduino isn't precise enough to do 60FPS
 
void setup()
{
  //This is establishing the pin assignments up there to input pins
 
  //This is needed to run the code.
  GamecubeController1.read();
 
  // Reset all inputs to default
  d.report.a = 0;
  d.report.b = 0;
  d.report.x = 0;
  d.report.y = 0;
  d.report.start = 0;
  d.report.dleft = 0;
  d.report.dright = 0;
  d.report.ddown = 0;
  d.report.dup = 0;
  d.report.z = 0;
  d.report.r = 0;
  d.report.l = 0;
  d.report.xAxis = 128;  // 75 -83
  d.report.yAxis = 128;
  d.report.cxAxis = 128;
  d.report.cyAxis = 128;
  d.report.left = 0;
  d.report.right = 0;

  GamecubeConsole.write(d); 

}
 
void loop() {
  static unsigned long lastChange = 0;
  static int frameCounter = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastChange >= frameDuration) {
    lastChange = currentMillis;
    frameCounter++;

    if (frameCounter >= 4) {
      d.report.b = !d.report.b;
      d.report.xAxis = 0;
    }

    if (frameCounter >= 8) {
      d.report.b = !d.report.b;
      d.report.xAxis = 255;
      frameCounter = 0;
    }
  }
    
  GamecubeConsole.write(d);
}


