
# Gadget
Gadget source code contains three main folders explained in detail:
-The core folder contains the Gadget's main, compiler, and latency measurement functions.
-The include folder that contains the distributions, event generators, key-value store wrappers, operators headers, and state machines
-The operators folder that contains the logic of all operators (e.g., tumbling and sliding windows)


## Table of Contents

- [Project Structure](#project-structure)
  - [Gadget core directory](#core)
  - [Gadget Include directory](#include)
    - [distributions](#distributions)
    - [event](event)
    - [kvwrappers](kvwrappers)
    - [operator](#operator)
    - [statemachine](#statemachine)
  - [operators](#operators)

## Gadget Structure
```bash
> tree .
├── core
├── include
│   └── gadget
│       ├── distributions
│       ├── event
│       ├── kvwrappers
│       ├── operator
│       │   └── operators
│       └── statemachine
└── operators
```
## The core folder
In the core folder, `main.cpp`  coordinates all elements of Gadget
`config.h`  is a compiler for Gadget config files. Its main goal is to parse the Gadget config file and guides users if something is missing from the config file by outputting appropriate error messages.
`PerformanceMetrics.cpp` conducts the latency measurements. For example, it calculates the average or median response times.

## The include folder
This folder contains five main folders: distributions, event, kvwrappers,operators, and state machines.

### distributions
Distributions folder provides all implementation for all distribution needed by Gadget, for example, key and value length, key popularity, and event timed distributions. It offers a wide variety of continuous and discrete distributions.
### event
event folders contains two Gadget main event Generator: `gadgetEventGenerator.h` and `tarceEvents.h`

`tarceEvents.h` reads event files . Gadget gets events from event traces and generates the corresponding state access workloads. Users need to configure gadgets to read events from event traces. This config file configures Gadget to read traces located here.

`gadgetEventGenerator.h` genrates users desisred events. It lets users configure the Gadget to generate events with the specific key and timestamp distribution. It also allows users to specify the watermark frequency and out-of-order events. See this config file that configures Gadget to generate user-specified events.

### kvwrappers
Gadget wrappers include wrappers for `RocksDB` , `Faster`, and `BerkeleyDB`. A wrappers' main goal is to translate Gadget generated operation (get-put-delete- merge) to the corresponding operations in a KV store. Gadget users can easily add their wrappers to the new KV store. They need to write a wrapper that translates gadget operations to the new KV store operations. Finally, they need to register their wrapper in `wrapperBuilder.h`

### operator
The operator folder contains four main drivers that perform Gadget vertical and horizontal partitioning. 
-`oneStreamContinuousOperator.h` is a driver for continuous operators (e.g., continuous aggregators) that horizontally partitions the input stream.
-`oneStreamWindowOperator.h` horizontally and vertically partition the input stream for window operators (e.g., tumbling and sliding windows).
-`twoStreamContinuousOperator.h`   horizontally partitions two-input streams.  
-`twoStreamWindowOperator.h` horizontally and vertically partition two-input streams (e.g., join operators).
### statemachine
This folder contains current state machines in Gadget. A state machine's goal is to generate a sequence of state accesses needed to process each event. For example, in an incremental tumbling window operator, two operations are needed to process each event: `get` and `put`.   

To add a new state machine to Gadget, users must implement three functions:  `reset`, `closeStateMachine`, and `HasNext`. 
- `Reset` resets the state machine for each new event. 
- `closeStateMachine` sets the state when the state machine (i.e, window) is evaluated
-  `HasNext` is the primary function that operates the state machine from one state to another. Finally, 
Gadget users can register their new state machine in `stateMachineBuilder.h`

## The operator folder
This folder contains the implementation of all operators that Gadget currently supports. Each operator determines the state machine the operator is using and the window assignment strategy for windowed operators.



