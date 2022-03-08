#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <float.h>
#include "worker.h"

int main(int argc, char **argv) {
	char ch;
	char path[PATHLENGTH];
	char *startdir = ".";
    char *image_file = NULL;
	while((ch = getopt(argc, argv, "d:")) != -1) {
		switch (ch) {
			case 'd':
			startdir = optarg;
			break;
			default:
			fprintf(stderr, "Usage: queryone [-d DIRECTORY_NAME] FILE_NAME\n");
			exit(1);
		}
	}
	if (optind != argc-1) {
		fprintf(stderr, "Usage: queryone [-d DIRECTORY_NAME] FILE_NAME\n");
	} else
		image_file = argv[optind];
	// Open the directory provided by the user (or current working directory)
	DIR *dirp;
	if((dirp = opendir(startdir)) == NULL) {
		perror("opendir");
		exit(1);
	} 
	/* For each entry in the directory, eliminate . and .., and check
	* to make sure that the entry is a directory, then call run_worker
	* to process the image files contained in the directory.
	*/		
	struct dirent *dp;
    CompRecord CRec;
	float min = FLT_MAX;
	char min_name[PATHLENGTH];
	//CompRecord CRec_temp;

	//printf("111, Jessica is smart \n");
	if(image_file != NULL){
		Image* img = read_image(image_file);
		//printf("111, Jessica is a very stupid paozi not like edison is a very very smart raccoon \n");
		while((dp = readdir(dirp)) != NULL) {
			if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 || strcmp(dp->d_name, ".svn") == 0){
				continue;
			}
			strncpy(path, startdir, PATHLENGTH);
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
			//printf("222, edison is smart \n");
			if(S_ISDIR(sbuf.st_mode)) {
				//printf("enter into sub directory \n");
				CRec = process_dir(path, img, -1);

				if (CRec.distance < min){
					min = CRec.distance;
					//min_name = CRec.filename;
					strncpy(min_name,CRec.filename, PATHLENGTH);
				}
				//printf("name is %s , min is %f\n", min_name, min);
				printf("Processing all images in directory: %s \n", path);
			}
			
		}
		free(img->p);
        free(img);
		printf("The most similar image is %s with a distance of %f\n", min_name, min);
	}
	return 0;
}
