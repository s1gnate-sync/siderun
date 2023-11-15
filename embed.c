#include "jslib.h"

static char *jsSectionName = "js=";
static char *jslibSectionName = "jslib=";

char doEmbed(char *in, char *out, char sectionName[], char sectionValue[]) {
	int pid = fork();
	if (pid < 0) {
		printLastError("embed.fork");
		return 0;
	}

	if (pid) {
		int status;
		waitpid(pid, &status, 0);
		return WIFEXITED(status) && WEXITSTATUS(status) == 0;
	} else {
		char sectionArg[strlen(sectionName) + strlen(sectionValue) + 2];
		strcpy(sectionArg, sectionName);
		strcat(sectionArg, "=");
		strcat(sectionArg, sectionValue);
		strcat(sectionArg, "\0");
		
		char *argv[] = {
			"objcopy", 
			"--add-section", 
			sectionArg, 
			in,
			out,
			NULL
		};

		execvp("objcopy", argv);
		printLastError("doEmbed.execvp");
		return 0;	
	}
}

void embed_() {
	if (!isString(1)) {
		printError("embed", "wrong usage");
	}

	char *inFile = malloc(PATH_MAX + 1);
	int inFileLen;
	if ((inFileLen = readlink("/proc/self/exe", inFile, PATH_MAX) < 0)) {
		printLastError("embed.readlink");
		returnFalse();
	}
	
	char *outFile = (char *) toString(1);
	if (isString(2) && isString(3)) {
		char tmpFilename[] = "exeXXXXXX";
		mkstemp(tmpFilename);

		if (!doEmbed(inFile, tmpFilename, "js", (char *) toString(2))) {
			unlink(tmpFilename);
			printError("embed.doEmbed", "Can't embed app file");
			returnFalse();
		}

		if (!doEmbed(tmpFilename, outFile, "jslib", (char *) toString(3))) {
			unlink(tmpFilename);
			printError("embed.doEmbed", "Can't embed lib file");
			returnFalse();
		}

		unlink(tmpFilename);
	} else if (isString(2)) {
		if (!doEmbed(inFile, outFile, "js", (char *) toString(2))) {
			printError("embed.doEmbed", "Can't embed app file");
			returnFalse();
		}
			
	} else if (isString(3)) {
		if (!doEmbed(inFile, outFile, "jslib", (char *) toString(3))) {
			printError("embed.doEmbed", "Can't embed lib file");
			returnFalse();
		}
	}

	returnTrue();
}

void initEmbed() {
	regGlobalFunction(embed);
}
