# Configure Gadget

A Gadget user configures Gadget by setting three main elements:
- Event source - There can be two event sources
- operators type
- KV store

The events source tells Gadget the sources of events. In Gadget, events can come from a trace (e.g., Google borg traces), or Gadget itself can generate events.

The operator types (e.g., `tumbling.keyed.incremental`) determine the operator for which Gadget generates state accesses load.

The KV store determines the KV store that Gadget evaluates using the generated state accesses load. If the user chooses a file, the generated state accesses load will be dumped on the file named `gadget.log`

Each main component has several parameters that need to be configured by the user. The Gadget compiler helps users set the required parameters by outputting error messages.


## Table of Contents
  - [Event Source](#event-source)
    - [when event.generator.type is gadget](#when-eventgeneratortype--is-gadget)
    - [when event.generator.type is tracefile](#when-eventgeneratortype--is-tracefile)
  - [Set the operator](#set-the-operator)
    - [operator type](#operator-type)
    - [key popularity](#key-popularity)
    - [key and value sizes](#key-and-value-sizes)
  - [Evaluate a KV store or dump on file](#evaluate-a-kv-store-or-dump-on-file)
  - [The distributions](#the-distributions)


## Event Source
Gadget needs one event source for single-input operators (e.g., tumbling and sliding windows) and two event sources for two-inputs operators (eg., sliding  and tumbling joins)
Gadget offers two types of event generators: 1) `gadget` 2) `tracefile`

When the event generator is `gadget`, it generates desired events. When the event generator is a  `tracefile`, the events are read from a trace file.

Lets first set the event generator type (example: please see line 4 in file `config.txt`):
```asm
first.event.generator.type = gadget
```
Next, lets set the needed parameters based on the type of the event generator:
### when event.generator.type  is gadget
If the event generator type is set to `gadget`, we must configure the  `key popularity` and `event time distribution` of events. We also need to configure the `watermark frequency`, the percentage of `out-of-order events` and the `lateness threshold`. Please see lines 6-18 of `config.txt` as an example.

The following lines  set the `key popularity` of events. The key popularity can  be set to constant, hotspot, zipf, sequential, and uniform.
```asm
#options(constant, hotspot, zipf, sequential, uniform)
first.event.generator.key.popularity.distrib.type = zipf
first.event.generator.key.popularity.distrib.params.s = 1
first.event.generator.key.popularity.distrib.params.keyspacesize= 10000000
```
The following lines set the `distribution of event times`.

```asm
#options(exponential, normal, constant, uniform)
first.event.generator.event.occurrence.gap.distrib.type = exponential
first.event.generator.event.occurrence.gap.distrib.params.lambda= 10
```

The following lines configure `watermark frequency`, the percentage of `out-of-order events`, and the `lateness threshold`

```asm
first.event.generator.watermark.frequency = 1
first.event.generator.outoforder.percentage = 2
first.event.generator.lateness.threshold = 3;
```

### when event.generator.type  is tracefile

If the event generator type is set to  `tracefile`, the user needs to set the column indexes for event key, event time, and event type (indexes start from 0). For example, config2.txt (lines 4-7) sets an event generator of type `tracefile`. It uses `task-input.txt` as the event source (please find this trace under `traces`)

```asm
#options: (tracefile, gadget)
first.event.generator.type = tracefile
first.event.generator.key.index = 0
first.event.generator.event.time.index = 2
first.event.generator.event.type.index = 4
first.event.generator.file.path = tasks-input.txt
first.event.generator.watermark.frequency = 1
```


## Set the operator
To configure the operators, users need to set the operator type, its required parameters, kay popularity, key size, and value size distribution of generated state access operations.

### operator type
Gadget offers the following operators for which it generates state accesses workload: `tumbling.all.incremental`, `tumbling.all.holistic`, `tumbling.keyed.incremental`,
`tumbling.keyed.holistic`, `sliding.all.incremental`, `sliding.all.holistic`, `sliding.keyed.incremental`,
`sliding.keyed.holistic`, `join.tumbling`,  `join.sliding`, `join.interval`, `session.holistic` ,`session.incremental` ,  `continuous.join`,  `flink.replayer`, `ycsb.replayer`. Tumbling window operators need `window.length` and sliding window operators need both  `window.length` and `sliding.length` to be set.  For example, in `config.txt` (lines 27-30), the operator type us set to   `tumbling.keyed.incremental` and `window.length` is set to 5 units of time.

```asm
operator.type= tumbling.keyed.incremental
expected.num.operations = 100000
window.length = 5
#sliding.length = 1
```
'expected.num.operations' determines the number of state access operations that will be generated.

### key popularity
Gadget users can define the key popularity of state objects (ie., window IDs). In file `config.txt`  key popularity is set to sequential, meaning that the key of  the newly created window (state) is set to LastWindowID+1. Gadget Users do not need to change this unless exploring novel window management strategies.
```asm
#options(constant, hotspot, zipf, sequential, uniform) - no need to change this
operator.key.popularity.distrib.type = sequential
operator.key.popularity.distrib.params.keyspacesize = 1000000000
```
### key and value sizes
Gadget users can set the key and value size of state accesses. For example, in file `config.txt`, the key and value size of state operations is constant and equal to 10 bytes.

```asm
#********  key size distribution (keys used to access state store)
#options(exponential, constant, uniform, normal)
operator.key.size.distrib.type = constant
operator.key.size.distrib.params.constant = 10

#******** value size distribution (values that are inserted to state store)
#options(exponential, constant, uniform, normal)
operator.value.size.distrib.type = constant
operator.value.size.distrib.params.constant = 10

```

## Evaluate a KV store or dump on file

Finally, Gadget users can choose to dump the generated state workload on a file or evaluate a KV store performance using the generated state workload.

```asm
# options(file - rocksdb- berkeley - faster)
#If file is chosen, Gadget dumps the generated stat access operations on the file (wrapper.path/gadget.log)
#if you choose to evaluate a KV store, you need to install the KV store first. 
wrapper.type = file
wrapper.path = .

```

## The distributions
Please note if you set the distribution of value or key sizes, popularity or event times, you may need to configure some extra parameters depending on the type of distribution. For example:

if you set the distribution to exponential, the parameter `lambda` needs to be defined.

If you set the distribution to  constant, the parameter `constant` needs to be defined.

If you set the distribution to  normal, the parameters `sd` and `mean` need to be defined.

If you set the distribution to normal, the parameters `a` and `b` need to be defined, where a is the lower bound, and b is the upper bound.

If you set the distribution to  hotspot, the parameters `hotspotfraction` and `hotoperationfraction` need to be defined.

If you set the distribution to  zipf, the parameter `s` needs to be defined.

