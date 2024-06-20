# UDP-Protocol-using-Femto-Container
Use Femto Containers virtualization technique in IoT devices to implement UDP Protocol

## Introduction
This repository contains the sources codes and results of the paper entitled "*Programmable Solutions for Low-power Lossy Wireless Networks: A Study of SDN and Femto Containers*" that is going to be published soon (a link will be provided when available).

## Directories
**RIOT**: contains the fork of the RIOT source tree that was used for the measurements.

**femto-containers**: contains the fork of the Femto COntainers source tree that was used for the measurements.


**UDP_GNRC**: contains the source code to run the native UDP protocol of GNRC Stack.


**UDP_hook_malloc**: contains the source code to run the UDP protocol in Femto Contianers using Dynamic Memory for packet buffer.

**UDP_hook_static**: contains the source code to run the UDP protocol in Femto Contianers using Static Memory for packet buffer.


## How to run
Each use case has its readme file with the instructions to run.