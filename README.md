# iduino - interactive Arduino


`iduino` is an experimental mashup of the [firmata](http://firmata.org/wiki/Main_Page) serial programmable Arduino and the [Bitscope Virtual Machine](http://bitscope.com/design/manual/?p=2) instruction set.

Using a simple interpreted command set the IO register set of an Arduino can be programmed interactively.

# Usage

Load the iduino.ino sketch onto your Arduino (tested with Arduino UNO, but other Atmel models should work similarly) and connect to the serial console at 115,200 baud. Each command is a single ASCII character (see below). iduino will echo each character as it is entered if it is valid. If the command is not valid, `?` will be echoed. Certain characters such as '\n' have no effect, but echoed for convenience.

# Command set

Drawing inspiration from the Bitscope VM API, iduino is programmed via two virtual registers, R0 and R1. R0 is known as the scratch register, R1 is the address register.

`[` - clears the contents of R0.

`0..9` `a..f` - left shifts the binary value of the character entered into R0.

`]` - ignored, for compatability with the Bitscope VM API.

`@` - copies the value in R0 into R1.

`s` - stores the value in R0 into the _address_ of R1, that is (R1).

`z` - stores the value in R0 into the _address_ of R1 and increments R1.

`p` - prints the contents of the address stored in R1.

`|` - ORs the value in (R1) with R0, this sets the bits of R0 in (R1).

`~` - ANDs the value in (R1) with the compliment of R0, this clears the bits of R0 in (R1).

# Memory map

The Arduino has a unified memory map. Each architectural, IO, and extended IO register has a predetermined location in memory (although those registers may not be physically present in SRAM). Each Arduino register may be referenced by R1 by loading its address via R0, however iduino R0 and R1 are not accessible via this method, addresses 0x00 and 0x11 refer to the Ardunio architectural registers r0 and r1. This limitation may be lifted in the future.

# Examples

This is a very incomplete getting started guide.

## Loading data

As a simple example, the number 0x42 can be loaded into R0 as follows

     [42]

The `[` clears R0, `4` shifts 0x4 into R0 from the right hand side, leaving 0x04 in R0, then '2' shifts 0x2 into R0, leaving 0x42 in R0. The `[` and `]` are optional, but we retain them in these examples for clarity.

## Setting a register

To do anything useful with iduino we need to set the configuration of various Arduino registers. To do this you will need to read the documentation for your particular Atmel chip closely (for example the [ATmega328p](http://www.atmel.com/Images/Atmel-8271-8-bit-AVR-Microcontroller-ATmega48A-48PA-88A-88PA-168A-168PA-328-328P_datasheet.pdf) found on the Arduino UNO). The Atmel chip stores the control registers for the various chip functions just beyond the architectural registers starting at address 0x20.

For example, the configuration and state of the digital pins 8-13, also known as PORTB, are stored in addresses 0x24 and 0x25 respectively. So, to set pin 8 to OUTPUT and turn it on, we can issue the following commands.

	[24]@[01]s[25]@[01]s

Let's break this down a little. 

* First we store the address of the DDRB, the port B direction register, 0x24, in R1 -- `[24]@`
* Next we store the value, 0x01 for PORTB, bit 0, which is Pin 8 on the Arduino Uno, into the address pointed to by R1, this sets Pin 8 to OUTPUT  -- `[01]s`
* Now that we have set Pin 8 to OUTPUT, load the address 0x25 into R1 -- `[25]@`
* Then store 0x01 into (R1) turning on the output. -- `[01]s`

## Setting a register (continued)

Taking the example a little further we can write the above more compactly.

	[24]@[01]zs

The main difference is the use of the `z` command in place of the first `s`. `z` operates similarly to `s`, but once the value has been stored it increments the value of R1. This leads to two improvements. 

* After the first `z`, the value of R0 and R1 respectively are 0x01 and 0x25 (R1 was incremented by `z`)
* This means we can issue a `s` to store 0x01 into address 0x25 without setting up R0 and R1 again. Alternatively `z` could be entered again, which would leave R1 with the value 0x26.

## Altering a register.

Many Arduino control registers share several duties so care must always be taken when storing, or overwriting, values stored in them. Say for example we wanted to change the value of Pin 8, but leave Pin 9 untouched. To do this, we can use the `|` operator.

	[24]@[01]|[25]@[01]|

Because we're not simply overwriting values, we loose some of the brevity of the previous example. The main difference between the original example and this one is the replacement of `s` with `|`. In effect the value in R0 is OR'd with the contents of (R1), this preserves any bits we did not intend to alter. 

## Toggling bits

The `|` command as a counterpart, `~` which ANDs the bitwise compliment of R0 with R1, effectvely unsetting any bit in R0 that was previously set. This can be used to toggle a bit.

	[24]@[01]|[25]@[01]|~|~|~|~|~|~|~|~|~

The `|` command sets the bits of R0 in (R1), `~` unsets them, and so forth.

	[24]@[03]s[25]@[03]s[01]|~|~|~|~|~|~|~|~|~|~

Sets both bits 0 and 1, Pins 8 and 9 to on, then toggles bit 0, Pin 8.
