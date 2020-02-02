/**
 *
 * Author: Daniel cender
 * Date: 02/02/2020
 * Description: This program takes a decimal integer from the user,
 *  then performs multiple bitwise operations to transform the int,
 *  before displaying the result back to the user.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Function utility inspired by people much more experienced than myself:
 * https://www.programmingsimplified.com/c/source-code/c-program-convert-decimal-to-binary
 * Description: This utility takes an integer as input.
 *  It then creates iterates through the 31 usable bits of the integer,
 *  shifting to that bit on the input variable, checking if it's 0 or 1,
 *  then adding it to the nth place in the string binary representation.
 */
char* decimal_to_binary(int n)
{
    char* returning = malloc(32 + 1);
    int count = 0;
    if (returning == NULL)
        exit(EXIT_FAILURE);
    for (int idx = 32; idx >= 0; idx--) {
        int current = (n >> idx) & 1; // looks at nth bit to check it
        if (current == 1) {
            *(returning + count) = '1';
        } else {
            *(returning + count) = '0';
        }
        count++;
    }
    return returning;
}

int main()
{
    int input;
    printf("Hello, please input a base-10 number between 0 and 4,095, inclusive:\n");
    scanf("%i", &input);
    printf("\nYou entered: %i\n", input);

    if (input < 0 || input > 4096) {
        printf("The number you entered is out of the allowed range! Exiting now!\n");
        return 1;
    }

    printf("Binary version: %s\n", decimal_to_binary(input));

    // TODO: Display the number as a 32-bit hexadecimal number on the console
    printf("Hexadecimal input format: 0x%X\n", input);

    // TODO: shifting the number 16 bits to the left
    input = input << 16;
    printf("Input, shifted 16 bits to the left: %s\n", decimal_to_binary(input));

    // TODO: then mask out (AND) the bottom 16 bits
    // int masked = shiftedLeft & 0xffff0000; // will leave top bits untouched and kill all lower
    input &= 0x0000;
    printf("Input, with bottom 16 bits masked out: %s\n", decimal_to_binary(input));

    // TODO: finally add (OR) the hex number 0x07FF to produce the final resultant number
    int oredNum = input | 0x07FF;
    printf("Input, after 'or-ing' with 0x07FF: %i\n", oredNum);

    // TODO: Display the final result in binary, hexadecimal, and decimal to the console.
    printf("Resulting number, decimal: %i\n", oredNum);
    printf("Resulting number, binary: %s\n", decimal_to_binary(oredNum));
    printf("Resulting number, hexadecimal: %X\n", oredNum);

    // TODO: d.	Write your program in a modular fashion to eliminate redundant code.

    return 0;
}
