#ifndef AUTOMATION_H
   #define AUTOMATION_H

   #include <Arduino.h>

   /**
    * @brief Start the automated sequence from the beginning.
    */
   void automation_start();

   /**
    * @brief Stop the sequence and zero the motors.
    */
   void automation_stop();

   /**
    * @brief Check if the robot is currently in autonomous mode.
    */
   bool automation_is_active();

   /**
    * @brief The "Brain" of the automation. Call this in the main loop().
    *
    * This function handles the timing and state transitions between
    * movements without blocking the rest of the system.
    */
   void automation_tick();

#endif