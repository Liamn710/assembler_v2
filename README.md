**Assembler for Simulated PC**
tldr - to run the assembler run the commends:
make
./assembler <input_file>
and the output will be in input.ob and optionally input.ext and input.ent
Overview:
This project is an assembler designed to generate machine code for a simulated PC with the following hardware specifications:

Registers: 8 general-purpose registers (r0 to r7).
RAM: 4096 cells, each with a size of 15 bits.
Data Representation: The PC works exclusively with natural numbers, using 2's complement for arithmetic operations.
Character Support: ASCII characters are supported, allowing for character manipulation and storage within the system.
Features
Assembly Language Support: The assembler translates assembly instructions into machine code that can be executed on the simulated PC.
Label Handling: Supports labels for control flow, enabling jumps and branches in the code.
Variable Management: Allocates memory for variables while ensuring they don't conflict with registers or instructions.
Macro Processing: Allows for macro definitions and expansions, reducing redundancy in the assembly code.
Output Formats: Generates output files in specific formats, including .ob for object code.
System Specifications
Registers:

8 general-purpose registers (R0 to R7).
Each register can hold a 15-bit value.
RAM:

4096 cells, each with a capacity of 15 bits.
The memory is addressed using natural numbers.
Arithmetic:

Arithmetic operations are performed using 2's complement.
The system supports basic arithmetic operations like addition, subtraction, and bitwise operations.
Character Encoding:

Supports ASCII character encoding, allowing storage and manipulation of characters within memory cells.
Assembler Components
Lexical Analysis: Breaks down the assembly code into tokens, identifying labels, instructions, operands, and directives.
Syntax Analysis: Ensures the correctness of the assembly code structure, verifying the proper use of instructions, labels, and operands.
Macro Processor: Handles macro definitions, allowing for code reuse and simplification of complex instructions.
Code Generation: Converts assembly instructions into machine code, producing a binary output that the simulated PC can execute.
Usage:
Input Assembly Code: Write your assembly code in the supported syntax.
Run the Assembler: The assembler processes the code, converting it into machine language.
Output Files: The assembler generates the necessary output files, including the .ob file containing the machine code.