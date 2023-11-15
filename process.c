#include "jslib.h"

void fork_() {
	int pid = fork();
	if (pid < 0) {
		printLastError("fork");
		returnFalse();
	}

	returnNumber(pid);
}

void getcwd_() {
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		printLastError("getcwd");
		returnNull();
	}

	returnString(cwd);
}

void getegid_() {
	returnNumber(getegid());
}

void geteuid_() {
	returnNumber(geteuid());
}

void gethostname_() {
	char buf[1024];
	if (gethostname(&buf[0], sizeof(buf))) {
		printLastError("gethostname");
		returnFalse();
	}

	returnString(buf);
}

void getgid_() {
	returnNumber(getgid());
}

void getgroups_() {
	/*
	https://stackoverflow.com/questions/22104383/proper-way-to-get-groups-of-a-user-in-linux-using-c
	*/
}

void getpgid_() {
	returnNumber(getpgid(toInteger(1)));
}

void getpgrp_() {
	pid_t ret;
	
	if ((ret = getpgrp())) {
		printLastError("getpgrp");
		returnFalse();
	}
	
	returnNumber(ret);
}

void getpid_() {
	returnNumber(getpid());
}

void getppid_() {
	returnNumber(getppid());
}

void getresgid_() {
	gid_t r, e, s;
	if (getresgid(&r, &e, &s)) {
		printLastError("getresgid");
		returnFalse();
	}
	
	newArray();
	array_setNumber(0, r);
	array_setNumber(1, e);
	array_setNumber(2, s);
}

void getresuid_() {
	uid_t r, e, s;
	if (getresuid(&r, &e, &s)) {
		printLastError("getresuid");
		returnFalse();
	}
	
	newArray();
	array_setNumber(0, r);
	array_setNumber(1, e);
	array_setNumber(2, s);
	
}

void getrlimit_() {
	struct rlimit rlim;
	if (getrlimit(toInteger(1), &rlim)) {
		printLastError("rlimit");
		returnFalse();
	}
	
	newObject();
	object_setNumber("rlim_cur", rlim.rlim_cur);
	object_setNumber("rlim_max", rlim.rlim_max);
}

void getsid_() {
	pid_t ret = getsid(toInteger(1));
	if (ret) {
		printLastError("getsid");
		returnFalse();
	}
	returnNumber(ret);
}

void getuid_() {
	pushNumber(getuid());
}

void kill_() {
	if (kill(toInteger(1), toInteger(2))) {
		printLastError("kill");
		returnFalse();
	}	

	returnTrue();
}

void setgid_() {
	if (setgid(toInteger(1))) {
		printLastError("setgid");
		returnFalse();
	}

	returnTrue();
}

void setgroups_() {
	// TODO
}

void sethostname_() {
	// TODO
}

void setpgid_() {
	if (setpgid(toInteger(1), toInteger(2))) {
		printLastError("setpgid");
		returnFalse();
	}
	
	returnUndefined();
}

void setregid_() {
	if (setregid(toInteger(1), toInteger(2))) {
		printLastError("setregid");
		returnFalse();
	}
	returnTrue();
}

void setresgid_() {
	if (setresgid(toInteger(1), toInteger(2), toInteger(3))) {
		printLastError("setresgid");
		returnFalse();
	}
	returnTrue();
}

void setresuid_() {
	if (setresuid(toInteger(1), toInteger(2), toInteger(3))) {
		printLastError("setresuid");
		returnFalse();
	}
	returnTrue();
}

void setreuid_() {
	if (setreuid(toInteger(1), toInteger(2))) {
		printLastError("setreuid");
		returnFalse();
	}
	returnTrue();
}

void setrlimit_() {
	// TODO
}

void setsid_() {
	// TODO
}

void setuid_() {
	char result = setuid(toInteger(1));
	if (result == 0) {
		pushTrue();
	} else {
		printLastError("setuid");
		pushFalse();
	}
}

void umask_() {
	umask(toInteger(1));
	returnTrue();
}

void nice_() {
	if (nice(toInteger(1)))	{
		printLastError("nice");
		returnFalse();
	}
	
	returnTrue();
}

void unshare_() {
	if (unshare(toInteger(1))) {
		printLastError("unshare");
		returnFalse();
	}

	returnTrue();
}

void waitpid_() {
	int status;
	int ret;
	if ((ret = waitpid(toInteger(1), &status, toInteger(2))) < 0) {
		printLastError("waitpid");
		returnFalse();
	}

	returnNumber(status);

	// newArray();
	// array_setNumber(0, ret);
	// array_setNumber(1, status);
}

void execv_() {
	int argc = array_getLength(2);
	char *argv[argc + 1];
	for (int i = 0; i < argc; i++)
		argv[i] = (char *) array_toString(2, i);
	argv[argc] = NULL;

	execv(toString(1), argv);			
	printLastError("execv");
	returnFalse();
}

void execve_() {
	int argc = array_getLength(2);
	char *argv[argc + 1];
	for (int i = 0; i < argc; i++)
		argv[i] = (char *) array_toString(2, i);
	argv[argc] = NULL;

 	argc = array_getLength(3);
	char *envp[argc + 1];
	for (int i = 0; i < argc; i++) 
		envp[i] = (char*) array_toString(3, i);
	envp[argc] = NULL;
	
	execve(toString(1), argv, envp);	
	printLastError("execve");
	returnFalse();
}

void execvp_() {
	int argc = array_getLength(2);
	char *argv[argc + 1];
	for (int i = 0; i < argc; i++) 
		argv[i] = (char*) array_toString(2, i);
	argv[argc] = NULL;
	
	execvp(toString(1), argv);
	printLastError("execvp");
	returnFalse();
}

void execvpe_() {
	int argc = array_getLength(2);
	char *argv[argc + 1];
	for (int i = 0; i < argc; i++)
		argv[i] = (char *) array_toString(2, i);
	argv[argc] = NULL;

 	argc = array_getLength(3);
	char *envp[argc + 1];
	for (int i = 0; i < argc; i++) 
		envp[i] = (char*) array_toString(3, i);
	envp[argc] = NULL;
	
	execvpe(toString(1), argv, envp);	
	
	printLastError("execvpe");
	returnFalse();
}

void clearenv_() {
	if (clearenv()) {
		printLastError("cleanenv");
		returnFalse();
	}
	
	returnTrue();
}

void getenv_() {
	char *ret = getenv(toString(1));
	if (ret == NULL) returnUndefined();
	returnString(ret);
}

void setenv_() {
	if (setenv(toString(1), toString(2), toInteger(3))) {
		printLastError("setenv");
		returnFalse();
	}
	
	returnTrue();
}

void putenv_() {
	if (putenv(toString(1))) {
		printLastError("putenv");
		returnFalse();
	}

	returnTrue();
}

void pipe_() {
	int fd[2];
	if (pipe(fd)) {
		printLastError("pipe");
		returnFalse();
	}

	newArray();
	array_setNumber(0, fd[0]);
	array_setNumber(1, fd[1]);
}

void pipe2_() {
	int fd[2];
	if (pipe2(fd, toInteger(1))) {
		printLastError("pipe2");
		returnFalse();
	}

	newArray();
	array_setNumber(0, fd[0]);
	array_setNumber(1, fd[1]);
}

void chdir_() {
	if (chdir(toString(1))) {
		printLastError("chdir");
		returnFalse();
	}
	else returnTrue();
}

void WIFEXITED_() {
	returnBoolean(WIFEXITED(toInteger(1)));
}

void WEXITSTATUS_() {
	returnNumber(WEXITSTATUS(toInteger(1)));
}

void WIFSIGNALED_() {
	returnBoolean(WIFSIGNALED(toInteger(1)));
}

void WTERMSIG_() {
	returnNumber(WTERMSIG(toInteger(1)));
}

void WCOREDUMP_() {
	returnBoolean(WCOREDUMP(toInteger(1)));
}

void WIFSTOPPED_() {
	returnBoolean(WIFSTOPPED(toInteger(1)));
}

void WSTOPSIG_() {
	returnNumber(WSTOPSIG(toInteger(1)));
}

void WIFCONTINUED_() {
	returnBoolean(WIFCONTINUED(toInteger(1)));
}

void initProcess() {
	regFunction(fork);
	regFunction(getcwd);
	regFunction(getegid);
	regFunction(geteuid);
	regFunction(gethostname);
	regFunction(getgid);
	regFunction(getgroups);
	regFunction(getpgid);
	regFunction(getpgrp);
	regFunction(getpid);
	regFunction(getppid);
	regFunction(getresgid);
	regFunction(getresuid);
	regFunction(getrlimit);
	regFunction(getsid);
	regFunction(getuid);
	regFunction(kill);
	regFunction(execv);
	regFunction(execve);
	regFunction(execvpe);
	regFunction(execvp);
	regFunction(nice);
	regFunction(setgid);
	regFunction(setgroups);
	regFunction(sethostname);
	regFunction(setpgid);
	regFunction(setregid);
	regFunction(setresgid);
	regFunction(setresuid);
	regFunction(setreuid);
	regFunction(setrlimit);
	regFunction(setsid);
	regFunction(setuid);
	regFunction(umask);
	regFunction(unshare);
	regFunction(waitpid);
	regFunction(getenv);
	regFunction(setenv);
	regFunction(clearenv);
	regFunction(putenv);
	regFunction(pipe);
	regFunction(pipe2);
	
	regFunction(chdir);

	object_setNumber("WNOHANG", WNOHANG);
	object_setNumber("WUNTRACED", WUNTRACED);
	object_setNumber("WCONTINUED", WCONTINUED);

	object_setNumber("SIGTERM", SIGTERM);
	
	regFunction(WIFEXITED);
	regFunction(WEXITSTATUS);
	regFunction(WIFSIGNALED);
	regFunction(WTERMSIG);
	regFunction(WCOREDUMP);
	regFunction(WIFSTOPPED);
	regFunction(WSTOPSIG);
	regFunction(WIFCONTINUED);

	object_setNumber("CLONE_VM", CLONE_VM);
	object_setNumber("CLONE_FS", CLONE_FS);
	object_setNumber("CLONE_FILES", CLONE_FILES);
	object_setNumber("CLONE_SIGHAND", CLONE_SIGHAND);
	object_setNumber("CLONE_PIDFD", CLONE_PIDFD);
	object_setNumber("CLONE_PTRACE", CLONE_PTRACE);
	object_setNumber("CLONE_VFORK", CLONE_VFORK);
	object_setNumber("CLONE_PARENT", CLONE_PARENT);
	object_setNumber("CLONE_THREAD", CLONE_THREAD);
	object_setNumber("CLONE_NEWNS", CLONE_NEWNS);
	object_setNumber("CLONE_SYSVSEM", CLONE_SYSVSEM);
	object_setNumber("CLONE_SETTLS", CLONE_SETTLS);
	object_setNumber("CLONE_PARENT_SETTID", CLONE_PARENT_SETTID);
	object_setNumber("CLONE_CHILD_CLEARTID", CLONE_CHILD_CLEARTID);
	object_setNumber("CLONE_DETACHED", CLONE_DETACHED);
	object_setNumber("CLONE_UNTRACED", CLONE_UNTRACED);
	object_setNumber("CLONE_CHILD_SETTID", CLONE_CHILD_SETTID);
	object_setNumber("CLONE_NEWCGROUP", CLONE_NEWCGROUP);
	object_setNumber("CLONE_NEWUTS", CLONE_NEWUTS);
	object_setNumber("CLONE_NEWIPC", CLONE_NEWIPC);
	object_setNumber("CLONE_NEWUSER", CLONE_NEWUSER);
	object_setNumber("CLONE_NEWPID", CLONE_NEWPID);
	object_setNumber("CLONE_NEWNET", CLONE_NEWNET);
	object_setNumber("CLONE_IO", CLONE_IO);
}
