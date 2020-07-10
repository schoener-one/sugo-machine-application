# CoffeeMachine Software Architecture

This document describes the architectural view of the CoffeeAutomat software package.

# Overview

![architecture overview](CoffeeAutomatArchitecture.svg)

# Components

## MessageQueue

The MessageBroker component is the base communication interface between all ServiceComponents. It is directly delegated within the class ServiceComponent, which also translates all command messages to the according state machine events.

![MessageQueue](MessageBroker.svg)
