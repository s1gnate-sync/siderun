#ifndef __SIDERUN_JS_H__
#define __SIDERUN_JS_H__

#include <mujs.h>
#include "siderun.h"

static js_State *J = NULL;

typedef void (*Callback)();
typedef struct FunctionData FunctionData;
struct FunctionData {
	const char *name;
	Callback callback;
};

void newState();
void freeState();
void collectGarbage();
void includeFile(const char*);
char executeFile(const char*);
char executeCode(const char*);

int getTop();

void pushUndefined();
#define returnUndefined() { pushUndefined(); return; }

void pushNull();
#define returnNull() { pushNull(); return; }

void pushTrue();
#define returnTrue() { pushTrue(); return; }

void pushFalse();
#define returnFalse() { pushFalse(); return; }

void pushBoolean(char);
#define returnBoolean(bool) { pushBoolean(bool); return; }

void pushNumber(double);
#define returnNumber(number) { pushNumber(number); return; }

void pushString(char*);
#define returnString(string) { pushString(string); return; }

void newFunction(const char*, Callback, int);

void throwError(const char*);

double toDouble(int);
int toInteger(int);
const char* toString(int);

void global_setProperty(const char*);
#define global_setFunction(name, fn, argc)	{ newFunction(name, fn, argc); global_setProperty(name); }
#define global_setNumber(name, number)		{ pushNumber(number); global_setProperty(name); }
#define global_setString(name, string)		{ pushString(string); global_setProperty(name); }
#define global_setTrue(name)				{ pushTrue(); global_setProperty(name); }
#define global_setFalse(name)				{ pushFalse(); global_setProperty(name); }
#define global_setBoolean(name, bool)		{ pushBoolean(bool); global_setProperty(name); }
#define global_setNull(name)				{ pushNull(); global_setProperty(name); }
#define global_setUndefined(name)			{ pushUndefined(); global_setProperty(name); } 

void newObject();
void object_setProperty(const char*);
#define object_setFunction(name, fn, argc)	{ newFunction(name, fn, argc); object_setProperty(name); }
#define object_setNumber(name, number)		{ pushNumber(number); object_setProperty(name); }
#define object_setString(name, string)		{ pushString(string); object_setProperty(name); }
#define object_setTrue(name) 				{ pushTrue(); object_setProperty(name); }
#define object_setFalse(name)				{ pushFalse(); object_setProperty(name); }
#define object_setBoolean(name, bool)		{ pushBoolean(bool); object_setProperty(name); }
#define object_setNull(name)				{ pushNull(); object_setProperty(name); }
#define object_setUndefined(name) 			{ pushUndefined(); object_setProperty(name); }

char isUserdata(int, const char*);
void* toUserdata(int, const char*);
void newUserdata(const char*, void*);
#define newUserdataObject(name, data) 	{ newObject(); newUserdata(name, data); }

void newArray();
void array_setIndex(int);
int array_getLength(int);
int array_toInteger(int, int);
const char* array_toString(int, int);

#define array_setNumber(index, number)	{ pushNumber(number); array_setIndex(index); }
#define array_setString(index, string)	{ pushString(string); array_setIndex(index); }
#define array_setTrue(index)			{ pushTrue(); array_setIndex(index); }
#define array_setFalse(index)			{ pushFalse(); array_setIndex(index); }
#define array_setBoolean(index, bool)	{ pushBoolean(bool); array_setIndex(index); }
#define array_setNull(index)			{ pushNull(); array_setIndex(index); }
#define array_setUndefined(index)		{ pushUndefined(); array_setIndex(index); }

char isArray(int);
char isString(int);
char isNumber(int);
char isNull(int);

void copy(int);

#endif
