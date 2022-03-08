#include <stdio.h>


/*
 * Read file type, row ,column and max_value of the color
 */
void helper_with_read_file(){
  char type[2] ="P3";
  scanf("%s",type);
  printf("%s \n", type);
  int col;
  int row;
  scanf("%d %d", &col,&row);
  printf("%d %d \n", col, row);
  int max_value;
  scanf("%d", &max_value);
  printf("%d \n", max_value);
}


/*
 * Read an image from the standard input and set the red value of each pixel to
 * zero.
 */
void remove_red(){
  helper_with_read_file();
  int input1, input2, input3;
  while(scanf("%d %d %d", &input1, &input2, &input3) != EOF){
    input1 = 0;
    printf("%d %d %d ", input1, input2, input3);
  }
}

/*
 * Read an image from the standard input and convert it from colour to black and white.
 */
void convert_to_black_and_white(){
  helper_with_read_file();
  int input1, input2, input3, average;
  while(scanf("%d %d %d", &input1, &input2, &input3) != EOF){
    average = (input1 + input2 + input3)/3;
    input1 = input2 = input3 = average;
    printf("%d %d %d ", input1, input2, input3);
  }
}


/*
 * Read an image from the standard input and convert it to a square image.
 */
void instagram_square(){
  char type[2];
  scanf("%s",type);
  printf("%s \n", type);
  int col;
  int row;
  scanf("%d %d", &col,&row);
  int min;
  if(col > row){
    min = row;
  }else if(row > col){
    min = col;
  }else if(row == col){
    min = col;
  }
  col = row = min;
  printf("%d %d \n", col, row);
  int max_value;
  scanf("%d", &max_value);
  printf("%d \n", max_value);

  int input1, input2, input3;
  while(scanf("%d %d %d", &input1, &input2, &input3) != EOF){
    printf("%d %d %d ", input1, input2, input3);
  }

}

