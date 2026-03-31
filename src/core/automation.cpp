 #include "automation.h"
   #include "command_packet.h"
   #include "project_config.h"
   #include "profiles/profile_mecanum.h"
   #include "core/safety.h"

   // Structure to pair a command with its required time
   struct AutoStep {
       control_command_t cmd;
       unsigned long duration_ms;
   };

   // Internal state variables
   static int s_current_step = -1;       // -1 = Idle/Off
   static unsigned long s_step_start_ms = 0;
   static const int s_sequence_len = 5;  // 5 steps in the sequence
   static AutoStep s_sequence[s_sequence_len];

   /**
    * @brief Helper to build a command struct that matches the BLE JSON format
    */
   control_command_t build_cmd(float vx, float vy, float omega) {
       control_command_t cmd;
       command_init(&cmd);

       // Match JSON: {"type":"control","vehicle":"mecanum",...}
       strcpy(cmd.type, "control");
       strcpy(cmd.vehicle, "mecanum");

       // Match JSON: "right":{"control":"joystick","x":vx,"y":vy}
       cmd.right.isJoystick = true;
       cmd.right.x = vx;
       cmd.right.y = vy;
       cmd.right.value = 0.0f;

       // Match JSON: "left":{"control":"dial","value":omega}
       cmd.left.isJoystick = false; // App uses dial for rotation
       cmd.left.x = 0.0f;
       cmd.left.y = 0.0f;
       cmd.left.value = omega;

       // Match JSON: "speed":60
       cmd.speed = 60; 

       // Initialize arrays to zero
       for(int i=0; i<6; i++) {
           cmd.aux[i] = 0.0f;
           cmd.toggles[i] = false;
       }

       return cmd;
   }

   void automation_start() {
       // 1. Define the Sequence
       // vy > 0: Forward, vx < 0: Left
       // Assuming 2000ms per meter at current speed settings
       
       s_sequence[0] = { build_cmd(0, 100, 0),    2000 }; // 1m Forward
       s_sequence[1] = { build_cmd(-100, 0, 0),   2000 }; // 1m Left
       s_sequence[2] = { build_cmd(0, 100, 0),    2000 }; // 1m Forward
       s_sequence[3] = { build_cmd(-100, 0, 0),   2000 }; // 1m Left
       s_sequence[4] = { build_cmd(-100, 100, 0), 2828 }; // sqrt(2)m at 45 deg (Left + Forward)

       s_current_step = 0;
       s_step_start_ms = millis();
       Serial.println("[AUTO] Sequence Started");
   }

   void automation_stop() {
       s_current_step = -1;

       // Send a "Stop" command to the motors
       control_command_t stop_cmd = build_cmd(0, 0, 0);
       profile_mecanum_apply(&stop_cmd);

       Serial.println("[AUTO] Sequence Finished/Stopped");
   }

   bool automation_is_active() {
       return s_current_step >= 0;
   }

    void automation_tick() {
        if (!automation_is_active()) return;

        // MANDATORY: Keep the safety watchdog fed!
        safety_feed();

        unsigned long now = millis();
        AutoStep& current = s_sequence[s_current_step];

        // Apply the current command to the motor profile
        // This pipes the data into the exact same path as the BLE commands
        profile_mecanum_apply(&current.cmd);

        // Check if it's time to move to the next step
        if (now - s_step_start_ms >= current.duration_ms) {
            s_current_step++;
            s_step_start_ms = now;

            // Check if we finished the last step
            if (s_current_step >= s_sequence_len) {
                automation_stop();
                return;
            }
            Serial.printf("[AUTO] Moving to Step %d\n", s_current_step);
        }
    }