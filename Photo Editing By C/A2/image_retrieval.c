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
	/*
	//管道用于子进程发送数据给主进程
  	int pipefd[2];
	//管道数组2,用于主进程分别发数据给子进程
  	//int pipearr[3][2];

	pid_t pid;
	// 干啥用的？
 	CompRecord * buff = NULL;

	pipe(pipefd);
	//创建管道
	if(pipe(pipefd) < 0){
		printf("Create Pipe Error! \n");
	}

	管道用于接收父进程发过来的数据，这个assignment用不到
	int i;
	for(i=0; i<3; i++){
		if(0 > pipe(pipearr[i])){
			printf("Create Pipe Error!\n");
		}
    }

	
	pid = fork();

	//如果fork返回值小于0，说明创建子进程失败
	if(pid < (pid_t)0){
		printf("Fork Error!\n");
		perror("fork");
     	return -1;
	}

	//如果fork返回值为0，表示进入子进程的逻辑
	if( pid == (pid_t)0){
		wait(1);
		close(pipefd[1]);
		//write(pipefd[1], "Hello\n", 6);
		//发送格式化数据给主进程
		FILE *f;
		f = fdopen(pipefd[1], "w");
		while(read(pipefd[0],&buff, sizeof(CompRecord)) > 0){
			write(STDOUT_FILENO,&buff,1);
		}
		write(STDOUT_FILENO,"\n",1);
		fprintf(f, "I am %d\n", getpid());
		fclose(f);
		close(pipefd[0]);

		
		接收父进程发过来的数据
		read(pipearr[i][0], buff, 20);
      	printf("MyPid:%d, Message:%s", getpid(), buff);
		

		//完成后及时退出循环，继续循环会出大问题，和fork的运行逻辑有关！
      	//break;
	}

	//进入父进程的逻辑
	if(pid > (pid_t)0){
		//接收子进程发过来的数据
		read(pipefd[0], buff, 20);
		// min m
		float m = FLT_MAX;
		// min distance CRec name
		int Pathlength;
		char n[Pathlength];
		int i;
		while(&buff[i] != NULL){
			if(buff[i].distance < m){
				m = buff[i].distance;
				strncpy(n, buff[i].filename, Pathlength);
			}
		}
		printf("MyPid:%d, Message:%s", getpid(), buff);
		//发送数据给子进程
		close(pipefd[0]);
      	write(pipefd[1], "Hello My Son\n", 14);
		close(pipefd[1]);
		wait((int*) NULL);
		
	}
	*/
	
	/*
	//管道用于子进程发送数据给主进程
  	int pipefd[2];
	pid_t pid;
 	CompRecord * buff = NULL;
	pipe(pipefd);

	//创建管道
	if(pipe(pipefd) < 0){
		printf("Create Pipe Error! \n");
	}
	*/

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
		

		while((dp = readdir(dirp)) != NULL) {
			// fork();
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
			
			int dir_num = 0;
			if(S_ISDIR(sbuf.st_mode)) {
				dir_num++;
			}

			//管道用于子进程发送数据给主进程
			int pipefd[dir_num][2];
			pid_t pid;
			// CompRecord * buff = NULL;
			pipe(pipefd[dir_num]);

			//创建管道
			if(pipe(pipefd[dir_num]) < 0){
				printf("Create Pipe Error! \n");
				exit(1);
			}

			//pid = fork();

			// Only call process_dir if it is a directory
			// Otherwise ignore it.
			if(S_ISDIR(sbuf.st_mode)) {
				pid = fork();

				//如果fork返回值小于0，说明创建子进程失败	
				if(pid < (pid_t)0){
					perror("Fork Error!\n");
					return -1;
				}

				//如果fork返回值为0，表示进入子进程的逻辑
				if( pid == (pid_t)0){
					int c11 = close(pipefd[dir_num][0]);
					if(c11 < 0){
						perror("Error closing \n");
					}
					CRec = process_dir(path, img, pipefd[dir_num][1]);

					int w2 = write(pipefd[dir_num][1],&CRec,sizeof(CompRecord));
					if(w2 < 0){
						perror("Error writing \n");
					}
					printf("********************************** \n");
					printf("Child Process: %s, %f Processing all images in directory: %s \n", CRec.filename, CRec.distance, path);
					printf("Processing all images in directory: %s \n", path);
					// printf("Processing all images in directory: %s \n", path);
					printf("********************************** \n");
					
					int c12 = close(pipefd[dir_num][1]);
					if(c12 < 0){
						perror("Error closing \n");
					}
					exit(0);
					return 0;
				}

				//进入父进程的逻辑
				if(pid > (pid_t)0){
					int c21 = close(pipefd[dir_num][1]);
					if(c21 < 0){
						perror("Error closing \n");
					}
					
					//接收子进程发过来的数据
					printf("read by father1 \n");
					int k2 = read(pipefd[dir_num][0], &CRec,sizeof(CompRecord));
					printf("read by father2 \n");
					if(k2 < 0){
						perror("Error reading \n");
					}
					printf("read by father3 \n");
					
					int c22 = close(pipefd[dir_num][0]);
					if(c22 < 0){
						perror("Error closing \n");
					}
				}
				if(CRec.distance < min){
					min = CRec.distance;
					strncpy(min_name, CRec.filename, PATHLENGTH);
				}
				printf("Result:The most similar image is %s with a distance of %f\n", min_name, min);
				printf("The most similar image is %s with a distance of %f\n", min_name, min);
			}
		}
		free(img->p);
        free(img);
	}
	return 0;
}
