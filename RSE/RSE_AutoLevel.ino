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

enum State {
  SETUP_FROM_PC,
  MASH_BUTTONS,
  CHECK_PICKUPS,
  FLY_TO_PC,
  COMPLETE,
  HEAL_POKEMON
};

State currentState = SETUP_FROM_PC;   // State that bot will start with
int timesLooped = 0;              // Track how many times bot loops so it knows when to heal
int pickupLoop = 0;              // Track how many turns it has been since last pickup
bool checkPickups = false;        // DO NOT MANUALLY EDIT THIS!! If turns == pickupRate, make this true. 

boolean mash_a = false;
boolean mash_b = false;


// User defined variables: 

int PP = 15;  // Total amount of PP you have for whatever OHKO move you are using. When this reaches zero, bot will heal at PC.
int pickupRate = 3; // Check for pickup items after however many number of battles (set to 0 to never check).
int pickupAmount = 4; // Amount of Pokemon in your party that have pickup. Bot will check from the 6th slot to the 1st slot, so place pickup Pokemon last in your party.
 
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
  d.report.xAxis = 128;
  d.report.yAxis = 128;
  d.report.cxAxis = 128;
  d.report.cyAxis = 128;
  d.report.left = 0;
  d.report.right = 0;

  GamecubeConsole.write(d); 

}
 
void loop() {

  switch (currentState) {
    case SETUP_FROM_PC:
      if (setup_from_pc()) {
        currentState = MASH_BUTTONS;
      }
      break;
      
    case MASH_BUTTONS:
      if (mash_buttons()) {
        currentState = CHECK_PICKUPS;
      }
      break;

    case CHECK_PICKUPS:
      // Check to see if pickups should be retrieved on this turn
      if (pickupRate != 0 and pickupAmount >= 1 and pickupAmount <= 6) {
        pickupLoop++;
        if (pickupLoop >= pickupRate) {
          checkPickups = true;
          pickupLoop = 0;
        }
        else {
          checkPickups = false;
        }
      }
      else {
        checkPickups = false;
      }

      currentState = FLY_TO_PC;
      break;

    case FLY_TO_PC:
      if (fly_to_pc()) {
        currentState = COMPLETE;
      }
      break;

    case COMPLETE:
      timesLooped++;
      if (timesLooped >= (PP - 1)) {
        timesLooped = 0;
        currentState = HEAL_POKEMON;
        break;
      }
      else {
        currentState = SETUP_FROM_PC;
      }
      break;

    // After 15 loops, heal up
    case HEAL_POKEMON:
      if (heal_pokemon()) {
        currentState = SETUP_FROM_PC;
      }
      break;
  }

  GamecubeConsole.write(d);
}


bool mash_buttons() {
  static unsigned long lastChange = 0;
  static int frameCounter = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastChange >= frameDuration) {
    lastChange = currentMillis;
    frameCounter++;

    // Mash A through battle
    if (frameCounter >= 0) {
      mash_a = true;
    }

    if (frameCounter >= 1040) {
      mash_a = false;
      d.report.a = 0;
      d.report.l = 0;
      mash_b = true;
    }

    if (frameCounter >= 1480) {
      mash_b = false;
      d.report.b = 0;
      frameCounter = 0;
      return true;
    }
  }
  
  if (mash_a == true) {
    d.report.a = !d.report.a;
    d.report.l = d.report.a;
  }
  if (mash_b == true) {
    d.report.b = !d.report.b;
  }

  return false;
}


bool setup_from_pc() {
  static unsigned long lastChange = 0;
  static int frameCounter = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastChange >= frameDuration) {
    lastChange = currentMillis;
    frameCounter++;

    // Code starts outside of Lavaridge PokeCenter
    if (frameCounter >= 1) {
      d.report.z = 1;
      d.report.ddown = 1;
    }

    if (frameCounter >= 2) {
      d.report.z = 0;
    }

    if (frameCounter >= 15) {
      d.report.ddown = 0;
      d.report.dright = 1;
    }

    if (frameCounter >= 195) {
      d.report.ddown = 1;
      d.report.dright = 0;
    }

    if (frameCounter >= 228) {
      d.report.ddown = 0;
      d.report.dright = 1;
    }

    if (frameCounter >= 232) {
      d.report.dright = 0;
      d.report.a = 1;
    }

    //If you overshoot Gabby, talk to Ty instead.
    if (frameCounter >= 238) {
      d.report.dup = 1;
      d.report.a = 0;
    }

    if (frameCounter >= 248) {
      d.report.dup = 0;
      d.report.a = 1;
      frameCounter = 0;
      return true;
    }
  }
  return false;
}


bool fly_to_pc() {
  static unsigned long lastChange = 0;
  static int frameCounter = 0;
  unsigned long currentMillis = millis();

  static int additionalFrames = 0;
  static int checkedPickups = 1;

  if (currentMillis - lastChange >= frameDuration) {
    lastChange = currentMillis;
    frameCounter++;
    if (frameCounter >= 0) {
      d.report.start = 1;
    }

    if (frameCounter >= 8) {
      d.report.start = 0;
      d.report.a = 1;
    }

    if (frameCounter >= 10) {
      d.report.a = 0;
    }

    if (checkPickups == true) {
      if (frameCounter >= 49) {
        d.report.dup = 1;
      }

      if (frameCounter >= 52) {
        d.report.dup = 0;
      }

      // Pickup loop starts here
      if (frameCounter >= 55 + additionalFrames) {
        d.report.dup = 1;
      }

      if (frameCounter >= 58 + additionalFrames) {
        d.report.dup = 0;
      }

      if (frameCounter >= 61 + additionalFrames) {
        d.report.a = 1;
      }

      if (frameCounter >= 64 + additionalFrames) {
        d.report.dup = 1;
        d.report.a = 0;
      }

      if (frameCounter >= 67 + additionalFrames) {
        d.report.dup = 0;
      }

      if (frameCounter >= 70 + additionalFrames) {
        d.report.dup = 1;
      }

      if (frameCounter >= 73 + additionalFrames) {
        d.report.dup = 0;
        d.report.a = 1;
      }

      if (frameCounter >= 76 + additionalFrames) {
        d.report.ddown = 1;
        d.report.a = 0;
      }

      if (frameCounter >= 79 + additionalFrames) {
        d.report.a = 1;
        d.report.ddown = 0;
      }

      if (frameCounter >= 82 + additionalFrames) {
        d.report.a = 0;
      }

      if (frameCounter >= 115 + additionalFrames) {
        d.report.a = 1;
      }

      if (frameCounter >= 118 + additionalFrames) {
        d.report.a = 0;
      }

      if (frameCounter >= 121 + additionalFrames and checkedPickups < pickupAmount) {
        additionalFrames += 73;
        checkedPickups++;
      }
      if (frameCounter >= 124 + additionalFrames and checkedPickups >= pickupAmount) {
        d.report.dleft = 1;
        checkPickups = false;
        additionalFrames += 85;
      }
    }

    // When pickup code is done running, run this
    if (checkPickups == false) {
      // Select first party member to fly back
      if (frameCounter >= 49 + additionalFrames) {
        d.report.a = 1;
        d.report.dleft = 0;
      }

      if (frameCounter >= 51 + additionalFrames) {
        d.report.ddown = 1;
        d.report.a = 0;
      }

      if (frameCounter >= 53 + additionalFrames) {
        d.report.ddown = 0;
      }

      if (frameCounter >= 55 + additionalFrames) {
        d.report.ddown = 1;
      }

      if (frameCounter >= 57 + additionalFrames) {
        d.report.a = 1;
        d.report.ddown = 0;
      }

      if (frameCounter >= 58 + additionalFrames) {
        d.report.a = 0;
      }

      cursor_selection(frameCounter, additionalFrames);

      if (frameCounter >= 350 + additionalFrames) {
        d.report.a = 0;
        frameCounter = 0;
        additionalFrames = 0;
        checkedPickups = 1;
        return true;
      }
    }
  }
  return false;
}

void cursor_selection(int frameCounter, int additionalFrames) {
  // Map cursor selection
    // If the bot gets stuck due to pokenav call, etc... it will highlight every possible square and press A over it.
    if (frameCounter >= 93 + additionalFrames) {
      d.report.a = 1;
    }

    if (frameCounter >= 95 + additionalFrames) {
      d.report.a = 0;
      d.report.dleft = 1;
    }

    if (frameCounter >= 97 + additionalFrames) {
      d.report.dleft = 0;
    }

    if (frameCounter >= 105 + additionalFrames) {
      d.report.a = 1;
    }

    if (frameCounter >= 107 + additionalFrames) {
      d.report.a = 0;
      d.report.dleft = 1;
    }

    if (frameCounter >= 109 + additionalFrames) {
      d.report.dleft = 0;
    }

    if (frameCounter >= 117 + additionalFrames) {
      d.report.a = 1;
    }

    if (frameCounter >= 119 + additionalFrames) {
      d.report.a = 0;
      d.report.dleft = 1;
    }

    if (frameCounter >= 121 + additionalFrames) {
      d.report.dleft = 0;
    }

    if (frameCounter >= 129 + additionalFrames) {
      d.report.a = 1;
    }
}

bool heal_pokemon() {
  static unsigned long lastChange = 0;
  static int frameCounter = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastChange >= frameDuration) {
    lastChange = currentMillis;
    frameCounter++;
    if (frameCounter >= 1) {
      d.report.dup = 1;
    }

    if (frameCounter >= 115) {
      d.report.a = 1;
      d.report.dup = 0;
      mash_a = true;
    }

    if (frameCounter >= 220) {
      mash_a = false;
      d.report.a = 0;
      d.report.l = 0;
      mash_b = true;
    }

    if (frameCounter >= 490) {
      mash_b = false;
      d.report.b = 0;
      d.report.ddown = 1;
    }

    if (frameCounter >= 570) {
      d.report.ddown = 0;
    }

    if (frameCounter >= 610) {
      return true;
    }
  }

  if (mash_a == true) {
    d.report.a = !d.report.a;
    d.report.l = d.report.a;
  }
  if (mash_b == true) {
    d.report.b = !d.report.b;
  }

  return false;
}
