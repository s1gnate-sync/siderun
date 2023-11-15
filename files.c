#include "jslib.h"

void newStatObject(struct stat buf) {
	newObject();
	object_setNumber("st_dev", buf.st_dev);
	object_setNumber("st_ino", buf.st_ino);
	object_setNumber("st_mode", buf.st_mode);
	object_setNumber("st_nlink", buf.st_nlink);
	object_setNumber("st_uid", buf.st_uid);
	object_setNumber("st_gid", buf.st_gid);
	object_setNumber("st_rdev", buf.st_rdev);
	object_setNumber("st_size", buf.st_size);
	object_setNumber("st_blksize", buf.st_blksize);
	object_setNumber("st_blocks", buf.st_blocks);
	object_setNumber("st_atime", buf.st_atime);
	object_setNumber("st_mtime", buf.st_mtime);
	object_setNumber("st_ctime", buf.st_ctime);
}

void dup_() {
	int fd = dup(toInteger(1));
	if (fd == -1) {
		printLastError("dup");
		returnFalse();
	}	
	
	returnNumber(fd);
}

void dup2_() {
	int fd = dup2(toInteger(1), toInteger(2));
	if (fd == -1) {
		printLastError("dup2");
		returnFalse();
	}	

	returnNumber(fd);
}

void dup3_() {
	int fd = dup3(toInteger(1), toInteger(2), toInteger(3));
	if (fd == -1) {
		printLastError("dup3");
		returnFalse();
	}	
	
	returnNumber(fd);	
}

void access_() {
	if (access(toString(1), toInteger(2))) {
		printLastError("access");
		returnFalse();
	}

	returnTrue();
}

void lstat_() {
	struct stat buf;
	if (stat(toString(1), &buf)) {
		printLastError("lstat");
		returnFalse();
	}
	
	newStatObject(buf);
}

void stat_() {
	struct stat buf;
	if (stat(toString(1), &buf)) {
		printLastError("stat");
		returnFalse();
	}

	newStatObject(buf);
}

void fstat_() {
	struct stat buf;
	if (fstat(toInteger(1), &buf)) {
		printLastError("fstat");
		returnFalse();
	}

	newStatObject(buf);
}

void S_ISDIR_() {
	returnBoolean(S_ISDIR(toInteger(1)));
}

void S_ISCHR_() {
	returnBoolean(S_ISCHR(toInteger(1)));
}

void S_ISBLK_() {
	returnBoolean(S_ISBLK(toInteger(1)));
}

void S_ISREG_() {
	returnBoolean(S_ISREG(toInteger(1)));
}

void S_ISFIFO_() {
	returnBoolean(S_ISFIFO(toInteger(1)));
}

void S_ISLNK_() {
	returnBoolean(S_ISLNK(toInteger(1)));
}

void S_ISSOCK_() {
	returnBoolean(S_ISSOCK(toInteger(1)));
}

void open_() {
	int fd = open(toString(1), toInteger(2), toInteger(3));
	if (fd < 0) {
		printLastError("open");
		returnFalse();
	}
	
	returnNumber(fd);
}

void read_() {
	int bufLen = 1024;
	if (isNumber(2)) {
		bufLen = toInteger(2);
	}
	
	char buf[bufLen + 1];	
	ssize_t len;
	if ((len = read(toInteger(1), &buf[0], sizeof(buf) - 1)) < 0) {
		printLastError("write");
		returnFalse();
	}

	if (len == 0) {
		returnNull();
	}

	buf[len] = '\0';	
	returnString(buf);
}

void opendir_() {
	DIR *dir;
	if ((dir = opendir(toString(1))) == NULL) {
		printLastError("opendir");
		returnFalse();
	}
	
	newUserdataObject("DIR", dir);
}

void rewinddir_() {
	if (!isUserdata(1, "DIR")) {
		printError("readdir", "argument is not a DIR");
		returnFalse();
	}

	rewinddir(toUserdata(1, "DIR"));
	returnUndefined();
}

void readdir_() {
	if (!isUserdata(1, "DIR")) {
		printError("readdir", "argument is not a DIR");
		returnFalse();
	}
	
	struct dirent *ent = readdir(toUserdata(1, "DIR"));
	if (ent == NULL) {
		returnNull();
	}

	newObject();
	object_setNumber("d_ino", ent->d_ino);
	object_setNumber("d_off", ent->d_off);
	object_setNumber("d_reclen", ent->d_reclen);
	object_setNumber("d_type", ent->d_type);
	object_setString("d_name", ent->d_name);
}

void closedir_() {
	if (!isUserdata(1, "DIR")) {
		printError("readdir", "argument is not a DIR");
		returnFalse();
	}

	if (closedir(toUserdata(1, "DIR"))) {
		printLastError("closedir");
		returnFalse();
	}

	returnTrue();
}

void readlink_() {
	char buf[PATH_MAX];
	if (readlink(toString(1), buf, sizeof(buf)) == -1) {
		printLastError("readlink");
		returnFalse();
	}

	returnString(buf);
}

void chmod_() {
	if (chmod(toString(1), toInteger(2))) {
		printLastError("chmod");
		returnFalse();
	}

	returnTrue();
}

void chown_() {
	if (chown(toString(1), toInteger(2), toInteger(3))) {
		printLastError("chown");
		returnFalse();
	}

	returnTrue();	
}

void fallocate_() {
	if (fallocate(toInteger(1), toInteger(2), toInteger(3), toInteger(4))) {
		printLastError("fallocate");
		returnFalse();
	}

	returnTrue();
}

void rename_() {
	if (rename(toString(1), toString(2))) {
		printLastError("rename");
		returnFalse();
	}

	returnTrue();
}

void rmdir_() {
	if (rmdir(toString(1))) {
		printLastError("rmdir");
		returnFalse();
	}

	returnTrue();
}

void close_() {
	if (close(toInteger(1))) {
		printLastError("close");
		returnFalse();
	}
	
	returnTrue();
}

void unlink_() {
	if (unlink(toString(1))) {
		printLastError("unlink");
		returnFalse();
	}	

	returnTrue();
}

void truncate_() {
	if (truncate(toString(1), toInteger(2))) {
		printLastError("truncate");
		returnFalse();
	}

	returnTrue();
}

void write_() {
	const char* buf = toString(2);	
	if (write(toInteger(1), buf, sizeof(buf)) < 0) {
		printLastError("write");
		returnFalse();
	}
	
	returnTrue();
}

void lchmod_() {
	if (lchmod(toString(1), toInteger(2))) {
		printLastError("lchmod");
		returnFalse();
	}

	returnTrue();
}

void lchown_() {
	if (lchown(toString(1), toInteger(2), toInteger(3))) {
		printLastError("lchown");
		returnFalse();
	}

	returnTrue();	
}

void link_() {
	if (link(toString(1), toString(2))) {
		printLastError("link");
		returnFalse();
	}

	returnTrue();
}

void mkdir_() {
	if (mkdir(toString(1), toInteger(2))) {
		printLastError("mkdir");
		returnFalse();
	}

	returnTrue();
}

void mknod_() {
	if (mknod(toString(1), toInteger(2), toInteger(3))) {
		printLastError("mknod");
		returnFalse();
	}

	returnTrue();
}

void symlink_() {
	if (symlink(toString(1), toString(2))) {
		printLastError("symlink");
		returnFalse();
	}

	returnTrue();
}

void fopen_() {
	FILE *file = fopen(toString(1), toString(2));	
	if (file == NULL) {
		printLastError("fopen");
		returnFalse();
	}

	newUserdataObject("FILE", file);
}

void fdopen_() {
	FILE *file = fdopen(toInteger(1), toString(2));
	if (file == NULL) {
		printLastError("fdopen");
		returnFalse();
	}
	
	newUserdataObject("FILE", file);
}

void fclose_() {
	if (isUserdata(1, "FILE")) {
		printError("fclose", "#1 is not a FILE object");
		returnFalse();
	}
	
	FILE *file = toUserdata(1, "FILE");
	if (fclose(file)) {
		printLastError("fclose");
		returnFalse();
	}

	returnTrue();
}

void fwrite_() {
	if (!isUserdata(1, "FILE")) {
		printError("fwrite", "#1 is not a FILE object");
		returnFalse();
	}
	
	const char* buf = toString(2);	
	size_t len = strlen(buf);
	if (fwrite(buf, 1, len, toUserdata(1, "FILE")) < len) {
		printLastError("fwrite");
		returnFalse();
	}

	returnTrue();
}

void fread_() {
	if (!isUserdata(1, "FILE")) {
		printError("fread", "#1 is not a FILE object");
		returnFalse();
	}	

	char buf[512];
	memset(buf, 0, sizeof(buf));

	FILE *file = toUserdata(1, "FILE");
	if (feof(file)) returnNull();
  	if (fread(buf, 1, sizeof(buf), file) != sizeof(buf) && ferror(file)) {
  		printLastError("fread");
  		returnFalse();
   	}

  	returnString(buf);
}

void feof_() {
	if (!isUserdata(1, "FILE")) {
		printError("feof", "#1 is not a FILE object");
		returnFalse();
	}	

	returnBoolean(feof(toUserdata(1, "FILE")));
}

void ferror_() {
	if (!isUserdata(1, "FILE")) {
		printError("ferror", "argument is not a FILE");
		returnFalse();
	}	

	returnBoolean(ferror(toUserdata(1, "FILE")));	
}

void ftell_() {
	if (!isUserdata(1, "FILE")) {
		printError("ferror", "argument is not a FILE");
		returnFalse();
	}	

	long offset = ftell(toUserdata(1, "FILE"));
	if (offset < 0) {
		printLastError("ftell");
		returnFalse();
	}
	
	returnNumber(offset);
}

void fseek_() {
	if (!isUserdata(1, "FILE")) {
		printError("ferror", "argument is not a FILE");
		returnFalse();
	}	

	if (fseek(toUserdata(1, "FILE"), toInteger(2), toInteger(3))) {
		printLastError("fseek");
		returnFalse();
	}
	
	returnTrue();
}

void lseek_() {
	if (lseek(toInteger(1), toInteger(2), toInteger(3))) {
		printLastError("lseek");
		returnFalse();
	}
	
	returnTrue();
}


void rewind_() {             
	if (!isUserdata(1, "FILE")) {
		printError("ferror", "argument is not a FILE");
		returnFalse();
	}	

	rewind(toUserdata(1, "FILE"));
	returnUndefined();
}

void stdin_() {
	newUserdataObject("FILE", stdin);
}

void stdout_() {
	newUserdataObject("FILE", stdout);
}

void stderr_() {
	newUserdataObject("FILE", stderr);
}

void fflush_() {
	int ret;
	if (isNull(1)) {
		ret = fflush(NULL);
	} else {
		if (!isUserdata(1, "FILE")) {
			printError("fflush", "argument is not a FILE");
			returnFalse();
		}	
		ret = fflush(toUserdata(1, "FILE"));
	}

	if (ret == EOF) {
		printLastError("fflush");
		returnFalse();
	}

	returnTrue();
}

void remove_() {
	if (remove(toString(1))) {
		printLastError("remove");
		returnFalse();
	}

	returnTrue();
}

void initFiles() {
	regFunction(remove);
	regFunction(fflush);
	regFunction(access);
	regFunction(open);
	regFunction(read);
	regFunction(opendir);
	regFunction(rewinddir);
	regFunction(readdir);
	regFunction(closedir);
	regFunction(readlink);
	regFunction(chmod);
	regFunction(chown);	
	regFunction(fallocate);
	regFunction(unlink);
	regFunction(rmdir);
	regFunction(rename);
	regFunction(truncate);
	regFunction(write);
	regFunction(close);
	regFunction(lchmod);
	regFunction(lchown);
	regFunction(symlink);
	regFunction(link);
	regFunction(mkdir);
	regFunction(mknod);
	regFunction(stat);
	regFunction(fstat);	
	regFunction(lstat);	
	regFunction(fopen);
	regFunction(fdopen);
	regFunction(fclose);
	regFunction(fwrite);
	regFunction(fread);
	regFunction(feof);
	regFunction(ferror);
	regFunction(ftell);
	regFunction(fseek);
	regFunction(rewind);
	regFunction(dup);
	regFunction(dup2);
	regFunction(dup3);
	regFunction(stdin);
	regFunction(stdout);
	regFunction(stderr);
	regFunction(lseek);

	regFunction(S_ISSOCK);
	regFunction(S_ISLNK);
	regFunction(S_ISFIFO);
	regFunction(S_ISREG);
	regFunction(S_ISBLK);
	regFunction(S_ISCHR);
	regFunction(S_ISDIR);

	object_setNumber("S_IFMT", S_IFMT);
	object_setNumber("S_IFDIR", S_IFDIR);
	object_setNumber("S_IFCHR", S_IFCHR);
	object_setNumber("S_IFBLK", S_IFBLK);
	object_setNumber("S_IFREG", S_IFREG);
	object_setNumber("S_IFIFO", S_IFIFO);
	object_setNumber("S_IFLNK", S_IFLNK);
	object_setNumber("S_IFSOCK", S_IFSOCK)
	object_setNumber("S_ISUID", S_ISUID);
	object_setNumber("S_ISGID", S_ISGID);
	object_setNumber("S_ISVTX", S_ISVTX);
	object_setNumber("S_IRUSR", S_IRUSR);
	object_setNumber("S_IWUSR", S_IWUSR);
	object_setNumber("S_IXUSR", S_IXUSR);
	object_setNumber("S_IRWXU", S_IRWXU);
	object_setNumber("S_IRGRP", S_IRGRP);
	object_setNumber("S_IWGRP", S_IWGRP);
	object_setNumber("S_IXGRP", S_IXGRP);
	object_setNumber("S_IRWXG", S_IRWXG);
	object_setNumber("S_IROTH", S_IROTH);
	object_setNumber("S_IWOTH", S_IWOTH);
	object_setNumber("S_IXOTH", S_IXOTH);
	object_setNumber("S_IRWXO", S_IRWXO);
	object_setNumber("S_IREAD", S_IREAD);
	object_setNumber("S_IWRITE", S_IWRITE);
	object_setNumber("S_IEXEC", S_IEXEC);

	object_setNumber("O_CREAT", O_CREAT);
	object_setNumber("O_EXCL", O_EXCL);
	object_setNumber("O_NOCTTY", O_NOCTTY);
	object_setNumber("O_TRUNC", O_TRUNC);
	object_setNumber("O_APPEND", O_APPEND);
	object_setNumber("O_NONBLOCK", O_NONBLOCK);
	object_setNumber("O_DSYNC", O_DSYNC);
	object_setNumber("O_SYNC", O_SYNC);
	object_setNumber("O_RSYNC", O_RSYNC);
	object_setNumber("O_DIRECTORY", O_DIRECTORY);
	object_setNumber("O_NOFOLLOW", O_NOFOLLOW);
	object_setNumber("O_CLOEXEC", O_CLOEXEC);
	object_setNumber("O_ASYNC", O_ASYNC);
	object_setNumber("O_DIRECT", O_DIRECT);
	object_setNumber("O_LARGEFILE", O_LARGEFILE);
	object_setNumber("O_NOATIME", O_NOATIME);
	object_setNumber("O_PATH", O_PATH);
	object_setNumber("O_TMPFILE", O_TMPFILE);
	object_setNumber("O_NDELAY", O_NDELAY);

	object_setNumber("O_RDONLY", O_RDONLY);
	object_setNumber("O_WRONLY", O_WRONLY);
	object_setNumber("O_RDWR", O_RDWR);

	object_setNumber("STDOUT_FILENO", STDOUT_FILENO);
	object_setNumber("STDERR_FILENO", STDERR_FILENO);
	object_setNumber("STDIN_FILENO", STDIN_FILENO);

	object_setNumber("AT_FDCWD", AT_FDCWD);
	object_setNumber("AT_EMPTY_PATH", AT_EMPTY_PATH);
	object_setNumber("AT_NO_AUTOMOUNT", AT_NO_AUTOMOUNT);
	object_setNumber("AT_SYMLINK_NOFOLLOW", AT_SYMLINK_NOFOLLOW);

	global_setNumber("STDIN_FILENO", STDIN_FILENO);
	global_setNumber("STDOUT_FILENO", STDOUT_FILENO);
	global_setNumber("STDERR_FILENO", STDERR_FILENO);

	object_setNumber("SEEK_SET", SEEK_SET);
	object_setNumber("SEEK_CUR", SEEK_CUR);
	object_setNumber("SEEK_END", SEEK_END);
	object_setNumber("SEEK_DATA", SEEK_DATA);
	object_setNumber("SEEK_HOLE", SEEK_HOLE);
	
	object_setNumber("F_OK", F_OK);
	object_setNumber("R_OK", R_OK);
	object_setNumber("W_OK", W_OK);
	object_setNumber("X_OK", X_OK);
	object_setNumber("F_ULOCK", F_ULOCK);
	object_setNumber("F_LOCK ", F_LOCK);
	object_setNumber("F_TLOCK", F_TLOCK);
	object_setNumber("F_TEST ", F_TEST);	
}
