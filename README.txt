# KBOS Ground

## Synchronization and Concurrency

##### Yihong Xu and Weizhe Wang

`2023-2-6`

### Project

`Taking_Flight.c` spawns aircraft to participate in a distributed simulation of a busy airport. The aircraft are spawned in a separate thread, and the main thread is not responsible for the airport. `Utils.c` and `Utils.h` provides header imports, constants and data structure definitions, and utility functions for simulation and logging.

### Constants

- `RWY_REGIONS_COUNT` = 6
- `SHORT_RWY_COUNT` = 6
- `LONG_RWY_COUNT` = 2
- `SMALL_PLANE_COUNT` = 30
- `LARGE_PLANE_COUNT` = 15
- `LOG_ROUNDS_COUNT` = 2

### Logging and Visualization

One additional feature is the ability of visualizing the program's output/logs. This program produces a special `[ID].csv` output containg the following information: `State`, `Location`, `Duration`, and `isReversed` for takeoff and landing

An accompanying Unreal Engine program then reads the csv files and visualizes every aircraft's movements. The program is written by Yihong Xu and is available at the following GitHub Repository: [KBOS Ground Concurrency Visualizer](https://github.com/GP2P/KBOS-Ground-Concurrency-Visualizer)

A video of the visualization and a remastered airport map is available at the following link: [Concurrency Simulation Visualization with Unreal Engine 5](https://xu.yih0.com/Dev/Unreal_Airport_Visualization)

### Other Features

#### Colorful Terminal Output
To distinguish each state in the output. They are assigned with different colors.
Seven ANSI Colors were defined as constants.

They are:
- `YEL  "\x1B[33m"` -- The declaration of Idle state
- `MAG  "\x1B[35m"` -- The declaration of Await Takeoff/Await Landing state
- `CYN  "\x1B[36m"` -- The declaration of Flying
- `GRN  "\x1B[32m"` -- The declaration of Takeoff
- `BLU  "\x1B[34m"` -- The declaration of Landing
- `WHT  "\x1B[37m"` -- Not used
- `RESET "\x1B[0m"` -- Reset color setting after printing state declaration

### Defects

None to our knowledge.
Prematurely terminated program would not save to the csv logs, by design.
