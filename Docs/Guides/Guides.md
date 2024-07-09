# Building a 6502 Computer

Embarking on the journey of building a computer from scratch is a fascinating endeavor that not only enriches your understanding of computer architecture but also connects you to the historical roots of computing. This guide is designed to walk you through the process of constructing a functional computer system based on the 6502 microprocessor, a legendary chip that powered some of the most iconic home computers of the 1980s. From assembling the hardware components to programming your first **"Hello, World"** application, we'll explore the intricacies of the 6502 architecture, learn about its input and output mechanisms, and delve into the hands-on experience of bringing a classic computing platform to life. Whether you're a hobbyist, a student of computer science, or simply curious about how computers work at a fundamental level, this project promises a rewarding and educational experience.

## Part 1: ["Hello, world" from scratch on a 6502](https://youtu.be/LnzuMJLZRdU?si=wg1GeZv9bp3qSy0M)

### Notes and steps taken:

- Review input/output pins: 16-bit addresses, 8-bit data bus

- Connect clock (phase input); recommend building system clock (Astable 555 timer) for manual stepping and debugging

- Add reset button (RESB pin) - hold low for at least two clock cycles to reset

- Use Arduino Mega to monitor address pins and data bus states

- Set Arduino pins to INPUT (pinMode), read in loop (digitalRead), print to serial monitor

- Connect clock to Arduino, use interrupt for state capture: *_attachInterrupt(digitalPinToInterrupt
  (CLOCK), onClock, RISING)_*

- onClock function reads state of address/data lines and R/W status on each clock pulse

- Leave space on breadboard for easier debugging access

- Hard-wire 0xEA (NOP instruction) on data bus using resistors

- Observe processor behavior:

     - 7-step initialization process on reset 
     - Load start address from *0xfffc* (low byte) and *0xfffd* (high byte) 
     - Start address comes out as 0xeaea due to hard-wired value 
     - Processor reads 0xEA as NOP instruction - Continues incrementing address (*_0xeaeb, 0xeaec, ..._*) every two clock cycles, reading and executing NOPs indefinitely

## Part 2: [How do CPUs read machine code?](https://youtu.be/yl8vPW5hydQ?si=0IdQQgAE9Au_ZRZi)

### Notes and steps taken:

- Install a 28C256 EEPROM, which stores 32 kilobytes (256k bits) of data.

- Connect the address and data bus pins of the 6502 to the EEPROM. The 6502 has 16 address pins (A0 to A15), while the EEPROM has 15 (A0 to A14). This means the processor can access 65,536 locations, but the EEPROM only stores 32,768 bytes.

- Problem: If the processor sets its address pins to a value greater than 0x7fff, it will start reading from the beginning of the EEPROM again.

- Solution: Connect A15 to the "Chip Enable" (CE) pin of the EEPROM. Invert the signal from A15 so that CE is low when A15 is high. This maps the lower addresses to the higher addresses, allowing the lower addresses of the microprocessor to be used for other purposes.

- Wiring: Connect A0 to A14 and the data bus pins from the 6502 to the EEPROM. Set further control pins as instructed. Connect A15 of the 6502 to an inverter, and the output of the inverter to CE.

- Create a file with 32k NOPs (*0xea*) and write it onto the EEPROM. Change the file at location *0x7ffc* and *0x7ffd* to set the start address to *0x8000*. Verify that reading the data from *0xfffc* and *0xfffd* returns *0x8000*.

- Create a program and store it at the beginning of the EEPROM: *A9 42 8D 00 60*. This loads the A register with *0x42* and stores it at address *0x6000*.

- Verify that the program executes as expected by monitoring with the Arduino. The processor eventually writes *0x42* to address *0x6000*.

- Use the W65C22 VIA (Versatile Interface Adapter) to handle external hardware. Connect the data bus, clock signal, read/write sync, and chip select pins. Wire the reset pin to the existing reset button.

- Implement address decode logic to activate the W65C22 when the 6502 has *0x6000* on its address pins. Use NAND gates for the first two bits and connect A13 directly to CS1. This enables the W65C22 from *0x6000* to *0x7fff*.

- Wire the register select pins using the lower bits of the address (A0 to A3).

- Verify the wiring with a program that outputs to port B: *A9 FF 8D 02 60 A9 55 8D 00 60 A9 AA 8D 00 60 4C 05 80*.

    - Load *0xff* into the A register, setting all bits of the data direction register B on the 65C22 to output.
    - Store the value *0x55* (*0101 0101*) to register 0 at *0x6000*, lighting up every other LED.
    - Flip the bits to *0xaa* (*1010 1010*) to make the LEDs blink.
    - Create an infinite loop by jumping back to the instruction of loading *0x55* (at location *0x8005*).
    - Keep the start address at *0x8000* as the program begins reading from the start of the EEPROM.

## Part 3: [Assembly language vs. machine code](https://youtu.be/oO8_2JJV0B4?si=gAdkF29UJactH9ie)

### Notes and steps taken:

- Write the previous program in assembly and use the vasm assembler to convert it to machine code.
  Structuring the assembly code:

     - Use the .org directive to indicate the memory location of the program. For example, .org $8000 means the following instruction is at address *0x8000* (*0x0000* in the ROM). Similarly, .org $fffc sets the start address of the program. Writing .word $8000 after this directive sets the values *00 80* to *0x7ffc* and *0x7ffd* in the ROM file.
     
     - Without .org $8000, the assembler would expect the program to start at *0x0000*, resulting in a ROM file twice the size (65,536 bytes instead of 32,768 bytes).

- Improve the program to use labels, then change it to output a different LED pattern: start with *0x50* (*0101 0000*), then use ROR (rotate right) to move the active LEDs to the right.

## Part 4: [Connecting an LCD to our computer](https://youtu.be/FY3zTUaykVo?si=TsxsGu_biWiJSF62)

### Notes and steps taken:

- Connect HD44780 LCD display to the breadboard. Add a variable resistor for brightness adjustment. Connect port B lines from W65C22 to the data lines (D0 to D7) of the display. Connect the top three bit pins from port A of W65C22 to the control pins RS (register select), RW (read/write), and E (enable) of the display.

- The display's data lines access either a data register (DR) or an instruction register (IR), determined by control signals from port A of W65C22. RS high targets the data register, RS low targets the instruction register. Actual reads or writes occur when the E (enable) pin is set high.

- Improve the assembly program to set constants for port A and B (PORTA, PORTB) and the data direction registers A and B (DDRA, DDRB).

- Set all pins of port B to output, and do the same for the top three bits of port A (*lda #%11100000, sta DDRA*).

- Send instructions to the display:

    - Set data length, number of lines, and character size with RS = 0, R/W = 0, D5 = 1, D 4 = 1, D3 = 1, D2 = 0. In assembly:
      
      - lda #%00111000, sta PORTB (load values for data lines into the A register of the 6502, store contents of A register to port B on W65C22).
      - Clear control signals on port A: lda #0, sta PORTA.
      - Set the enable bit: lda #E, sta PORTA (E = %10000000) and immediately clear control signals again.

    - Continue similarly for sending instructions for display on, entry mode (address counter increments with each character to write left to right), and writing a character.

    - To write characters, set RS high to target the data register, put the ASCII value of the character on the data lines, and write it by toggling the enable bit.

- Test the program by writing it to the ROM, powering on the circuit, and holding reset. The letters should appear on the screen.

## Part 5: [What is a stack and how does it work?](https://youtu.be/xBjQVxVxOxc?si=FVMrSRqH_UF0FBzj)

### Notes and steps taken:

- Improve **"Hello World!"** program by creating subroutines for ´lcd_instruction´ and ´print_char´. Use ´jsr´ to jump to these subroutines and ´rts´ to return.

- Write the program to the ROM and test it. It doesn't work because the subroutines require a stack to store return addresses. The microprocessor attempts to store the return address before jumping to the subroutine, but there's no RAM to store it. When rts is called, it can't retrieve the return address.

- The 6502 expects the stack to be in the memory area 0x0100 to 0x01ff and has an 8-bit stack pointer (0x00 to 0xff, 256 bytes).

- Initialize the stack pointer at the beginning of the program using txs to transfer a value from register X to the stack pointer (ldx #$ff, txs). The stack grows downwards towards the lower addresses, just like on other platforms.

## Part 6: [RAM and bus timing](https://youtu.be/i_wrxBdXTgM?si=eZx3W49TOd4SnCUI)

### Notes and steps taken:

- Install 62256 RAM chip (256k bits, 32 kilobytes).

- The RAM chip pinout matches the ROM chip, so wire the address and data lines from the ROM chip to the RAM chip. Connect the WE of the RAM to the RW pin of the 6502.

- Address layout:

  *0x0000* to *0x3fff* for RAM (*0x0100* to *0x1fff* for stack)
  *0x8000* to *0xffff* for ROM (as before)

- RAM detection: Check that A15 and A14 are low to address the RAM (0x0000 to 0x3fff).

- Potential wiring setup:

    - Connect A14 and A15 of the microprocessor to OE (output enable, active low) and CS (chip select, active low) of the RAM to activate it only in the address range 0x0000 to 0x3fff.
    - Analyze timing compatibility of the microprocessor and RAM.

- Timing analysis:

    - Running the processor at 1 MHz gives us at least 1,000 nanoseconds per clock cycle.
    - RAM takes up to 70 nanoseconds to output valid data after setting addresses and control signals.
    - Ensure the microprocessor waits this time before reading data.

- Write sequence confirmation:

   - CS (connected to A15) needs to go high before address bits change and RW before WE.
   - Delaying CS low after the address is valid and high before address/data become invalid can be achieved by tying it to the clock (phi2).

- Logic gates setup:

   - Use two NAND gates to ensure CS goes low only when the clock is high.
   - Connect A15 through a NAND gate to invert it, and use this output with the clock signal as inputs to another NAND gate.
   - This setup ensures low output only when the clock is high and A15 is low.
   - The timing of the NAND gates is short enough to not affect the circuit.

## Part 7: [Subroutine calls, now with RAM](https://youtu.be/omI0MrTWiMU?si=ZH_86FrEWJ2sToCn)

### Notes and steps taken:

- Wire up A14 to OE, and A15 through the NAND gates to CS as described above
- Tie inputs to 4th NAND gate to high; just for good measure
- Improve **"Hello World"** program to clear screen when setting up the display in the beginning _(#%00000001)_
