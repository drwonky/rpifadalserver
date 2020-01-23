# Raspberry Pi Fadal File Server

Written by Perry Harrington and Copyright 2020.  For now the License is GPLv3.

The title pretty much says it all! ;)

I have a Fadal CNC machine, these old beasties have an interesting way of doing file upload/download with a computer.  Fadal made it possible to remotely control the machine via the serial port, since the control treats the serial port like a terminal server.

If you issue CD,*num* at the control it will begin echoing the console output to the TTY.

If the TTY sends the DC2 control character then it can upload a file to the control.

Some clever folks figured out that you can carry on a 2 way conversation without actually executing commands at the control, they substituted the "+" character for the return key.

By this way you can have a device listen on the serial port for a set of commands, issue commands to said device, then have said device send or receive data from the machine.

The typical embodiment of these gizmos has a serial port, microcontroller, and USB host port for talking to a USB key fob.

Does this begin to sound like a familiar $35 gizmo we may all have sitting around?

Some people might have a similar $5 gizmo sitting around, but sadly it needs another $20 in accessories to do anything.

I had an idea for an elaborate touch screen rPi powered device to do this remote control, and I wrote half the UI to make it work, but the text widget performed terribly with just 1MB of NC file loaded.

If you don't deal with CNC machines often, you wouldn't know that a lot of programs today are multi-MB text files, but if you're clever, you can cram a lot into a hundred kilobytes.

## What You See Is What You Get

This repo contains a file server written in C++ to interface with the hardware serial port of a Raspberry Pi.  You connect the serial port to the Fadal and it offers up filesystm access to a USB key fob plugged into the Raspberry Pi.

Rather than blatantly ripping off the guy who came up with the original embodiment of this concept, my command list is based on Unix commands like ls, rm, and the like.

I tried to pick short and useful mnemonics like how the "sz" command is used for sending a file with zmodem over a terminal from a Unix machine, the "sf" command sends a text file.

If I implement XMODEM, it'll be called "sx" after the Unix command, though I don't think my -1 control supports XMODEM.

My intent is for this to be an educational piece of software, since writing working streambufs and making a proper callback based event handler took some research to get right (there are no single good examples I could find).  It's not my intent to put out a product that someone can simply rip and turn into a commercial product to undercut the guys at Calmotion.  If you can setup a Raspberry Pi and connect it to your CNC, and care more about DIY than saving $350, then have fun with my code.
