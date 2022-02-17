# SugoMachine Firmware Architecture

## Introduction

This document describes the architecture of the SugoMachine software package.

### Purpose

### Scope

### Glosar

... Definitions, Acronyms, and Abbreviations

## General architectural goals and constrains

The SugoMachine firmware is mainly responsible to control the hardware behaviour, like the motors, pumps and valves. The controlling of the hardware is subject of the real-time requirements. The goals testability and scalability are very important.

## Logical view

### Components

A component is the smallest logical unit of the architecture. All components are independently from others. The only way a component can use the service of one other is to use their interface directly or over the command interface (e.g. ServiceComponents).

### Layer model

For a better testability and scalability, the software is devided into layers. Every component is assigned only to one layer.

1. **ServiceComponents**: Contains all components which are responsible for the different internal and external services of the sugo-machine. A service component in that layer is only callable over the internal or external command interface.

2. **HardwareComponents**: Contains the logical counterpart of a physical hardware part of the sugo-machine. The components within this layer must not have any dependency to the physical hardware. The only way they can control the hardware is over the hardware abstraction layer.

3. **HardwareAbstractionLayer**: Exposes an appropriate interface to the underlying hardware of the sugo-machine. The functionality scope of that layer should be kept lean in order to replace it quickly by new hardware or a test stub. 

![architecture overview](SugoMachineArchitecture.svg)

### Components

#### MessageBroker

The MessageBroker component is the base communication interface between all ServiceComponents. It is directly delegated within the class ServiceComponent, which also translates all command messages to the according state machine events.

![MessageQueue](MessageBroker.svg)

## Process view

## Development view

## Physical view

## Scenarios (Use Cases)

