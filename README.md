# Bad News

Unfortunately as made apparent by recent progress and recent testing it has been made clear to me that this library is going to have to be written in a lower-level language like C. Python is easy to program in but it does not offer the same speed as something like C. We can still write some things in python (which I will finish in case C takes too long) but some things require more optimization than I think we can get out of Python.

# Things we can probably do with Python

- (maybe) higher acuracy gyro reads
- a ToLoc function like last year
- something that detects position based off of color sensor reads

# Things that we likely will have to do in C

- easily-integrated failsafes
- positional tracking based on motor and gyro reads
- direction of movement dinamically changing based on deviation from given path
- an easy-to-use graphical interface
- multiple pathfinding algorithms
- reverse kinemetics for attachments


