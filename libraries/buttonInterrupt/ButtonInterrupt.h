#ifndef ButtonInterrupt_h
#define ButtonInterrupt_h

#include "Arduino.h"


#define switched                            true // value if the button  switch has been pressed
#define triggered                           true // controls  interrupt handler
#define interrupt_trigger_type            RISING // interrupt  triggered on a RISING input
#define debounce                              10  // time to wait in milli secs

volatile  bool interrupt_process_status = {
  !triggered                                     // start with no switch press pending,  ie false (!triggered)
};
bool initialisation_complete =            false;  // inhibit any interrupts until initialisation is complete

#define     button_switch                       3 // external interrupt  pin





void  button_interrupt_handler()
{
  if (initialisation_complete == true)
  {  //  all variables are initialised so we are okay to continue to process this interrupt
    if (interrupt_process_status == !triggered) {
      // new interrupt so okay  start a new button read process -
      // now need to wait for button release  plus debounce period to elapse
      // this will be done in the button_read  function
      if (digitalRead(button_switch) == HIGH) {
        // button  pressed, so we can start the read on/off + debounce cycle wich will
        //  be completed by the button_read() function.
        interrupt_process_status  = triggered;  // keep this ISR 'quiet' until button read fully completed
      }
    }
  }
} 

bool read_button() {
  int button_reading;
  // static variables because we need to retain old values  between function calls
  static bool     switching_pending = false;
  static  long int elapse_timer;
  if (interrupt_process_status == triggered) {
    //  interrupt has been raised on this button so now need to complete
    // the button  read process, ie wait until it has been released
    // and debounce time elapsed
    button_reading = digitalRead(button_switch);
    if (button_reading == HIGH)  {
      // switch is pressed, so start/restart wait for button relealse, plus  end of debounce process
      switching_pending = true;
      elapse_timer  = millis(); // start elapse timing for debounce checking
    }
    if (switching_pending  && button_reading == LOW) {
      // switch was pressed, now released, so check  if debounce time elapsed
      if (millis() - elapse_timer >= debounce) {
        // dounce time elapsed, so switch press cycle complete
        switching_pending  = false;             // reset for next button press interrupt cycle
        interrupt_process_status  = !triggered; // reopen ISR for business now button on/off/debounce cycle complete
        return switched;                       // advise that switch has been pressed
      }
    }
  }
  return !switched; // either no press request or debounce  period not elapsed
}



void Button_init(){
  pinMode(button_switch, INPUT);
  attachInterrupt(digitalPinToInterrupt(button_switch),
                  button_interrupt_handler,
                  interrupt_trigger_type);
  initialisation_complete = true; // open interrupt processing for business
} 



#endif