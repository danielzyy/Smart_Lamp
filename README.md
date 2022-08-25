# Smart_Lamp
Smart lamp built with Adafruit Circuit Playground Express board.

Three modes (manual light, sound light, and alarm) are cycled through by pressing the mode button on the back left of the lamp.
The slide switch on the board toggles between two light modes(solid ring color cycle and rainbow ring cycle).
The left and right buttons on the board are also used in the sound light and alarm modes.
## Modes
### Manual Light (Mode 0)
<b>Two lighting modes: </b> <br>
| Solid Ring Color Cycle (Slide switch to the left) | Rainbow Ring Cycle (Slide switch to the right) |
| ----------- | ----------- |
|<img src="https://user-images.githubusercontent.com/15254803/184299737-0dffe765-6e4c-4a94-9160-db9b343487ed.gif" height="400"/> | <img src="https://user-images.githubusercontent.com/15254803/184301622-cdbfe228-2146-43aa-905e-52bed3ea5fdb.gif" height="400" /> |

<b>Brightness Control Using Rotary Encoder</b>
* Turning left increases brightness of lamp
* Turning right decreases brightness of lamp
* Pressing on the rotary encoder pauses/resumes the color cycle in solid ring color cycle mode.

### Sound Light (Mode 1)
Lights are triggered by noise that exceeds the sound limit. The sound limit is displayed at the top of the display and can be increased or decreased using the left and right buttons on the board.

Brightness of the lights are still controlled by the rotary encoder(same as Mode 0).

### Alarm (Mode 2)
Alarm time is set with the left and right buttons on the board (left button increases hours and right button increases minutes). Pressing on the encoder turns the alarm on. The status of the alarm is displayed at the top of the screen (ON/OFF). <b> Must switch back to Mode 0 or 1 after turning on alarm for it to ring. </b><br>

To turn off ringing alarm, hold down the encoder button until ringing stops.

## Circuit
<img src="https://user-images.githubusercontent.com/15254803/186573084-82e34de7-197e-450d-96d5-ca49f7a47319.jpg" height="400"/> <img src="https://user-images.githubusercontent.com/15254803/186573091-16dce85a-345d-4d9b-b3a4-d13f73d945a4.jpg" height="400"/>

## 3D Model Assembly
* Top compartment slides over Adafruit board to diffuse light
* Middle compartment holds all electronic components except for the RTC
* Bottom compartment holds hte RTC and any batteries needed, with cables routed through to the middle compartment
* All parts were 3D printed with 25% density

<img src="https://user-images.githubusercontent.com/15254803/184983535-7d7c02c1-c186-4b3e-9060-a8059c3d2e23.png" height="450"/><img src="https://user-images.githubusercontent.com/15254803/184984303-904ea797-697c-458e-8195-fa05617e17aa.png" height="450"/>
