#include "siderun.h"

SectionInfo* getSectionInfo(char *targetpath, char *section_name) {
  Elf32_Shdr *    shdr32;
  Elf32_Ehdr *    ehdr32;
  Elf64_Shdr *    shdr64;
  Elf64_Ehdr *    ehdr64;

  Elf *    elf;
  Elf_Scn *    scn;
  Elf_Data *    data;

  int  fd;
  unsigned int    cnt;
  if ((fd = open(targetpath, O_RDONLY)) == -1) {
    perror("open error");
    return NULL;
  }

  (void) elf_version(EV_CURRENT);
  if ((elf = elf_begin(fd, ELF_C_READ, NULL)) == NULL) {
    perror("elf_begin error");
    return NULL;
  }

  int i = gelf_getclass(elf);
  if (i == ELFCLASS32) {
    ehdr32 = elf32_getehdr(elf);
    scn = elf_getscn(elf, ehdr32->e_shstrndx);
    data = elf_getdata(scn, NULL);

    if (ehdr32 == NULL || scn == NULL || data == NULL) {
      return NULL;
    }
  } else if (i == ELFCLASS64) {
    ehdr64 = elf64_getehdr(elf);
    scn = elf_getscn(elf, ehdr64->e_shstrndx);
    data = elf_getdata(scn, NULL);

    if (ehdr64 == NULL || scn == NULL || data == NULL) {
      return NULL;
    }
  }

  scn = NULL;
  cnt = 0;
  while ((scn = elf_nextscn(elf, scn)) != 0) {
    if (i == ELFCLASS32) {
      if ((shdr32 = elf32_getshdr(scn)) == NULL) {
        return NULL;
      }

      char *name = (char *)data->d_buf + shdr32->sh_name;

      if (strcmp(name, section_name) == 0) {
        SectionInfo *x;
        x = (SectionInfo*) malloc(sizeof(SectionInfo));
        x->offset = shdr32->sh_offset;
        x->size = shdr32->sh_size;
        return x;
      }


    } else if (i == ELFCLASS64) {
      if ((shdr64 = elf64_getshdr(scn)) == NULL) {
        return NULL;
      }

      char *name = (char *) data->d_buf + shdr64->sh_name;

      if (strcmp(name, section_name) == 0) {
        SectionInfo *x;
        x = (SectionInfo*) malloc(sizeof(SectionInfo));
        x->offset = shdr64->sh_offset;
        x->size = shdr64->sh_size;
        return x;
      }
      
    }

    cnt+=1;
  }
}

char* getCurrentPath() {
  char buffer[PATH_MAX];

  if (readlink("/proc/self/exe", buffer, PATH_MAX) == -1) {
    return NULL;
  }
  
  return strdup(buffer);
}
