// #include "automated.h"
// #include "command_packet.h"
// #include <Arduino.h> // so I can access millis() clock time from Pico
// #include <vector>

// #define overall_speed 0.5 // [-1, 1], for tuning so everything is executed slower

// // typedef struct {
// //   char type[16];                /**< Command type string: "control" for motor
// //                                      commands, "heartbeat" for keep-alive pings
// //                                      that tell the safety watchdog the app is
// //                                      still connected. */
// //   char vehicle[16];             /**< Vehicle configuration string. Must be one
// //                                      of: "mecanum", "fourwheel", "tracked", or
// //                                      "dual". This determines how the left/right
// //                                      inputs are interpreted. */
// //   joystick_input_t left;        /**< Left control input from the app.
// //                                      @see joystick_input_t for field details. */
// //   joystick_input_t right;       /**< Right control input from the app.
// //                                      @see joystick_input_t for field details. */
// //   uint8_t speed;                /**< Speed multiplier from the app's speed
// //                                      slider, 0-100%. Applied as a scaling factor
// //                                      to all motor outputs. 100 = full speed. */
// //   float aux[AUX_CHANNEL_COUNT]; /**< Auxiliary control channels (6 spare
// //                                      sliders). Range: -100 to +100 each. Map
// //                                      these to custom functions in your code. */
// //   bool toggles[TOGGLE_COUNT];   /**< Toggle switch states (6 on/off switches).
// //                                      Use for features like headlights or mode
// //                                      selection. */
// // } control_command_t;


// std::vector<control_command_t, 6> automated_command_sequence; // create empty fixed size array of command structures from 'command_packet.h'

// // return duration needed to command car for specific command
// unsigned long distance_integral(int distance, int velocity) // velocity [-100, 100], must tune
// {
//     unsigned long duration_param = distance / velocity; // unsigned long because millis() uses it to prevent overflow
//     return duration_param;
// }

// int automated_step = 0; // which command in sequenc are we doing?
// unsigned long step_start_time = 0; // When did we start the CURRENT step?
// bool automation_mode = false;        // Is the automation toggle (channel 1 on app) turned on?


// void execute_automated_step()
// {
//     if (!automation_mode) return;

//     unsigned long now = millis(); // time since microcontroller 'woke up'
    
//     control_command_t = automated_command_sequence[i]; // extract corresponding structure
//     bool target_reached = false;
//     while(!target_reached)
//     {

//         for (int i = 0; i < 5; i++) // iterate over sequence of command structures
//         {
//             unsigned long current_time = millis();
//             unsigned long duration = distance_integral(1, overall_speed);
//             while(millis() < duration)
//             {

//             }
//         }
//     }
// }




// int main()
// {
//     for (int i = 0; i < 5; i++)
//     {
//         struct current_control_command_t = automated_command_sequence[i];
//         execute_automated_step(current_control_command_t);
//     }
// }

