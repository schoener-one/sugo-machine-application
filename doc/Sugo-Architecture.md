# Sugo Machine Firmware Architecture

## Introduction

This document describes the architecture of the **Sugo** machine framework. The Sugo machine software framework is intended to provide a very simple configurable and scalable software specially for driving complex machines with a lot of components in it.

The machine framework and machine design was original intended to build and run the Sugo machine, which is a kind of filament merger machine. Because that concept worked very well, I decided to provide it as an open source code project to learn from and to drive the software development further by the community.

### Scope

This document covers the Sugo machine architecture in generally in means of a framework, as well as the specialized architecture of the example Sugo machine as it is implemented here. It should make the basic concepts, components and the communication model understandable.

### Glossary

| Definitions, Acronyms, Abbreviations     | Description                                                     |
|------------------------------------------|-----------------------------------------------------------------|
|_|_|
|||

### Refernces

### Overview

This architecture document is based on the '4+1 architecture view model'.

All chapters have a general framework description part and a machine specialized part, which describes the machine components in detail. This document is not meant to be ready or complete, but should help the developer to understand the basic concepts.

---
## General architectural goals and constrains

The architecture was introduced to provide a framework which makes it possible to construct complex machine systems quickly, easily and robust by design as well as easy scalable with a minimum of development time overhead.

The overall general principles which should be applied for the architecture and the software design are **KISS** (_Keep It Simple Stupid_), **DRY** (_Don't Repeat Yourself_) and **YAGNI** (_You Aren't Gonna Need It_).

The major goals of this architecture are:

* **Simplicity**: Reduce system complexity and the need to adapt or program parts, with only focus on the ROI (region of interest) in order to add or adapt functionality. Makes the system easy understandable to spot potential problems early in the design as well as prohibit errors in advance.
* **Maintainability**: The system model design has to be quickly maintainable and adaptable, which reduces the effort of refactoring.
* **Scalability**: Additional components, functionality or new hardware parts must be addable easily without the need of long development processes.
* **Robustness**: The code generation from machine model must provide a stable functioning system and reduce potential errors. That property should be provided by design.

> **_NOTE:_**  It is important to clearly name all service components and not introduce synonyms to ensure an easy-to-understand software system and traceability from documents to code!

---
## Logical view

### Remote control module

### Gateway

### Service components

#### MachineControl

The _MachineControl_ is the central machine controller component. It coordinates all subcomponents of the machine, so they work in a sufficient way together. The _MachineControl_ component provides an interface, which can be controlled from a user input or from a gateway routed communication. For example, it is responsible to start and stop the machine and to handle error states.

##### FilamentCoilControl

##### FilamentCoilMotor

##### FilamentMergerControl

##### FilamentMergerHeater

##### FilamentFeederControl

##### FilamentFeederMotor

##### FilamentPreHeater

##### FilamentTensionSensor

##### UserInterfaceControl


## Process view

### System component state machine

#### Error handling

## Development view

### Memory usage and allocation

### Package structure

### Deployment


## Physical view

## Scenarios (Use Cases)

