/*
 * mmaptest.c
 *
 *  Created on: 14.09.2014
 *      Author: andreas
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>

int main(int argc, char **argv) {
	char buf[4096];
	char buf2[4096];
	int fd, i;
	char *path;
	unsigned int write_count;
	unsigned long file_size;
	unsigned long file_offset = 0;
	char* data;

	if (argc < 4) {
		fprintf(stderr, "Too few arguments\n");
		return EXIT_FAILURE;
	}

	path = argv[1];
	file_size = atol(argv[2]) * 1024 * 1024;
	write_count = atoi(argv[3]);
	if (argc > 4)
		file_offset = atol(argv[4]) * 1024 * 1024;

	if ((fd = open("/dev/urandom", O_RDONLY)) < 0) {
		perror("Error opening /dev/urandom");
		return EXIT_FAILURE;
	}

	if (read(fd, buf, sizeof(buf)) != 4096) {
		perror("Error initializing buffer");
		return EXIT_FAILURE;
	}

	close(fd);

	printf("Initializeing target...\nPath: %s, Size: %luMB, Offset: %luMB, Count: %u\n",
			path, file_size / 1024 / 1024, file_offset / 1024 / 1024, write_count);


	if ((fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
		perror("Error opening target");
		return EXIT_FAILURE;
	}

	if (ftruncate(fd, file_size)) {
		perror("Error resizing");
		return EXIT_FAILURE;
	}


	puts("Mapping file");
	if ((data = mmap(NULL, file_size - file_offset, PROT_READ | PROT_WRITE, MAP_SHARED, fd, file_offset)) == MAP_FAILED) {
		perror("Mmap failed");
		return EXIT_FAILURE;
	}

	puts("Writing data...");
	for (i = 0; i < write_count; ++i) {
		// first read the page
		memcpy(buf2, data + i * 4096, sizeof(buf2));
		// then write it
		memcpy(data + i * 4096, buf, sizeof(buf));
		if (msync(data, file_size - file_offset, MS_SYNC)) {
			perror("Msync failed");
			return EXIT_FAILURE;
		}
	}

	puts("Unmapping file...");
	if (munmap(data, file_size - file_offset)) {
		perror("Munmap failed");
		return EXIT_FAILURE;
	}

	close(fd);

	puts("Success!");
	return EXIT_SUCCESS;
}
