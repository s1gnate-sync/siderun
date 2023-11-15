// --

global.define("global", "uid", system.getuid());
global.define("global", "gid", system.getgid());
global.define("global", "pid", system.getpid());

(function() {
	var exePath = system.readlink("/proc/self/exe");
	if (exePath === false) {
		printError("can't read /proc/self/exe");
		exit(1);
	}

	var exeDir = exePath.split('/');
	exeDir.pop();
	global.define("global", "exeDir", exeDir.join('/'));
	global.define("global", "exePath", exePath);
})();

// --

global.define("global", "parseArguments", function(index) {
	var opts = {};
	for (var i = index ? index : 1; i < argv.length; i++) {
		var arg = argv[i];

		if (arg === "--") {
			i++;
			break;
		} else if (arg[0] !== "-") {
			break;
		} else {
			if (!(arg in opts)) {
				opts[arg] = [];
			}

			var nextArg = argv[i + 1];
			if (nextArg !== undefined && nextArg[0] !== "-") {
				i++;
				if (parseFloat(nextArg).toString() === nextArg) {
					opts[arg].push(+nextArg);
				} else {
					opts[arg].push(nextArg);	
				}
			} else {
				opts[arg].push(true);
			}
		}
	}
	
	var args = argv.slice(i);
	args.unshift(argv[0]);

	return Object.create({
		get: function(key, defaultValue) {
			if (typeof key === "number") {
				return this.args[key] || defaultValue;
			}

			var value = this.opts[key.length > 1 ? "--" + key : "-" + key];
			return value ? value[0] : defaultValue;
		},
		list: function(key) {
			if (typeof key === "number") {
				return this.args.slice(key);
			}

			return this.opts[key.length > 1 ? "--" + key : "-" + key] || [];
		}
	}, {
		opts: {value: opts, enumerable: false, writable: false},
		args: {value: args, enumerable: false, writable: false}
	});
});

// --

global.define("stat", null, function (path) {
	return system.stat(path) !== false;	
});

global.define("stat", "isFile", function (path) {
	var info;
	return (info = system.lstat(path)) ? system.S_ISREG(info.st_mode) : false;	
});

global.define("stat", "isDir", function (path) {
	var info;
	return (info = system.lstat(path)) ? system.S_ISDIR(info.st_mode) : false;	
});

global.define("stat", "isLink", function (path) {
	var info;
	return (info = system.lstat(path)) ? system.S_ISLINK(info.st_mode) : false;	
});

// --

global.define("exec", null, function (name, args, options) {
	if (!options) options = {};
	
	args = [name.split("/").pop()].concat(args instanceof Array ? args : []);
	
	args = args ? args.map(function (item) {
		return item.toString();
	}) : [];
	
	var inPipe, outPipe, errPipe;
	var openFlags = system.O_CLOEXEC | system.O_CREAT;
	var openMode = system.S_IWUSR | system.S_IRUSR | system.S_IRGRP | system.S_IWGRP | system.S_IROTH;
	var pid = -1;
	var status = exec.RUNNING;
	var statusValue;
	var output = "";
	var outputErr = "";

	var instance = {};

	Object.defineProperty(instance, "name", {
		enumerable: true,
		get: function () {
			return name;
		}	
	});

	Object.defineProperty(instance, "args", {
		enumerable: true,
		get: function () {
			return args.concat([]);
		}	
	});

	Object.defineProperty(instance, "wait", {
		value: function () {
			if (status !== exec.RUNNING) {
				printError("can't wait for exec in incorrect status");
				return null;
			}
			
			status = system.waitpid(pid, 0);
			if (status === false) {
				status = exec.EXECERROR;
			} else if (system.WIFEXITED(status)) {
				statusValue = system.WEXITSTATUS(status);
				status = exec.EXITED;
			} else if (system.WIFSIGNALED(status)) {
				statusValue = system.WTERMSIG(status);
				status = exec.SIGNALED;
			} else if (system.WCOREDUMP(status)) {
				status = exec.COREDUMP;
			} else if (system.WIFCONTINUED(status)) {
				status = exec.CONTINUED;
			} else if (status.WIFSTOPPED(status)) {
				statusValue = system.WSTOPSIG(status);
				status = exec.STOPPED;
			}

			return instance.error;
		}
	});

	Object.defineProperty(instance, "output", {
		enumerable: true,
		get: function () {
			return output;
		}
	});

	Object.defineProperty(instance, "outputError", {
		enumerable: true,
		get: function () {
			return outputError;
		}
	});		

	Object.defineProperty(instance, "pid", {
		enumerable: true,
		get: function () {
			return pid;
		}
	});
	
	Object.defineProperty(instance, "status", {
		enumerable: true,
		get: function () {
			return status;
		}
	});

	Object.defineProperty(instance, "error", {
		enumerable: true,
		get: function () {
			return status === exec.EXITED && statusValue !== 0;
		}
	})

	Object.defineProperty(instance, "exitStatus", {
		enumerable: true,
		get: function () {
			return status === exec.EXITED ? statusValue : undefined;
		}
	});

	Object.defineProperty(instance, "terminationSignal", {
		enumerable: true,
		get: function () {
			return status === exec.SIGNALED ? statusValue : undefined;
		}
	});

	Object.defineProperty(instance, "stopSignal", {
		enumerable: true,
		get: function () {
			return status === exec.STOPPED ? statusValue : undefined;
		}
	});		

	Object.defineProperty(instance, "write", {
		value: function(data) {
			if (!inPipe) {
				return null;
			}

			if (!(data instanceof Array)) {
				data = [data];
			}

			for (var i = 0; i < data.length; i++) {
				if (data[i] === null) {
					system.close(inPipe[1]);
					inPipe = undefined;
					return null;
				}
				
				if (!system.write(inPipe[1], data[i])) {
					return false;
				}
			}
		}
	});		

	Object.defineProperty(instance, "read", {
		value: function (size) {
			if (!outPipe) {
				return null;
			}		

			var buf = system.read(outPipe[0], size ? size : 512);
			if (buf === false) {
				return null;
			} else if (buf === null) {
				system.close(outPipe[0]);						
				outPipe = undefined;
			}
			
			return buf;
		}
	});

	Object.defineProperty(instance, "readError", {
		value: function(size) {
			if (!errPipe) {
				return null;
			}		
			
			var buf = system.read(errPipe[0], size ? size : 512);
			if (buf === false) {
				return null;
			} else if (buf === null) {
				system.close(errPipe[0]);						
				errPipe = undefined;
			}
			
			return buf;
		}
	});

	Object.defineProperty(instance, "kill", {
		value: function(signal) {
			if (status !== exec.RUNNING) {
				return;
			}
			
			return kill(pid, signal ? signal : system.SIGTERM);
		}
	});

	if (options.argv0) {
		args[0] = options.argv[0];
	}
	
	if (!(pid === -1 && status === exec.RUNNING)) {
		return true;
	}
	
	var env = options.env ? Object.keys(options.env).map(function (key) {
		return key + "=" + options[key].toString();
	}) : null;	// if (!options) {

	var stdin;
	if (options.stdin !== undefined) {
		stdin = options.stdin;
	} else if (options.write) {
		stdin = (options.write instanceof Array) ? options.write.concat(null) : [options.write, null];
	}
		
	var stdout = options.stdout;
	if (stdout === null) {
		stdout = "/dev/null";
	}
	
	var stderr = options.stderr;
	if (stderr === null) {
		stderr = "/dev/null";
	}

	if ((stdin instanceof Array || stdin === true || stdin === null) && !(inPipe = system.pipe())) {
		printError("failed to create inPipe");
		return true;
	}

	if (stdout === true && !(outPipe = system.pipe())) {
		printError("failed to create outPipe");
		return true;
	}
	
	if (stderr === true && !(errPipe = system.pipe())) {
		printError("failed to create errPipe");
		return true;
	}

	pid = system.fork();	
	if (pid === false) {
		printError("failed to fork");
		return true;
	}

	if (pid) {
		/* parent exec */

		if (inPipe) {
			system.close(inPipe[0]);
			if (stdin === null) {
				system.close(inPipe[1]);
				inPipe = undefined;
			} else if (stdin instanceof Array) {
				instance.write(stdin);
			}
		}
		
		if (outPipe) {
			system.close(outPipe[1]);
		}
		if (errPipe) {
			system.close(errPipe[1]);
		}

		if (!options.async) {
			var bufOut = true, bufErr = true;
			while (bufOut || bufErr) {
				if (bufOut && (bufOut = instance.read()) !== null) {
					output += bufOut;
				}
				
				if (bufErr && (bufErr = instance.readError()) !== null) {
					outputError += bufErr;
				}
			}
			
			instance.wait();
		}

		return instance;
	}

	if (inPipe) {					
		system.close(inPipe[1]);
		system.dup2(inPipe[0], system.STDIN_FILENO);
		system.close(inPipe[0]);
	} else if (typeof stdin === "number") {
		system.dup2(stdin, system.STDIN_FILENO);
	} else if (typeof stin === "string") {
		var fdIn = system.open(stdout, openFlags | system.O_RDONLY, openMode);
		if (fdIn === false) {
			printError("failed to open file for stdin");
			return null;
		}
		
		system.dup2(fdIn, system.STDIN_FILENO);
	}
	
	if (outPipe) {					
		system.close(outPipe[0]);
		system.dup2(outPipe[1], system.STDOUT_FILENO);
		system.close(outPipe[1]);
	} else if (typeof stdout === "number") {
		system.dup2(stdout, system.STDOUT_FILENO);
	} else if (typeof stdout === "string") {
		var fdOut = system.open(stdout, openFlags | system.O_WRONLY | system.O_TRUNC, openMode);
		if (fdOut === false) {
			printError("failed to open file for stdout");
			return true;
		}
		
		system.dup2(fdOut, system.STDOUT_FILENO);
	}
	
	if (errPipe) {
		system.close(errPipe[0]);
		system.dup2(errPipe[1], system.STDERR_FILENO);
		system.close(errPipe[1]);
	} else if (typeof stderr === "number") {
		system.dup2(stderr, system.STDERR_FILENO);
	} else if (typeof stderr === "string") {
		var fdErr = system.open(stderr, openFlags | system.O_WRONLY | system.O_TRUNC, openMode);
		if (fdErr === false) {
			printError("failed to open file for stderr");
			return true;
		}
		
		system.dup2(fdErr, system.STDERR_FILENO);
	}

	if (options.chdir) {
		system.chdir(options.chdir);
	}
	
	if (name.indexOf('/') > -1) {
		if (env) {
			system.execve(name, args, env);
		} else {
			system.execv(name, args, env);
		}
	} else {
		if (env) {
			system.execvpe(name, args);
		} else {
			system.execvp(name, args);	
		}
	}

	exit(1);
});

global.define("exec", "EXECERROR", -2);
global.define("exec", "RUNNING", -1);
global.define("exec", "EXITED", 0);
global.define("exec", "SIGNALED", 1);
global.define("exec", "STOPPED", 2);
global.define("exec", "CONTINUED", 3);
global.define("exec", "COREDUMP", 4);

// --

global.define("user", null, {});
global.define("user", "isRoot", function() {
	return system.getuid() === 0;
});
global.define("user", "change", function(uid, gid) {
	if (typeof gid === undefined) {
		gid = global.gid;
	}

	if (uid === 0) {
		return system.setuid(uid) && system.setgid(gid);
	}

	return system.setgid(gid) && system.setuid(uid);
});

// --

global.define("namespace", null, {});
global.define("namespace", "MOUNT", system.CLONE_NEWNS);
global.define("namespace", "NETWORK", system.CLONE_NEWNET);
global.define("namespace", "unshare", function(flags) {
	return system.unshare(
		system.CLONE_THREAD | flags.reduce(function(ret, item) {
			return ret | item;
		}, 0)
	);
});

global.define("template", null, function(string, data) {
	if (data === undefined) {
		data = {};
	}
	
	return string.split("${").reduce(function(ret, item, index) {
		if ((index + 1) % 2 === 0) {
			var nameEndIndex = item.indexOf("}");
			var name = item.substring(0, nameEndIndex);

			var value = data[name];
			if (value === undefined) {
				value = template.global[name];
			}
			
			ret.push(value);
			ret.push(item.substring(nameEndIndex + 1));
		} else {
			ret.push(item);
		}
		
		return ret;
	}, []).join("");
});

global.define("template", "global", {});

global.define("io", null, {});
global.define("io", "readFile", function(filename){
	var file = system.open(stdout, system.O_RDONLY, 0);
	if (file === false) {
		return null;
	}
	
	var contents = [];
	var buf;
	while ((buf = system.read(file)) !== null) {
		contents.push(buf);	
	}	
	
	return buf.join('');
});

// --
