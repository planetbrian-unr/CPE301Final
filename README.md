# CPE 301 Final Project
**Group name:** TBD

**Members:** Timothy Ang, Randall Cheng, Darren Ly, Brian Wu

**Project overview (Overleaf):** [edit](https://www.overleaf.com/6391323233fnwcwtbzqymh#390bff), [view-only](https://www.overleaf.com/read/dsyhzghzqyfx#209be3)


## Modified FAQs:
**Commits:** To upload our work, just upload the .ino file (among other things) on Github (Add File > Upload Files). No need to `git clone` or `git commit`.

**Design:** No need to build an outer body for the cooler. Test your circuit for all conditions to ensure all stages work properly. Sprinkle water to check the water sensor or hold the temperature sensor to adjust the temperature.

**Vent:** No need to build vent, just activate stepper motor when required to based on the project description. Affix something to the motor to verify it's moving.

**Motors:** For the fan motor, refer to the [Motors slides](https://webcampus.unr.edu/courses/108762/modules/items/3149526), and TinkerCad link/circuit for ideas. Speed manipulation isn't required (no need for `analogWrite()`), so connect the speed pin to Vcc or set it HIGH for max speed.

**ISR:** Implement your own ISR for the start button. You can use [`attachInterrupt()`](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/) to accomplish this (refer to link for the specific digital pins that will work). Check this [example](https://webcampus.unr.edu/courses/108762/files/13151854) from Canvas.

**1-min Delay:** ONLY for the 1-min delay you can use [`millis()`](https://docs.arduino.cc/built-in-examples/digital/BlinkWithoutDelay/).