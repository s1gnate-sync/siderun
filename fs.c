#include "jslib.h"

void chroot_() {
	if (chroot(toString(1))) {
		printLastError("chroot");
		returnFalse();
	}

	returnTrue();
}

void sync_() {
	sync();
	returnTrue();
}

void mount_() {
	void *data = isNull(5) ? NULL : (char *) toString(5);
	if (mount(toString(1), toString(2), toString(3), toInteger(4), data)) {
		printLastError("mount");
		returnFalse();
	}
	
	returnTrue();
}

void umount_() {
	if (umount(toString(1))) {
		printLastError("umount");
		returnFalse();
	}
	
	returnTrue();
}

void umount2_() {
	if (umount2(toString(1), toInteger(2))) {
		printLastError("umount2");
		returnFalse();
	}
	
	returnTrue();
}

void inotify_add_watch_() {
	// TODO
}

void inotify_init_() {
	// TODO
}

void inotify_init1_() {
	// TODO
}

void inotify_rm_watch_() {
	// TODO
}

void initFs() {
	regFunction(chroot);
	regFunction(sync);
	regFunction(umount);
	regFunction(umount2);
	regFunction(mount);
	regFunction(inotify_add_watch);
	regFunction(inotify_init);
	regFunction(inotify_init1);
	regFunction(inotify_rm_watch);

	object_setNumber("MS_RDONLY", MS_RDONLY);
	object_setNumber("MS_NOSUID", MS_NOSUID);
	object_setNumber("MS_NODEV", MS_NODEV);
	object_setNumber("MS_NOEXEC", MS_NOEXEC);
	object_setNumber("MS_SYNCHRONOUS", MS_SYNCHRONOUS);
	object_setNumber("MS_REMOUNT", MS_REMOUNT);
	object_setNumber("MS_MANDLOCK", MS_MANDLOCK);
	object_setNumber("MS_DIRSYNC", MS_DIRSYNC);
	object_setNumber("MS_NOSYMFOLLOW", MS_NOSYMFOLLOW);
	object_setNumber("MS_NOATIME", MS_NOATIME);
	object_setNumber("MS_NODIRATIME", MS_NODIRATIME);
	object_setNumber("MS_BIND", MS_BIND);
	object_setNumber("MS_MOVE", MS_MOVE);
	object_setNumber("MS_REC", MS_REC);
	object_setNumber("MS_SILENT", MS_SILENT);
	object_setNumber("MS_POSIXACL", MS_POSIXACL);
	object_setNumber("MS_UNBINDABLE", MS_UNBINDABLE);
	object_setNumber("MS_PRIVATE", MS_PRIVATE);
	object_setNumber("MS_SLAVE", MS_SLAVE);
	object_setNumber("MS_SHARED", MS_SHARED);
	object_setNumber("MS_RELATIME", MS_RELATIME);
	object_setNumber("MS_KERNMOUNT", MS_KERNMOUNT);
	object_setNumber("MS_I_VERSION", MS_I_VERSION);
	object_setNumber("MS_STRICTATIME", MS_STRICTATIME);
	object_setNumber("MS_LAZYTIME", MS_LAZYTIME);
	object_setNumber("MS_NOREMOTELOCK", MS_NOREMOTELOCK);
	object_setNumber("MS_NOSEC", MS_NOSEC);
	object_setNumber("MS_BORN", MS_BORN);
	object_setNumber("MS_ACTIVE", MS_ACTIVE);
	object_setNumber("MS_NOUSER", MS_NOUSER);
}
