#include "siderun.h"

int main(int argc, char *argv[]) {
	int status = 1;

	newState();
	initJsLib();
	
	SectionInfo *jsSection = getSectionInfo("/proc/self/exe", "jslib");
	if (jsSection) {
		int fd = open("/proc/self/exe", O_RDONLY);
		lseek(fd, jsSection->offset, SEEK_SET);
		char code[jsSection->size + 1];
		ssize_t len;
		if ((len = read(fd, &code[0], sizeof(code) - 1)) == -1) {
			perror("cannot read jslib section");
			return 1;
		}
		close(fd);		
		code[len] = '\0';
		if (executeCode(code)) {
			freeState();
			perror("error in jslib section code");
			return 1;
		}
	}
	
	jsSection = getSectionInfo("/proc/self/exe", "js");
	if (jsSection) {
		int fd = open("/proc/self/exe", O_RDONLY);
		lseek(fd, jsSection->offset, SEEK_SET);
		char code[jsSection->size + 1];
		ssize_t len;
		if ((len = read(fd, &code[0], sizeof(code) - 1)) == -1) {
			perror("cannot read jsmain section");
			return 1;
		}
		close(fd);
		code[len] = '\0';
		initJsArgs(argv[0], 1, argc, argv);
		if ((status = executeCode(code))) {
			perror("error in jsmain sectioncode");
		}
	} else {
		initEmbed();
		
		if (argc > 2 && strcmp(argv[1], "-c") == 0) {
			initJsArgs("[string]", 3, argc, argv);
			status = executeCode(argv[2]);
		} else if (argc > 1) {
			initJsArgs(argv[1], 2, argc, argv);
			status = executeFile(argv[1]);
		} else {
			fprintf(stderr, "usage: %s <file> | -c <inline>\n", argv[0]);
		}		
	}

	freeState();

	return status;
}
