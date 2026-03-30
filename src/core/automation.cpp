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
   static const int s_sequence_len = 1;  // Only 1 step active for now
   static AutoStep s_sequence[7];        // Still keeping buffer for 7

   /**
    * @brief Helper to build a command struct without messy JSON
    */
   control_command_t build_cmd(float vx, float vy, float omega) {
       control_command_t cmd;
       command_init(&cmd); // Set defaults (type="control", vehicle="mecanum")

       // In mecanum profile: Right Joystick handles translation
       cmd.right.isJoystick = true;
       cmd.right.x = vx;
       cmd.right.y = vy;

       // Left Joystick/Dial handles rotation
       cmd.left.isJoystick = true;
       cmd.left.x = omega;

       cmd.speed = 100; // 100% power
       return cmd;
   }

   void automation_start() {
       // 1. Define the Sequence
       // Forward 1m: ~1274ms @ Max Speed
       // Turn 90:    ~400ms  @ Max Speed

       s_sequence[0] = { build_cmd(0, 100, 0),   100 }; // Forward 1m       
       s_sequence[1] = { build_cmd(100, 0, 0),   100  }; // Left 1m
       s_sequence[2] = { build_cmd(0, 100, 0),   100 }; // Forward 1m
       s_sequence[3] = { build_cmd(100, 0, 0),   100  }; // Left 1m
       s_sequence[5] = { build_cmd(0, 0, -100),  150  }; // Turn 45 Left
       s_sequence[6] = { build_cmd(0, 100, 0),   150 }; // Forward 2.5m

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