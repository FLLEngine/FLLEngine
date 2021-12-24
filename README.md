# FLLEngine

A library for controlling a lego ev3 robot with every tool you would ever need to compete in FIRST Lego League. It was created by a member of the Kalles Junior High robotics team for the Kalles Mindbenders' 2021-2022 season.

## Todo
- [x] create a working micropython library with easy cross-compilation
- [x] add hidden functions for using sensors/motors with sysfs
- [x] add custom gyro function
- [x] create basic driving functions
- [x] implement ToLoc function
- [ ] fix mispelling of docker package :D
- [ ] include basic functions for attachment motors
- [ ] include checks for gyro accuracy
- [ ] add support for ev3 buttons
- [ ] incorporate wind-down on motors when stopping
- [ ] implement PID for heading control
- [ ] eliminate unnecessarily hardcoded values
- [ ] incorporate basic color sensor usage
- [ ] add ui on the robot
- [ ] incorportate advanced color sensor usage
- [ ] fix broken native python libraries (I'm not smart enough yet)

## Usage

**Only Available for Linux**

First, clone the repository somewhere on your computer using:
`git clone github.com/FLLEngine/FLLEngine`
or:
`git clone git@github.com:FLLEngine/FLLEngine.git`

Then use this command to make sure all the necessary submodules are up-to-date:
`git submodule update --init --recursive`

After that, create the Docker package which is required for cross-compilation (all Docker stuff was borrowed from the [pybricks team](https://github.com/pybricks/pybricks-micropython)):
`compile/setup.sh armel rebuild`

Finally, compile the library
`docker exec --tty fllngine_armel make`

The result, a new micropython executable, can be found at micropython/ports/unix/micropython. This can then be copied over to the robot with scp and used to execute python files.


## Acknowledgements

A LOT of the hard stuff of getting this library to compile correctly was figured out by analyzing the pybricks source code.
