#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <math.h>
#include <float.h>
#include "worker.h"


/*
 * Read an image from a file and create a corresponding 
 * image struct 
 */

Image* read_image(char *filename){
    //printf("%s \n", filename);
    FILE * img_file = fopen(filename, "r");
    //printf("before fscanf %s %p\n", filename, img_file);
    
    // Open file
    if (img_file == NULL){ 
        printf("Cannot open file \n"); 
        exit(0); 
    } 
  
    char type[200];
    fscanf(img_file, "%s", &type[0]);
    //printf("%s \n", type);
    if (strcmp("P3", type) == 0){
        int width;
        int height;
        fscanf(img_file, "%d %d", &width, &height);
        //printf("%d %d \n", width, height);

        int max_value;
        fscanf(img_file, "%d", &max_value);
        //printf("%d \n", max_value);

        Pixel *pixels  = malloc(width*height*sizeof(Pixel));
        int pixel_color[3];
        int pixel_size = 0;
        while (fscanf(img_file, "%d %d %d", &pixel_color[0], &pixel_color[1], &pixel_color[2]) != EOF 
            && pixel_size < width*height) {
            Pixel* pixel = malloc(sizeof(Pixel));
            pixel->red = pixel_color[0];
            pixel->green = pixel_color[1];
            pixel->blue = pixel_color[2];
            pixels[pixel_size] = *pixel;
            pixel_size++;
        }

        Image* img = malloc(sizeof(Image));
        img->width = width;
        img->height = height;
        img->max_value = max_value;
        img->p = pixels;

        fclose(img_file);
        //printf("almost succeed \n");
        return img;
    }else{
        return NULL;
    }
}

/*
 * Print an image based on the provided Image struct 
 */

void print_image(Image *img){
        printf("P3\n");
        printf("%d %d\n", img->width, img->height);
        printf("%d\n", img->max_value);
       
        for(int i=0; i<img->width*img->height; i++)
           printf("%d %d %d  ", img->p[i].red, img->p[i].green, img->p[i].blue);
        printf("\n");
}

/*
 * Compute the Euclidian distance between two pixels 
 */
float eucl_distance (Pixel p1, Pixel p2) {
        return sqrt( pow(p1.red - p2.red,2 ) + pow( p1.blue - p2.blue, 2) + pow(p1.green - p2.green, 2));
}

/*
 * Compute the average Euclidian distance between the pixels 
 * in the image provided by img1 and the image contained in
 * the file filename
 */

float compare_images(Image *img1, char *filename) {
        Image* img2 = read_image(filename);
        int width1 = img1->width;
        int height1 = img1->height;
        int width2 = img2->width;
        int height2 = img2->height;
        if(width1 == width2 && height1 == height2){
            float result = 0;
            int pixel_size = 0;
            while (pixel_size < width1*height1) {
                result = result + eucl_distance(img1->p[pixel_size], img2->p[pixel_size]);
                pixel_size++;
            }
            free(img2->p);
            free(img2);
            result = result / (width1*height1);
            return result;
        }else{
            return FLT_MAX;
        }

}

/* process all files in one directory and find most similar image among them
* - open the directory and find all files in it 
* - for each file read the image in it 
* - compare the image read to the image passed as parameter 
* - keep track of the image that is most similar 
* - write a struct CompRecord with the info for the most similar image to out_fd
*/
CompRecord process_dir(char *dirname, Image *img, int out_fd){
    printf("get into Process_dir \n");
    DIR* mydir = opendir(dirname);
    char path[PATHLENGTH];
    
    struct dirent *dp;
    CompRecord CRec;

    // 如果文件夹里有至少一张图片
    // 那么cur min就是这张图片和 img之间的距离
    float min = FLT_MAX;
    float cur = FLT_MAX;
    CRec.distance = min;
    strncpy(CRec.filename, "NULL", PATHLENGTH);
    
    //如果文件夹里有多张图片
    while((dp = readdir(mydir)) != NULL) {
        if(strcmp(dp->d_name, ".") == 0 || 
            strcmp(dp->d_name, "..") == 0 ||
            strcmp(dp->d_name, ".svn") == 0||
            strcmp(dp->d_name, ".DS_Store") == 0){
                continue;
        }
        strncpy(path, dirname, PATHLENGTH);
        strncat(path, "/", PATHLENGTH - strlen(path) - 1);
        strncat(path, dp->d_name, PATHLENGTH - strlen(path) - 1);

        struct stat sbuf;
        if(stat(path, &sbuf) == -1) {
            //This should only fail if we got the path wrong
            // or we don't have permissions on this entry.
            perror("stat");
            exit(1);
        }
        // Only call process_dir if it is a directory
        // Otherwise ignore it.
        if(S_ISREG(sbuf.st_mode)) {
            cur = compare_images(img, path);
            if (cur < min){
                min = cur;
                CRec.distance = min;
                strncpy(CRec.filename,dp->d_name, PATHLENGTH); 
            }
        }
    }
    if(out_fd == -1){
        write(out_fd,(void *)&CRec,sizeof(CompRecord));
    }
    printf("Process_dir: similar image is %s with a distance of %f in directory: %s \n", CRec.filename, CRec.distance, path);
    return CRec;
}
