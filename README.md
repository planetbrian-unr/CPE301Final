# CPE 301 Final Project
**Group name:** As You Can See

**Members:** Timothy Ang, Randall Cheng, Darren Ly, Brian Wu

**Due Date:** May 10, 2024

![](/media/circuit.jpeg)

## Modified FAQs:
**Commits:** To upload our work, upload files via Github.

**Design:** Test your circuit for all conditions to ensure all stages work. Submerge the water sensor/hold the temperature sensor to test them.

**Vent:** A vent is unnecessary; affix something to the stepper motor and activate it when required.

**Fan Motor:** Speed manipulation isn't required (no need for `analogWrite()`). Connect the speed pin to Vcc/set it HIGH for max speed.

**ISR:** Implement an ISR for the start/stop button. [`attachInterrupt()`](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/) can be used (refer to the link for the specific digital pins that will work; additional resource [here](https://webcampus.unr.edu/courses/108762/files/13151854)).

**1-min Delay:** ONLY for the 1-min delay you can use [`millis()`](https://docs.arduino.cc/built-in-examples/digital/BlinkWithoutDelay/).
