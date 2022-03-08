#include <stdio.h>
#include <stdlib.h>

/*
 * Include the function prototypes of all those functions implemented in image_processing.c.
 * Do not remove or modify them in any way.
 */

void remove_red();
void convert_to_black_and_white();
void instagram_square();

int main(int argc, char *argv[]) {
        /* Task 1 needs to be completed here: Make sure that the command line arguments are correct. In case an error exists,
         * print the error message and terminate the program. Otherwise, invoke the corresponding
         * image processing function. */
        if(argc != 2){
                printf("Error: Expecting one command-line argument, which needs to be either 1, 2, or 3..");
        }
        if(argv[1][1] != '\0'){
                 printf("Error: Expecting one command-line argument, which needs to be either 1, 2, or 3..");
        }else{
                if(argv[1][0] == '1'){
                remove_red();
                }else if(argv[1][0] == '2'){
                        convert_to_black_and_white();
                }else if(argv[1][0] == '3'){
                        instagram_square();
                }else{
                        printf("Error: Expecting one command-line argument, which needs to be either 1, 2, or 3.");
                }
        }
        return 0;
}
