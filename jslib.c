#define __JSLIB__
#include "jslib.h"

static char includePath[PATH_MAX] = ".";

static const char *helpCode = 
	"function help() { Object.keys(system).forEach(function (key) { print(typeof system[key], key + '\\n');  }); };\n";

static const char *stacktraceCode =
	"Error.prototype.toString = function() {\n"
	"var s = this.name;\n"
	"if ('message' in this) s += ': ' + this.message;\n"
	"if ('stackTrace' in this) s += this.stackTrace;\n"
	"return s;\n"
	"};\n"
;

void evalFile_() {
	int i, top = getTop();
	for (i = 1; i < top; ++i) {
		const char *name = toString(i);
		char path[PATH_MAX];
		if (strncmp(name, "/", 1) == 0) {
			includeFile(name);
		} else {
			strcpy(path, includePath);
			strcat(path, "/");
			strcat(path, name);
			includeFile(path);
		}
	}
}

void print_() {
	int i, top = getTop();
	for (i = 1; i < top; ++i) 
		printf("%s%s", toString(i), i < top - 1 ? " " : "");
	returnUndefined();
}

void printError_() {
	int i, top = getTop();
	for (i = 1; i < top; ++i) 
		fprintf(stderr, "%s%s", toString(i), i < top - 1 ? " " : "");
	returnUndefined();
}

void initJsArgs(char *argv0, int from, int to, char *argv[]) {
	if (strcmp(argv[0], "") != 0) {
		if (readlink(argv0, includePath, sizeof(includePath)) > 0) {
			char *base = strrchr(includePath, '/');
			if (base) base[0] = '\0';
		}
	}

	newArray();
	array_setString(0, argv0);

	int i = from;
	while (i < to) {
		array_setString(i - from + 1, argv[i]);
		i++;
	}
	
	global_setProperty("argv");
	global_setNumber("argc", to - from + 1);
}

void exit_() {
	exit(isNumber(1) ? toInteger(1) : 0);
}

void verbose_() {
	quiet = !toInteger(1);
	returnUndefined();
}

void initJsLib() {
	quiet = 0;
	
	executeCode(helpCode);
	executeCode(stacktraceCode);

	regGlobalFunction(verbose);
	regGlobalFunction(evalFile);
	regGlobalFunction(print);
	regGlobalFunction(printError);
	regGlobalFunction(exit);

	newObject();
	{
		initFiles();
		initOs();
		initProcess();
		initFs();
	}
	global_setProperty("system");
}
