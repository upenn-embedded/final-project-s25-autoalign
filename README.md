# AutoAlign

and a target that is covered in pressure sensors which send data back to the motors of the stand to readjust its positioning.

- Team Number: 3
- Team Name: AutoAlign
- Team Members: Ayan Bhatia, Matilda Dingemans, Destynn Keuchel
- GitHub Repository URL: [https://github.com/upenn-embedded/final-project-s25-autoalign](https://github.com/upenn-embedded/final-project-s25-autoalign)
- GitHub Pages Website URL: [for final submission]

  <div style="page-break-after: always;"></div>

## Final Project Proposal

### 1. Abstract

Our project is an adaptive targeting system that uses pressure sensor feedback to automatically realign a motor-controlled platform that holds a nerf gun toward the center of a target. It will consist of two functional pieces that communicate with eachother. The first will be a Nerf Toy Gun stand that can move where the Nerf Gun points up, down, left, and right. The second will be a target that is covered in pressure sensors that sends information back to the stand to so it realigns itself for the next shot to be more accurate.

### 2. Motivation

_What is the problem that you are trying to solve? Why is this project interesting? What is the intended purpose?_

The problem we are solving is the aiming of a projectile, and the purpose of our project will be to orient a nerf gun to hit a target. We are doing so by controlling a nerf gun with a 2-axis motor system informed by feedback from a peripheral target device.

  <div style="page-break-after: always;"></div>

### 3. System Block Diagram

![image](Block.png)

  <div style="page-break-after: always;"></div>

### 4. Design Sketches

The motor stand will be 3D printed. We will use Solidworks to design our stand and the RPL to print. We will laser cut wood for the target to place the pressure sensors on.

Motor Stand:
![image](Drawing1.jpeg)

Target:
![image](Drawing2.jpg)

  <div style="page-break-after: always;"></div>

### 5. Software Requirements Specification (SRS)

- Coordinates of impact sent from peripheral ATMega to principal ATMega
- Impact on pressure sensor must ping an interrupt in the peripheral ATMega
- The principal ATMega controls directional motors via PWM
- Principal ATMega uses a timer to pull the trigger on the nerf gun after a set amount of time of the peripheral recieving input

**5.1 Definitions, Abbreviations**

Here, you will define any special terms, acronyms, or abbreviations you plan to use for hardware

PWM - Pulse Width Modulation
SPI - Serial Peripheral Interface
RF - Radio Frequency
LDC - Liquid Crystal Display

**5.2 Functionality**

| ID     | Description                                                                                                                                                                                      |
| ------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| SRS-01 | Impact on pressure sensor will ping an interrupt in the peripheral ATMega                                                                                                                        |
| SRS-02 | Peripheral ATMega sends pressure sensor coordinates to principal ATMega via SPI RF modules (backup will be using direct wire communication via pin-change interrupts on the principal ATMega)    |
| SRS-03 | The principal ATMega controls directional motors via PWM                                                                                                                                         |
| SRS-04 | Principal ATMega uses a timer to pull the trigger on the nerf gun after a set amount of time (for the motors to adjust) of the peripheral receiving input and outputting to the principal ATMega |
| SRS-05 | Principal ATMega will use SPI to communicate with the LCD display                                                                                                                                |

  <div style="page-break-after: always;"></div>

### 6. Hardware Requirements Specification (HRS)

**6.1 Definitions, Abbreviations**

Here, you will define any special terms, acronyms, or abbreviations you plan to use for hardware

**6.2 Functionality**
| ID | Description |
| ------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| HRS-01 | Pressure sensors will be used on the peripheral target that will signal force from projectile to peripheral ATMega |
| HRS-02 | Display connected to principal ATMega will display location and distance from center of recieved impacted projectile|
| HRS-03 | Two electronic motors will be used to control the pitch and yaw |
| HRS-04 | An electronic motor will be used to "press" the trigger of the nerf gun (or triggering mechanism will be hardwired to the ATMega) |
| HRS-05 | Nerf gun will launch projectile towards the peripheral target |

  <div style="page-break-after: always;"></div>

### 7. Bill of Materials (BOM)

_What major components do you need and why? Try to be as specific as possible. Your Hardware & Software Requirements Specifications should inform your component choices._

_In addition to this written response, copy the Final Project BOM Google Sheet and fill it out with your critical components (think: processors, sensors, actuators). Include the link to your BOM in this section._

We need two AT32Mega boards, one on the principle and one on the peripheral. Each need to also have an RF transceiver to communicate with each other. On the principle, we need an LCD screen and driver. We also need three motors, one for up/down, one for left/right, and one for the trigger. We also need a power management system for these motors, ideally a buck converter that connects to a wall outlet. On the peripheral, we need 17 pressure sensors, 9 small ones and 8 large ones to create our target pressure grid.

[https://docs.google.com/spreadsheets/d/1tAK2J1TNvOPft6oS3_Q8MS1eIg2bzJUsa1MUXQpc5uQ/edit?usp=sharing](https://docs.google.com/spreadsheets/d/1tAK2J1TNvOPft6oS3_Q8MS1eIg2bzJUsa1MUXQpc5uQ/edit?usp=sharing)

### 8. Final Demo Goals

_How will you demonstrate your device on demo day? Will it be strapped to a person, mounted on a bicycle, require outdoor space? Think of any physical, temporal, and other constraints that could affect your planning._

We can do our final demo in the hallway outside of Detkin. We would probably want to place our mount system on top of a table on one side of the hallway, and then attach our target system on top of another table on the other side. Our project would also be able to function on the ground.

  <div style="page-break-after: always;"></div>

### 9. Sprint Planning

_You've got limited time to get this project done! How will you plan your sprint milestones? How will you distribute the work within your team? Review the schedule in the final project manual for exact dates._

| Milestone  | Functionality Achieved                                                                                                                                                                                                                  | Distribution of Work                                                                                                                                                          |
| ---------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Sprint #1  | Prototype of physical stand (completed on solidworks and 3D printed), target backing laser cut, parts ordered recieved, pressure sensors recieved and tested, motors recieved and tested, decide whether the nerf gun can be hard wired | Ayan: Physical Stand Prototype Matilda: Target Backing and order parts, Destynn: Nerf Gun Hardwire, Test Sensors                                                              |
| Sprint #2  | preliminary prototype of moving aim stand, preliminary prototype of peripheral target with pressure sensors attached, wifi/communication modules recieved and tested                                                                    | Ayan: Preliminary Prototype of Moving Stand, Matilda: Preliminary Prototype of Peripheral Target with pressue sensors, Destynn: Wifi Communication + helping Matilda and Ayan |
| MVP Demo   | full prototype of moving aim stand, full prototype of peripheral target, wifi/communication modules incorporated, communication between target pressure sensors and moving stand motors                                                 | All together                                                                                                                                                                  |
| Final Demo | Tweaking accurary, fixing current problems, ensuring everything consistently works                                                                                                                                                      | All together                                                                                                                                                                  |

**This is the end of the Project Proposal section. The remaining sections will be filled out based on the milestone schedule.**

## Sprint Review #1

### Last week's progress

Ayan: Solidworks of the wedge and the mount. We decided that since the gun magazine was angled downwards, we wanted to make a wedge that can make gun fire straight instead of down. Helped build the target zone

Matilda: Ordered parts. Created a prototype target zone to do tests on and had the ATMega to print something to the terminal if the bullet hit the zone.

Destynn: Worked with RF sensors and understood how to work them and how commuication between two modules is transmitted. Helped design the new target prototype and construct it.

All of us decided that we were going to use 17 buttons over 17 pressure sensors. We thought of a new design with 4 springs and a button underneath a pad of acrylic/wood that would push the button down lightly underneath the pad. Matilda made the first prototype of the pad with the springs and tested different buttons to see which one was the best. Since the pressure sensors were originally going to be our ADC component, we are going to use buttons instead with a resistive divider circuit to complete the ADC component. Each button will be assigned to a different voltage and we will convert using ADC on the ATMega.

WORKING ON/PROTOTYPE IMAGE OF TARGET ZONE PROTOTYPE:
![image](targetSolid.png)
![image](workTarget.png)
![image](workTarget2.png)
![image](targetPrototype.png)
![image](targetPrototype2.png)

Here is a video of the target prototype working 80% of the time. We will need to get new softer springs as well as ensure the target hit prints every hit, and just once.

Video: [https://drive.google.com/file/d/1j0xhVTu1WuUeO6oQJYjVxc8Wiqz5kmaa/view?usp=sharing](https://drive.google.com/file/d/1j0xhVTu1WuUeO6oQJYjVxc8Wiqz5kmaa/view?usp=sharing)

IMAGE OF STAND/WEDGE IN SOLIDWORKS:
![image](standSolid.png)

RF Module:
![image](RF.png)

### Current state of project

We currently have a target zone prototype. It uses four springs in the corners of two sandwiched laser cut 1/8" acrylic pieces. The back acrylic piece has a hole in the middle that holds a button. When the gun fires at the front piece of a acrylic, the springs compresses and the button presses. With that we are then able to record whether the button has been pressed by the bullet. Our current prototype works about 80% of the time. The other 20% it does not register the hit. We will have to mess around with which springs to use, how tall to make them, and how close to place the button to the top pad. The code might also be an issue somehow, but we doubt it. There will eventually be 17 target zones. The 8 out on the outside will be bigger while the 9 on the inside will be smaller.

Ayan also created a Solidworks file of our stand/wedge that will hold the motors and toy. It has yet to be lazer cut.

We plan to order some new springs for our next prototype for the target. We also need something for the trigger of the toy, such as another motor. We will order these componenets soon.

### Next week's plan

Matilda: Tweak the current prototype to work at least 95% of the time. Build more target zones and assemble full target. Done means fully glued and constructed. Work with Destynn to use ADC to get input from different buttons. Done means ATMega can recognize different parts of the target seperately. Construction will take a few hours and then working with Destynn will take another few hours to connect the full target prototype to the ATMega.

Destynn: Build resistive divider circuit for buttons for target peripheral. Done means working in conjunction with the target Matilda will construct. Continue working on RF (Matilda will help). Resistive Circuit won't take long, doing the ADC for reading the voltages will take a little longer to ensure each target is correctly read and accurate.

Ayan: Print and Assemble Motor Stand. Attach to ATMega to control motors. Done means correct up and down movement. This will take a few seperate planning and work sessions.

<div style="page-break-after: always;"></div>

## Sprint Review #2

### Last week's progress

This week, a lot was completed!

First, Ayan printed out the Nerf toy stand, which houses both the motors and the Nerf gun, and once we connect it up, it will be able to have a full range of motion in a certain direction. It has holes for the wires, and it houses the gun nicely. Here is an image of the motor stand as well as a Google Drive link to a video that demonstrates its functionality:

![image](nerfStand1.png)
![image](nerfStand2.png)
Link to video of it moving: [https://drive.google.com/file/d/1VINAu2DPriv7eX9o841xM5F6CaizVxNs/view?usp=sharing](https://drive.google.com/file/d/1VINAu2DPriv7eX9o841xM5F6CaizVxNs/view?usp=sharing)

Something we noted is that the stand may need to be expanded to counterweight the gun’s weight. Right now, it tilts a little because the gun hangs on a certain side where the motor connects. If we make the base wider, then it will balance better. Overall, for a first prototype, though, it works nicely.

Next, we laser cut all of the components for the target. The backing is 3 separate pieces (simply because it was too big for one sheet of acrylic), and it has 17 holes in it which signify where the buttons will sit.

![image](targetBack.png)

Here is the front of the target. It is not assembled yet. Each piece will sit on four springs.

![image](targetFront.png)

Then, when the bullet is fired at each square, a certain button will be pressed down.

![image](buttonInSquare.png)

A current issue we are facing is the ability of the gun to push the springs down completely. An idea we’ve had is to laser cut a small flat piece of acrylic that can be glued to the buttons to make them sit closer to the front pieces. That means that the bullet doesn’t have to exert as much force to press the button down. The top slab will essentially be touching the top of the button, so if it hits it, it will definitely register.

Next, as mentioned last week, we decided to use all buttons and still want to use ADC, so we settled on a resistive divider. Each button pressed will signify a different voltage. Since we didn’t want to step down 17 buttons, we've decided to use 3 ADC pins. This also allows us to register if the bullet hits between squares. No two ADC pins will touch (and the different pins are signified by the color of their front panel, so no two colors touch). So, if two buttons are pressed down because the bullet hit between them, we will be able to register that and move the motor accordingly.

Matilda worked on a schematic and prototype of this resistive divider, making sure it could print the different button presses to the serial monitor. Here is the schematic for one of the setups:

![image](ResistiveDividerSchematic.png)

The red panels (since there are 8 of them) will need 9 resistors, which matches the schematic above. The yellow and white will both need 5 resistors, and the center piece will be its own GPIO pin for the bullseye. Here is the prototype with 8 buttons:

![image](resistiveDivider.png)

The breadboard setup is the prototype, while the green perfboard will be in the final design. Here is a video showing that the buttons each show different signals in the serial monitor.

Link: [https://drive.google.com/file/d/1IZJQeoeH9H_dqRwRdRc7nax1aOv_U5zq/view?usp=sharing](https://drive.google.com/file/d/1IZJQeoeH9H_dqRwRdRc7nax1aOv_U5zq/view?usp=sharing)

Then, the green board is all 3 ADC pin stages on one board. One with 9 resistors, and two with 5. It has been soldered.

![image](resistiveFront.png)
![image](resistiveBack.png)

What needs to be added to this is stripped wires that will be sent out to each button at each stage of the divider. Each button will be soldered with two longer wires and then placed into the target and the board, and then connected to the ATmega as well.

Lastly, Destynn figured out how our RF modules worked and was able to transmit data between a receiver and transmitter. He also helped design the target and the motor stand. Also, he helped build the resistive divider schematic.

![image](RFModule.png)

### Current state of project

The current state of our project is looking good. We have all of our parts printed out and everything we need. The only thing we should order more of is springs. We will send out a form today for that.

We have the stand, as well as the target backing and front, and the resistive divider needs to be connected to all the buttons, but we will do that once we have decided how to ensure the bullets can press them down inside the target backing.

Once we get the motors working, the stand should be able to move the Nerf toy how we want it to, and once the buttons are placed into the target and the front pieces are glued on, it should be able to communicate with the resistive divider.

Then, once that signaling is all done, the RF modules will be able to communicate between the gun and the target and move it accordingly.

### Next week's plan

Next week's plan consists of fleshing out what we currently have.

Ayan will set up the motor with the motor drivers and the ATmega, ensuring we have proper control of movement. If the base needs to be reprinted after testing, he will either reprint it to be wider or glue it to a large flat bottom piece or stool. He will also print out the small pieces that may be attached to each button to ensure they can touch the front of the target. He will make sure the base is not tilting in one direction due to weight distribution. The definition of done here is being able to move the gun to point in different directions simply by changing code on his computer. If the motors cooperate, this shouldn't take too long, and he will also help with the target. Reprinting the base may take a bit more time.

Matilda will figure out how to properly wire all 17 buttons to the resistive divider and integrate them with the ATmega. She will solder them and attach them to the target. Once the button issue is resolved, she will glue each target front piece to the backing with the springs. The definition of done here is a full target module with all buttons placed (and able to be pressed by the bullet), soldered onto the perfboard, and successfully working with the ATmega, sending specific signals for each button. This will likely take a few work sessions, as soldering all buttons and integrating them into the perfboard is a time-consuming process.

Destynn will continue working on the RF module to ensure we can send signals between our two ATMegas. He will also help with solving the button issue and gluing the target properly. The definition of done here is having the two ATMegas communicating successfully and establishing a method to send ADC signals between them. As he is close to finishing this, he will also assist Matilda with soldering and the target setup.

For the MVP, we aim to have the motors functioning properly and the full target module operational. We hope to send signals from the target to the gun to control its movement. By final demo day, we plan to fine-tune the aiming system and ensure that even if the bullet hits between slabs, it can still register the impact and adjust accordingly for improved accuracy.

<div style="page-break-after: always;"></div>

## MVP Demo

1. Show a system block diagram & explain the hardware implementation.

![image](MvpBlock.png)
The blue represents unfinished work

2. Explain your firmware implementation, including application logic and critical drivers you've written.

3. Demo your device.

4. Have you achieved some or all of your Software Requirements Specification (SRS)?

We have achieved nearly all of our SRS. We have achieved impact on the button sends a specific signal to the ATMega. The RF module sends the signal from one ATMega to the other that controls the motor. The Principle ATMega sucessfully moves our motor stand based on signals we send it via PWM. The two SRS requirements we haven't completed are the LCD screen and the trigger to automatically fire the nerf gun without us having to shoot it. These will be done by the final project. We thought they weren't necessary for the MVP to show our product worked as we intended.

1.  Show how you collected data and the outcomes.

Each target piece has a button behind it, that is triggered when the bullet hits it. Each color is associated with a different ADC pin, where different pins are part of a different resisitve divider.

5. Have you achieved some or all of your Hardware Requirements Specification (HRS)?

   1. Show how you collected data and the outcomes.

6. Show off the remaining elements that will make your project whole: mechanical casework, supporting graphical user interface (GUI), web portal, etc.

The target is practically complete, and all of the pieces are printed. The remaining elements are the LCD screen and making the trigger motor to ensure we do not have to touch it as it runs. Then we will also add functionality to ensure that if a bullet triggers two buttons it can understand where it hit more clearly. Lastly, we will make our motor stand more powerful by attaching new motor drivers.

7. What is the riskiest part remaining of your project?

The trigger for the gun being automatic. It will be difficult to do this because we need to find a way to both activate the power and press the mechanical switch without touching the gun or misaligning it.

1. How do you plan to de-risk this?
   One option is hardwiring, and our backup option is just phsically actuating it. We will use servos to pull both buttons that need to be pulled to ensure it works.

2. What questions or help do you need from the teaching team?

## Final Project Report

Don't forget to make the GitHub pages public website!
If you’ve never made a GitHub pages website before, you can follow this webpage (though, substitute your final project repository for the GitHub username one in the quickstart guide): [https://docs.github.com/en/pages/quickstart](https://docs.github.com/en/pages/quickstart)

### 1. Video

[Insert final project video here]

- The video must demonstrate your key functionality.
- The video must be 5 minutes or less.
- Ensure your video link is accessible to the teaching team. Unlisted YouTube videos or Google Drive uploads with SEAS account access work well.
- Points will be removed if the audio quality is poor - say, if you filmed your video in a noisy electrical engineering lab.

### 2. Images

[Insert final project images here]

_Include photos of your device from a few angles. If you have a casework, show both the exterior and interior (where the good EE bits are!)._

### 3. Results

_What were your results? Namely, what was the final solution/design to your problem?_

#### 3.1 Software Requirements Specification (SRS) Results

_Based on your quantified system performance, comment on how you achieved or fell short of your expected requirements._

_Did your requirements change? If so, why? Failing to meet a requirement is acceptable; understanding the reason why is critical!_

_Validate at least two requirements, showing how you tested and your proof of work (videos, images, logic analyzer/oscilloscope captures, etc.)._

| ID     | Description                                                                                               | Validation Outcome                                                                          |
| ------ | --------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| SRS-01 | The IMU 3-axis acceleration will be measured with 16-bit depth every 100 milliseconds +/-10 milliseconds. | Confirmed, logged output from the MCU is saved to "validation" folder in GitHub repository. |

#### 3.2 Hardware Requirements Specification (HRS) Results

_Based on your quantified system performance, comment on how you achieved or fell short of your expected requirements._

_Did your requirements change? If so, why? Failing to meet a requirement is acceptable; understanding the reason why is critical!_

_Validate at least two requirements, showing how you tested and your proof of work (videos, images, logic analyzer/oscilloscope captures, etc.)._

| ID     | Description                                                                                                                        | Validation Outcome                                                                                                      |
| ------ | ---------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------- |
| HRS-01 | A distance sensor shall be used for obstacle detection. The sensor shall detect obstacles at a maximum distance of at least 10 cm. | Confirmed, sensed obstacles up to 15cm. Video in "validation" folder, shows tape measure and logged output to terminal. |
|        |                                                                                                                                    |                                                                                                                         |

### 4. Conclusion

Reflect on your project. Some questions to address:

- What did you learn from it?
- What went well?
- What accomplishments are you proud of?
- What did you learn/gain from this experience?
- Did you have to change your approach?
- What could have been done differently?
- Did you encounter obstacles that you didn’t anticipate?
- What could be a next step for this project?

## References

Fill in your references here as you work on your final project. Describe any libraries used here.
