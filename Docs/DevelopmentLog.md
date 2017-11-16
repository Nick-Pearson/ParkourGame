# Development Log

Kept up to date with notes from latest meetings

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


