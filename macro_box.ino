//================================================================================================================
// Setup
#include <Keyboard.h>
#include <Encoder.h>

// Define the pin connections for your keys, toggle switches and rotary encoders.
// Don't touch this unless you are re-wiring something.
                 //{A1, A2, A3, A4,B1, B2, B3, B4}   <--macro pad keys
const int keys[] = {A0, A2, A3, 1, A1, A4, A5, 0}; //<-- arduino digital I/O pins
const int toggleSwitches[] = {4, 5}; // Mode1/2, Macro/Fidget
Encoder cursor1(7, 9);
Encoder cursor2(11, 12);
//================================================================================================================
// How to program a macro for the SHACK BOX:
// Macros are stored as arrays, which define the sequence of key commands.
// The arrays correspond to the keys on the SHACK BOX as follows

// macros[13][x] = {
// A1 key, 
// A2 key, 
// A3 key,
// A4 key,
// B1 key, 
// B2 key, 
// B3 key, 
// B4 key,
// Curser1 knob press,
// Curser1 CW turn,
// Curser1 CCW turn,
// Curser2 CW turn,
// Curser2 CCW turn,
//}

// x just has to be the length of the longest macro sequence array (because arrays are dumb)

// For key definitions see:
// https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardmodifiers/
//
// WARNING: Be careful when testing keyboard macros! If you goof up, it can put your PC in a weird state 
// where a button is pressed and isn't released or something.  
//================================================================================================================
// MODE 1 CONFIGURATION:
//----------------------------------------------------------------------------------------------------------------
// Define macros
const int macros[12][20] = {
  {KEY_LEFT_CTRL, 'c'}, //A1       
  {KEY_LEFT_CTRL, 'v'}, //A2
  {KEY_LEFT_CTRL, 'x'}, //A3 
  {KEY_LEFT_GUI, KEY_LEFT_SHIFT, 's'}, //A4
  {KEY_LEFT_CTRL, 'g'}, //B1
  {KEY_LEFT_CTRL, 'z'}, //B2
  {KEY_LEFT_CTRL, 'y'}, //B3
  {KEY_LEFT_CTRL, 's'}, //B4
  {KEY_LEFT_ARROW}, //Cursor1 CCW
  {KEY_RIGHT_ARROW}, //Cursor1 CW
  {KEY_UP_ARROW}, //Cursor2 CCW
  {KEY_DOWN_ARROW} //Cursor2 CW
};
const int macroLengths[12] = {2, 2, 2, 3, 2, 2, 2, 2, 1, 1, 1, 1};  // the length of each macro
//----------------------------------------------------------------------------------------------------------------
// MODE 2 CONFIGURATION:
//----------------------------------------------------------------------------------------------------------------
// Define macros
const int altMacros[12][20] = {
  {KEY_LEFT_GUI, 'd'}, //A1       
  {KEY_LEFT_CTRL, 'p'}, //A2
  {KEY_LEFT_CTRL, KEY_TAB}, //A3 
  {KEY_LEFT_GUI, KEY_TAB}, //A4
  {KEY_F5}, //B1
  {KEY_LEFT_CTRL, '-'}, //B2
  {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, '+'},//B3
  {KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_DELETE}, //B4
  {KEY_LEFT_ARROW}, //Cursor1 CCW
  {KEY_RIGHT_ARROW}, //Cursor1 CW
  {KEY_UP_ARROW}, //Cursor2 CCW
  {KEY_DOWN_ARROW} //Cursor2 CW
};
const int altMacroLengths[12] = {2, 2, 2, 2, 1, 3, 3, 3, 1, 1, 1, 1};  // the length of each macro 
//================================================================================================================
//================================================================================================================
// Don't touch this code.
void setup() {
  // Initialise the keyboard
  Keyboard.begin();
  // Configure the keys and toggle switches as inputs with pullup resistors
  for (int i = 0; i < 8; i++) {
    pinMode(keys[i], INPUT_PULLUP);
  }
  // Configure 
  for (int i = 0; i < 2; i++) {
    pinMode(toggleSwitches[i], INPUT_PULLUP);
  }
}
//================================================================================================================
// Main program:

void execute_macro(int idx, const int macro_arr[][20], const int macro_length_arr[]){
  for (int j = 0; j < macro_length_arr[idx]; j++) { // iterate through the macro sequence
     // If the key is CTRL or SHIFT or ALT or OS-logo, press but don't release it
     if ((macro_arr[idx][j] == KEY_LEFT_CTRL) || (macro_arr[idx][j] == KEY_LEFT_SHIFT) || (macro_arr[idx][j] == KEY_LEFT_ALT) || (macro_arr[idx][j] == KEY_LEFT_GUI)) { 
        // If the key is CTRL or SHIFT or ALT or OS logo, press but don't release it
        Keyboard.press(macro_arr[idx][j]);
        }
        else {
        // If the key code is not a control key, press it and then release all keys
        if (idx > 8){ // but if its a cursor it needs to be a write. 
           Keyboard.write(macro_arr[idx][j]);
        }
        else{
        // If the key is CTRL or SHIFT or ALT or OS logo, press but don't release it
        Keyboard.press(macro_arr[idx][j]);
        Keyboard.releaseAll();
        }}
}}

void loop() {
  if (digitalRead(toggleSwitches[1]) == LOW) { // Enable MACRO mode (otherwise the box does nothing in FIDGET mode)
  // Check each key
  for (int i = 0; i < 8; i++) {
    if (digitalRead(keys[i]) == LOW) { // If the key is pressed (input is LOW because of the pullup resistor), send a key press
      if (digitalRead(toggleSwitches[0]) == LOW) { // but first check what mode we're in.  LOW -> MODE1
        // MODE 1
        execute_macro(i, macros, macroLengths);
      }
      else {
        // MODE 2
        execute_macro(i, altMacros, altMacroLengths);    
      }
      delay(200);  // crude debouncing so it doesn't spam when you press a key.
      }
    }
      //---------------------------------------------------------------------
      // Cursor functionality:
      //---------------------------------------------------------------------
   
      // Check cursor1 
      int cursor1Position = cursor1.read();
      if (cursor1Position > 3) { // Turning Cursor 1 CCW (3 pulses per knob click)
        if (digitalRead(toggleSwitches[0]) == LOW) {
          execute_macro(8, macros, macroLengths);
          cursor1.write(0); // reset cursor value
        }
        else {
           execute_macro(8, altMacros, altMacroLengths);
           cursor1.write(0); // reset cursor value
      }}
      else if (cursor1Position < -3) { // Turning Cursor 1 CW (3 pulses per knob click)
        if (digitalRead(toggleSwitches[0]) == LOW) {
          execute_macro(9, macros, macroLengths);
          cursor1.write(0); // reset cursor value
        }
        else {
           execute_macro(9, altMacros, altMacroLengths);
           cursor1.write(0); // reset cursor value
      }}
     // Check cursor2 
     int cursor2Position = cursor2.read();
       if (cursor2Position > 3) { // Turning Cursor 2 CCW (3 pulses per knob click)
        if (digitalRead(toggleSwitches[0]) == LOW) {
        execute_macro(10, macros, macroLengths);
        cursor2.write(0); // reset cursor value
        }
        else {
           execute_macro(10, altMacros, altMacroLengths);
           cursor2.write(0); // reset cursor value
      }}
      else if (cursor2Position < -3) { // Turning Cursor 2 CW (3 pulses per knob click)
        if (digitalRead(toggleSwitches[0]) == LOW) {
        execute_macro(11, macros, macroLengths);
        cursor2.write(0); // reset cursor value
        }
        else {
           execute_macro(11, altMacros, altMacroLengths);
           cursor2.write(0); // reset cursor value
      }}
  }
 }

 
