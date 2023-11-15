#include "jsfacade.h"

void onReport(js_State *J, const char* message) {
	(void)(J);
	fprintf(stderr, "%s\n", message);
}

void defineGlobal() {
	if (isNull(2)) {
		js_copy(J, 3);
		js_defglobal(J, toString(1), JS_READONLY | JS_DONTENUM);
	} else {
		js_getglobal(J, toString(1));
		js_copy(J, 3);
		js_defproperty(J, -2, toString(2), JS_READONLY | JS_DONTENUM);	
	}
	returnUndefined();
}

void newState() {
	if (J) {
		fprintf(stderr, "creating new state without freeing the previous one\n");
		abort();
	}
	
	J = js_newstate(NULL, NULL, JS_STRICT);
	js_setreport(J, onReport);

	newObject();
	newFunction("define", defineGlobal, 0);
	object_setProperty("define");
	global_setProperty("global");
}

void collectGarbage() {
	if (J) {
		js_gc(J, 0);
	}
}

void freeState() {
	if (!J) {
		fprintf(stderr, "freeing a state without creating one\n");
		abort();
	}

	js_freestate(J);
	J = NULL;
}

int getTop() {
	return js_gettop(J);
}

void includeFile(const char *file) {
	js_loadfile(J, file);
	js_pushundefined(J);
	js_call(J, 0);
	js_pop(J, 1);
	js_pushundefined(J);
}

char executeCode(const char *source) {
	char needState = !J;
	if (needState) {
		newState();
	}

	if (js_try(J)) {
		fprintf(stderr, "executeCode: %s", js_tostring(J, -1));
		return 1;
	}

	int status = js_dostring(J, source);
	js_endtry(J);

	if (needState) {
		freeState();
	}

	return status;
}

char executeFile(const char *file) {
	char needState = !J;
	if (needState) {
		newState();
	}

	if (js_try(J)) {		
		fprintf(stderr, "executeFile: %s in %s", js_tostring(J, -1), file);
		return 1;
	}

	int status = js_dofile(J, file);
	js_endtry(J);

	if (needState) {
		freeState();
	}

	return status;	
}

void global_setProperty(const char *name) {
	js_defglobal(J, name, JS_READONLY | JS_DONTENUM);
}

void object_setProperty(const char *name) {
	js_setproperty(J, -2, name);
}

void array_setIndex(int index) {
	js_setindex(J, -2, index);
}

void pushUndefined() {
	js_pushundefined(J);
}

void pushNull() {
	js_pushnull(J);
}

void pushTrue() {
	js_pushboolean(J, 1);
}

void pushFalse() {
	js_pushboolean(J, 0);
}

void pushBoolean(char bool) {
	js_pushboolean(J, !!bool);
}

void pushNumber(double number) {
	js_pushnumber(J, number);
}

void pushString(char *string) {
	js_pushstring(J, string);
}

void newObject() {
	js_newobject(J);
}

void newArray() {
	js_newarray(J);
}

void array_getIndex(int index, int itemIndex) {
	if (!isArray(index)) {
		js_error(J, "array_getLength: #%d is not an array", index);
	}

	js_getindex(J, index, itemIndex);
}

int array_getLength(int index) {
	if (!isArray(index)) {
		js_error(J, "array_getLength: #%d is not an array", index);
	}
	
	return js_getlength(J, index);
}

int array_toInteger(int index, int itemIndex) {
	array_getIndex(index, itemIndex);
	return toInteger(-1);
}

const char* array_toString(int index, int itemIndex) {
	array_getIndex(index, itemIndex);
	return toString(-1);	
}


void onFunctionCall(js_State *J) {
	FunctionData *currentFunctionCall = NULL;
	if ((currentFunctionCall = js_currentfunctiondata(J))) {
		int top = getTop();	
		currentFunctionCall->callback();
		if (getTop() == top) {
			fprintf(stderr, "%s: has no return value\n", currentFunctionCall->name);
			pushUndefined();
		}
	} else {
		js_error(J, "Invalid function data");
	}
}

void onFinalize(js_State *J, void *p) {
	(void)(J);
	(void)(p);
	// free(p);
}

void newUserdata(const char *name, void *data) {
	return js_newuserdata(J, name, data, onFinalize);
}

char isUserdata(int index, const char *name) {
	return js_isuserdata(J, index, name);
}

void* toUserdata(int index, const char *name) {
	if (!isUserdata(index, name)) {
		js_error(J, "toUserdata: #%d is not a %s userdata", index, name);
	}
	
	return js_touserdata(J, index, name);
}

void throwError(const char *message) {
	js_error(J, "%s", message);	
}

void newFunction(const char *name, Callback function, int argc) {
	FunctionData *data = malloc(sizeof(struct FunctionData));
	data->callback = function;
	data->name = name;
	js_newcfunctionx(J, onFunctionCall, name, argc, data, onFinalize);
}

double toDouble(int index) {
	if (!js_isnumber(J, index)) {
		js_error(J, "toDouble: #%d is not a number", index);
	}
	
	return js_tonumber(J, index);
}

int toInteger(int index) {
	if (!js_isnumber(J, index)) {
		js_error(J, "toInteger: #%d is not a number", index);
	}
	
	return js_toint32(J, index);
}

char isNumber(int index) {
	return js_isnumber(J, index);
}

const char* toString(int index) {
	return js_tostring(J, index);
}

char isNull(int index) {
	return js_isnull(J, index);
}

char isArray(int index) {
	return js_isarray(J, index);
}

char isString(int index) {
	return js_isstring(J, index);
}

