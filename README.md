Step 1: Project Introduction & Budget
Build a vibcoding voice keyboard for yourself for less than $40 
 I am a person who does not type very fast, so with the advent of the Vibcoding era, natural language programming has become increasingly simple. My efficiency has not improved much, 
 So I wondered if I could make a future voice keyboard for myself that supports voice input. 
First, I looked for many online tutorials, but in fact, there wasn't a single one that provided a complete step-by-step guide on making a voice keyboard. So, in this note, I'll teach you how to make one step by step. 
You need to prepare the materials. We don't need to draw a PCB; we just need simple soldering and wiring to complete it. Let's go!
Step 2: Prepare Materials
 First, you need to prepare the materials. You will need 4 Grove-Mech keycaps, a Grove - OLED Display 0.66", a XIAO nRF52840 Sense, a small battery, and with a total cost of less than $40, you can make yourself a future  vibcoding voice keyboard
Step 3: Functional Verification & AI-Assisted Coding
Since I don't know if this hardware can implement my idea, I got another Grove Base for XIAO to conduct a functional verification. 
First, what I need to implement is that the button can map some shortcut keys, so I need to improve the initialization of a button, the connection, sending, and receiving of Bluetooth. Therefore, I sent my requirements to AI to have it improve this code. 
 Note that some detailed issues, such as the connection method, what kind of port the S1S2 of the button is connected to, and what kind of triggering method you prefer (long press to record and release to stop, or press once to record and press again to stop), all need to be confirmed during this process.
 I'm increasingly feeling that when vibcoding creates something, our role should be product interaction definition. Oh, right, mind maps are definitely a great helper in this process

<img width="1354" height="1526" alt="image" src="https://github.com/user-attachments/assets/b49c727e-91ce-42df-80d4-9e340b5e5a0a" />

This is an older version of the design. The specific content doesn’t matter much. What really counts is the way of thinking and logical reasoning behind it.
Here’s a practical Vibcoding tip: AI has limited context window capacity, so we should never assign it an enormous task all at once. Instead, humans need to break a huge project down into small subtasks.
But when you face a complex project you have no clue how to implement, splitting it up on your own is really difficult.Here’s a practical Vibcoding tip: AI has limited context window capacity, so we should never assign it an enormous task all at once. Instead, humans need to break a huge project down into small subtasks.


The solution is simple: just send your rough idea to AI, tell it you want to proceed step by step, and it will list out the whole workflow clearly from Step 0 onwards.
This part would work perfectly for a video recording — you can show the actual Vibcoding interface right here.
Step 4: Debug Bluetooth & Key Functions
Of course, the journey was far from smooth. I hit a tough roadblock during development: Bluetooth connected successfully, but data transmission failed. Sometimes I had to press the button repeatedly for a long time before any text finally popped up.
I kept troubleshooting on my own, then described all these weird abnormal behaviors to AI for analysis.
The root cause turned out to be improper trigger logic. I originally thought the issue came from unstable button level signals, but after adjusting the trigger mechanism, the keys became much more responsive and sensitive.I kept troubleshooting on my own, then described all these weird abnormal behaviors to AI for analysis.


<img width="1324" height="1636" alt="image" src="https://github.com/user-attachments/assets/a71600e8-01b5-473a-aae2-2d4e27c98b64" />

Once one key was fully debugged and functional, I followed the same logic to finish configuring all the other keys one by one. Just like this.

<img width="1356" height="1650" alt="image" src="https://github.com/user-attachments/assets/b7213b8f-1668-46dc-a73f-0298533bde6e" />


Step 5: Add Voice Input Module
After finishing the code for the Bluetooth keyboard, I started adding the voice input module.
This stage came with quite a few twists and turns. At first, I planned to use the built-in microphone on the XIAO board. I did lots of research and confirmed this solution is technically feasible.
But for longer voice-to-text conversion, hardware-only recording isn’t enough. It requires custom scripting, and I wasn’t very skilled at writing such scripts back then. So I put this native solution aside for now.
It’s absolutely achievable though, and I plan to refine and complete it later.This stage came with quite a few twists and turns. At first, I planned to use the built-in microphone on the XIAO board. I did lots of research and confirmed this solution is technically feasible.

But for longer voice-to-text conversion, hardware-only recording isn’t enough. It requires custom scripting, and I wasn’t very skilled at writing such scripts back then. So I put this native solution aside for now.


When I got stuck and didn’t know how to move forward, I studied how mainstream commercial products implement voice input.
I found many of them use a clever workaround: they activate the system’s built-in microphone, and even support connecting external mics like DJI microphones to achieve smooth voice input.When I got stuck and didn’t know how to move forward, I studied how mainstream commercial products implement voice input.


That inspired me to use the same trick. The core idea is simple: assign one dedicated key only to trigger the system’s built-in dictation and voice input shortcut.
Step 6: Add OLED Display & Screen Content Design
Later I decided to add a display screen. Then I thought about what to show on it. I wanted a lovely desktop pet style display. I even prepared the pixel art image in advance. But after converting it to pixel display format… the result was totally messed up. Who can explain this to me?????
<img width="1362" height="1672" alt="image" src="https://github.com/user-attachments/assets/9858fc05-3387-44e5-b183-ea3baf6ec268" />


Step 7: Wiring & Soldering
After all these bumpy trials and successful function verification, I moved into the productization phase.
Since the wiring quantity was small and not complicated, I skipped PCB layout design.
Instead, I went with the classic maker way: flying wires.Since the wiring quantity was small and not complicated, I skipped PCB layout design.


Next came soldering work. I needed to get all supplies ready: plenty of jumper wires, a small rechargeable battery (highly recommend adding an extra button for power control), a soldering iron, solder wire, and a pair of heat-resistant hands.
<img width="1356" height="948" alt="image" src="https://github.com/user-attachments/assets/8747cf40-7759-43a3-a086-4b06fea7a11c" />


The soldering work was actually way more complicated than it looks. Let me break it down for you step by step.
Before starting soldering, I drew a rough wiring sketch based on my predefined macro definitions, like this.
<img width="1334" height="1756" alt="image" src="https://github.com/user-attachments/assets/258e78f3-b033-4791-ab83-c351595016cd" />

Alright, this draft is way too sketchy. Let me refine it in detail.
<img width="1230" height="1002" alt="image" src="https://github.com/user-attachments/assets/90e061b3-994f-49f0-9cc5-eb25e014386f" />


The final schematic is packed with dense lines, which looks overwhelming at first glance—and even more intimidating when actually wiring it up.
I summed up a few tricks to make complex soldering easier.
The biggest difficulty lies in common ground and power lines. Six wires needed to connect to the tiny pins on the XIAO board.
The board is so small, the pins are even tinier. It seemed almost impossible to wire them neatly.I summed up a few tricks to make complex soldering easier.

The biggest difficulty lies in common ground and power lines. Six wires needed to connect to the tiny pins on the XIAO board.


But there’s a smart way to do it: solder one main wire to the XIAO pin first. Strip extra long wire ends on the other six cables, and also leave extra length on the main XIAO wire.
Wrap all six stripped wire ends tightly around the main wire like wrapping a dumpling. Fill the joint with enough solder to fix it firmly, then wrap the whole joint with electrical insulation tape. Done.But there’s a smart way to do it: solder one main wire to the XIAO pin first. Strip extra long wire ends on the other six cables, and also leave extra length on the main XIAO wire.






It sounds easy, but I still made many mistakes while soldering. I summarized the two most common errors beginners make:
Insufficient solder on joints. Always apply enough solder on both the pad and the stripped wire end itself. I mentioned this tip in my previous post too.
A new trick I learned this time: lay wires flat against the pad to maximize contact area. Even if you need to bend the wire into another direction later, a large flat contact joint is far more stable than a tiny single-point connection.
Step 8: 3D Enclosure Design & Modeling
The flying wires look messy and exposed. Next, I decided to design a 3D enclosure to hide all the wiring neatly inside.
Designing the 3D shell still requires drawing structural diagrams for component docking and assembly.
I roughly measured the size of each part and started 3D modeling. This is my first version — honestly, it was pretty unsuccessful.
<img width="1356" height="1580" alt="image" src="https://github.com/user-attachments/assets/5f67cdeb-b7b5-47dd-b307-e7c313901178" />

After that, I used a vernier caliper for precise measurement, calibrated the position of screw holes, and drew detailed hand-drawn sketches.
<img width="1336" height="1622" alt="image" src="https://github.com/user-attachments/assets/05484b97-d0f0-4b19-9637-804914dcf6e3" />


<img width="1374" height="928" alt="image" src="https://github.com/user-attachments/assets/b71573c1-57fa-4212-855f-df003fdd54bb" />


<img width="1354" height="1792" alt="image" src="https://github.com/user-attachments/assets/b69dcd69-7b7f-457f-b878-0cf0515ce862" />

Then I sent my hand-drawn sketches to AI, hoping it could generate a standard structural drawing for reference.
But AI only works with plain text, and its understanding of 3D spatial logic is really confusing.
<img width="1414" height="884" alt="image" src="https://github.com/user-attachments/assets/df354169-6cda-4186-947b-4c02d7579b91" />

I kept telling it to arrange the keys vertically instead of horizontally, yet it kept outputting horizontal layouts all the time. Eventually, I ran out of all my AI usage quota. I was almost in tears.
What I really needed was a standard reference blueprint to guide my manual 3D modeling. I originally wanted AI to do the modeling directly, but obviously, it just wasn’t feasible.
Industrial modeling demands high precision. I needed accurate positions for buckles, screw holes and physical keys. These sizes and layouts are extremely hard for AI to control, and also hard for me to describe clearly in plain text. In the end, I modeled the entire enclosure by myself.
<img width="1356" height="1220" alt="image" src="https://github.com/user-attachments/assets/528acc20-4315-450d-8779-6da0ea04f529" />

After the 3D printing was finished, I started assembly. I made another silly mistake here: I should have threaded all the wires through the middle shell layer before soldering, but I forgot to do that in advance.
No big deal though. Since it’s a fixed assembly structure, I just used a hot melt glue gun to fix everything in place.
Finally, here are the finished product photos.
Step 9: Assembly & Finished Product
<img width="1368" height="984" alt="image" src="https://github.com/user-attachments/assets/ca9b4697-1ef7-4f08-be65-86c170835dee" />


<img width="1324" height="1786" alt="image" src="https://github.com/user-attachments/assets/db435bbc-4638-44f1-b9ba-56333885c754" />



