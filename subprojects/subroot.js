if (typeof embed !== "undefined") {
		evalFile("./library.js");	
}

var options = (function() {
	var args = global.parseArguments(1);
	if (args.get('help', args.get('h'))) {
		return {
			isValid: true,
			showHelp: true,
		};
	}
	
	var entrypoint = args.list(1);
	var keepEnvVars = args.list('keep-env').concat(args.list('E'));
	var rootDir = args.get('root-dir', args.get('r', system.getcwd()));
	var bindMounts = [];
	var mounts = [
		["proc", "/proc", "proc", 0, null],
		["none", "/sys", "sysfs", 0, null]
	];

	if (!args.get('no-dev')) {
		mounts.push(["devtmpfs", "/dev", "devtmpfs", 0, null]);
		mounts.push(["devpts", "/dev/pts", "devpts", 0, null])
	}

	args.list('tmp').concat(args.list('t')).forEach(function(item) {
		var parts = item.split(',');
		mounts.push(["none", parts[0], "tmpfs", 0, parts[1] ? parts[1] : null]);
	});

	args.list('mount').concat(args.list('m')).forEach(function (item) {
		var parts = item.split(',');
		mounts.push([parts[0], parts[1], parts[2], 0, parts[3] ? parts[3] : null]);	
	});

	args.list('bind').concat(args.list('b')).forEach(function (item) {
		var parts = item.split(',');
		bindMounts.push([parts[0], rootDir + parts[1], "bind", system.MS_BIND | system.MS_PRIVATE, null]);
		if (parts[2]) {			
			bindMounts.push(["", rootDir + parts[1], "", system.MS_BIND | system.MS_REMOUNT | system.MS_RDONLY, null]);
		}
	});

	var cwdTarget = args.get("bind-work-dir", args.get("w"));
	if (cwdTarget) {
		bindMounts.push([system.getcwd(), rootDir + cwdTarget, "bind", system.MS_BIND | system.MS_PRIVATE, null]);
	}
	
	return {
		isValid: entrypoint.length > 0,
		rootDir: rootDir,
		workDir: args.get('work-dir', args.get('d', '/')),
		uid: args.get('uid', args.get('u', global.uid)),
		gid: args.get('gid', args.get('g', global.gid)),
		envVars: args.list('env').concat(args.list('e')).map(function (kv) {
			var pos = kv.indexOf("=");	
			return [kv.substring(0, pos), kv.substring(pos + 1, kv.length)];
		}),
		keepEnvVars: keepEnvVars,
		clearEnv: keepEnvVars.length > 0,
		mounts: mounts,
		bindMounts: bindMounts,
		entrypoint: entrypoint,
		debug: system.getenv('DEBUG'),
	};
})();

if (options.debug) {
	print("Options: ", JSON.stringify(options), "\n");
} else {
	verbose(0);
}

if (!options.isValid) {
	printError(argv[0] + ": missing entry point\n");
	exit(1);
}

if (options.showHelp) {
	printError(argv[0] + ": [options] -- [<entrypoint> [<entrypoint-argument>...]]\n");
	printError("-r, --root-dir <dir>	set root directory (default is " + system.getcwd() + ")\n");
	printError("-d, --work-dir <dir>	set working directory (default is /)\n");
	printError("-u, --uid <id> set user id  (default is " + global.uid + ")\n");
	printError("-g, --gid <id> set group id (default is " + global.gid + ")\n");
	printError("-e, --env <key>=<value>	set environment variable\n");
	printError("-E, --keep-env <key>	only keep specific environment variable (default is no-op)\n");
	printError("-b, --bind <src>,<dest>,<ro>	bind <src> to <dest>\n");
	printError("-w, --bind-work-dir <dest> share current directory to <dest>\n");
	printError("-m, --mount <src>,<dest>,<type>[,<data>]	mount filesystem specified by <src> to <dest>, <data> depends on <type>\n");
	printError("-t, --tmp <dest>[,<data>]	mount tmpfs to <dest>\n");
	printError("	--no-dev do not mount devtmpfs and devpts filesystems\n");
	printError("\n");
	exit(0);
}

function exitFatal(msg) {
	printError(msg + "\n");
	exit(2);
} 

if (!namespace.unshare([namespace.MOUNT])) {
	exitFatal("Failed to create mount namespace");
}

if (!user.change(0, 0)) {
	exitFatal("Can't gain root permissions");
}

options.bindMounts.forEach(function (args, id) {
	if (!system.mount.apply(undefined, args)) {
		exitFatal("Bind failed for entry #" + id + " (" + args + ")");
	}
});

if (!system.chroot(options.rootDir)) {
	exitFatal("Can't change root to " + options.rootDir);
}

system.chdir("/");

options.mounts.forEach(function (args, id) {
	if (!system.mount.apply(undefined, args)) {
		exitFatal("Mount failed for entry #" + id + " (" + args + ")");
	}
});

if (!system.setgid(options.gid)) {
	exitFatal("Failed to change group id # " + options.gid);
}

if (!system.setuid(options.uid)) {
	exitFatal("Failed to change user id " + options.uid);
}

if (options.clearEnv) {
	var copyEnvVars = options.keepEnvVars.map(function (name) {
		var value = system.getenv(name);
		if (value) {
			return [name, value];
		}
	});
	
	system.clearenv();

	copyEnvVars.concat(options.envVars).forEach(function (kv) {
		if (kv) {
			system.setenv(kv[0], kv[1], 1);		
		}
	});
} else {
	options.envVars.forEach(function (kv) {
		system.setenv(kv[0], kv[1], 1);
	});
}

system.chdir(options.workDir);

if (!system.execvp(options.entrypoint[0], options.entrypoint)) {
	exitFatal("entrypointed execution was unsuccessful");	
}

// end
