# Development Log

Kept up to date with notes from latest meetings

## 7. Meeting with Tilo and team meeting afterwards (10am MVB)

### Tilo:

Tilo gave a number of points to improve our game, we are meeting with him again next thursday to discuss our solutions. Solutions are inline with Tilo's points

* We need to demonstrate a multiplayer gameplay prototype

*We are adding a new milestone to our project planning scheduled for 2 weeks before the Easter demo to ensure we have a working multiplayer prototype*

* We need to build more interaction between players

*Jonas and David are both working on systems that will add this functionality and also allow for interesting gameplay*

*Use players momentum as a factor when determining collisions*

* We need methods to ensure players are aware of the game state and stay in roughly the same game area

*Implementing an X-Ray vision system (similar to VATS in fallout) that will highlight key other players on the screen*

* We need to think of a best case I/O system

*Going to design an I/O system that could work and discuss the idea with Richard and Beth to see if it is feasible*

*Going to add a telemetry system so we can track keypresses on the School Visit day and use that data to inform how players learn to play the game*

* Additional work to know where we can excel as a team

*Our main focus is building the best games day experience, this is going to rely on good spectator technologies for managing and tracking games*

### Team Meeting:

Discussed the points above
Agreed that we should start work on the report as it is large

__Corin:__
* Modifying the Minigame Manager system and adding the racing game
* Moving onto level design for CTF (planning on building a large symmetrical sandbox at first)

__Jonas:__
* Refactoring Grip and Push moves to be driven from the same input and be selected based on context
* Going to try and extend this system to allow a Vault move and also ability to push off other players

__Ben:__
* Creating a number of simple 3D models
* Discussed textures and agreed to leave the models textureless for now
* Will work with Corin later next week on filling a test level with these objects

__James:__
* Finishing player names, making sure server/client interaction is correct and that names orient to the camera
* Will work on the backend for the leaderboard system after this

__Nick:__
* Finished footstep audio
* Working on post process effects for the XRay vision and TV screens

__David:__
* Working on the tackle move
* Going to try and get a generic system so the player can roll out of situations where they would usually ragdoll

### Weightings

After the meeting we discussed the work levels from last term and generally agreed that the level of work put in is in roughly three levels (most to least)
* Nick + James
* Corin, Jonas + Ben
* David

Going to discuss exact numerical weightings at a later date


## 6. Meeting 15/12/17, 6pm MVB
Our industrial mentor Craig came by to check where the project was currently at.

Only Corin and David were present - end of term meant for deadlines and people travelling home.

__Suggestion of an agile 'spike' was raise by Craig, wrt our physical animations:__
A short timeboxed attempt to implement physical animations, without regard to the quality. We can then use this to assess the viablity of the technology, and if accepted use this to guide our final implementation. The work done on the spike would then be thrown away to allow for a release quality implementation.

__Assigning creative responsibility:__
Craig suggested the team should ideally have a single person in charge of the creative direction, handling art, style and other aestehtic choices.

__Art style:__
Minimal style would be preferable, due to lack of creative expertise and a lower time requirement. Maybe focus on color first. Other games with similar technology have a minimalistic style so look for inspiration there.

__Timeline:__
Aim to have a functional product at the end of feburary/march, that way the rest of the time can be used to polish and focus on making a complete product.

__Next meeting:__
Get back to Craig with a time and date for our next meeting.

## 5. Meeting 05/12/17, 4pm MVB

Meeting with Tilo at 2pm on thursday with demo build to show to him and a 4th year student

__Jonas:__
* Committed ik-test work 
* **Working on refactoring ik system - not required for the demo**

__David:__
* **Working on QTE event task - not required for demo**

__Ben:__
* Committed work preventing collision capsule from moving when in ragdoll
* **Looking at disabling ragdoll**
* **Going to look at physics constraints for the 'grab' action if there is time before the demo**

__James:__
* We all agreed most networking stuff is looking good currently for demo
* **Working on a c++ function for the lobby allowing resizing based on DNS results for servers**

__Corin:__
* Updated previous work to be compatible with 4.18
* **Working on cleaning up hand position work so we can use it in the demo**

__Nick:__
* **Going to merge in the ik-test work to main branch**
* Working on hacking in the ik test work so we can use it in the demo
* **Going to merge in Capsule work by Ben**

## 4. Meeting 27/11/17, 4pm MVB

Not present:
* David
* James

We agreed that we should set more hard goals in future to start to lock down our ideas and keep the project on track

__Jonas:__
* Researching IK and physics in the engine, found very few useful resources
* **Going to push debug IK work to github so we can see it**
* **Working on our own IK solver implementation (Nick to send link to tutorial to help on this)**

__Corin:__
* **Creating a system to apply physics impluses to the skeleton to move limbs - going to test this on a grab move**

__Ben:__
* Working on disabling ragdoll states, and preventing capsule move during ragdoll. **continuing on this**

__Nick:__
* Cleaning up C++ so we can avoid hard references to bone names
* **Networking ragdoll state of the player so clients can see ragdolling**

__Gameplay Discussion:__
* Can see a 'mini-game' style input where the user has to tap a button quickly to perform a difficult action, could be used to punish players who fail to sucessfully complete a parkour move. **Nick to find references in other games and ask David to create a demo of this feature**

* Concern that the data structure we use to represent parkour moves and hand positions may hinder us in future - not sure whether a plane or line is best. For the time being we will implement a base class with the same functions so that the calling code doesn't have to worry about whether they have a plane or a line.  **Corin to look into this, might require more thought to manage these struct pointers and avoid memory leaks**

# 4. Meeting 23/11/17, ~4pm MVB

Not present:
* Jonas

Met briefly in Back lab to set up team lab machine and ran a test with multiple players in the Windows lab

## 3. Meeting with Tilo 09/11/17, 2pm Tilo's Office

Not present:
* David

Notes by Nick

General Notes:
* IK driving physics will be difficult but is worth us persuing
* Asked for a windows machine to be made availible for development use
* Requested ports to be opened (they may be only opened on certain rows)
* Need a proper prototype test map
* Need to ensure we are integrating our work regularly
* Tilo wants us to consider the idea of a custom controller, also to think about our third key technology

__Group meeting after:__

New Tasks:
* Set up a system so we can have constraints along the arms so that the arms can support the body in ragdoll
* Research combining physics with IK - what works by default in unreal
* Writing a custom IK solver
* Using a sphere trace around the player to maintain a list of possible IK targets
* Sort function that can take a large list of input points and choose IK targets for left and right hands from the list based

Plan to meet again on Monday

## 2. Meeting 06/11/17, 1pm MVB

Not present:
* David

Notes by Nick

General Notes:
* Thursday we have to demo our current work to Tilo
* Corin to organise a meeting with the Mentor, will update when he has dates
* Need to finalise our list of key technologies

__James:__
* Requested ports (7777, 3000 - 3009) to be opened
* **Continuing on the serialisation optimisation task from last week**
* Looking at setting up a VPS for us all to connect to / use
* Other optimisations e.g. distributed servers, compression etc. as discusses

__Ben:__
* Ragdoll work pushed to github
* **Moving the ragdoll code from the Level blueprint to C++, also looking to encapsulate this functionality into a nice system for others to use**

__Corin:__
* Did some IK research, discussed this with Jonas
* **Working on subclassing our level geometry so that we can query it for hand positions**
* **Also looking at modifying the current test map to include this new geometry class**

__Jonas:__
* Set up a point and click style IK debug system with the right arm
* **Setting up the above into a system for others to be able to use**

**Corin and Jonas to combine work so we can have a very basic implementation of the hands reaching out for an object**

__Nick:__
* Worked on bug fixing the dedicated server build, now fixed
* **Setting up landing screen GUI for connections to different IP addresses**

## 1. Meeting 30/10/17, 6pm MVB

Not present:
* Corin (stuck in the north)
* Jonas

Notes by Nick

__Nick:__
* Finished simplifying the mannquin skeleton
* Added version class
* **Working on an instant replay system for spectator class (#12)**
* **Updating to Unreal 4.18**

__James:__
* Finished setting up the dedicated server build on Mac
* **Working on splash screen**
* **Will be testing the server on the cloud**
* **Researching low level unreal networking to see if we can optimise it (#13)**

__Ben:__
* Implemented keyboard controls to activate ragdoll on parts of the body (#11)
* **Updating the ragdoll controls to work with the updated skeleton**
* **Looking at setting up animation slots for various parts of the body (#8)**

__David:__
* **Researching shape trace collision for hands and feet, possibly will need a custom trace channel**
* **Setting up basic state machine for tracking when the characters hands/feet are attached to something (#14)**
