/*
 * elf.c - A simple package for manipulating symbol tables in elf binaries.
 */

#include    "../include/elfsym.h"
/* 
 * elf_open - Map a binary into the address space and extract the
 * locations of the static and dynamic symbol tables and their string
 * tables. Return this information in a Elf object file handle that will
 * be passed to all of the other elf functions.
 */
Elf_obj eobj;
Elf_obj *elf_open(uintptr_t addr) 
{
    int i, fd;
    //struct stat sbuf;
    memset((void*)&eobj, 0, sizeof(Elf_obj));
    Elf_obj *ep = &eobj;
    Elf32_Shdr *shdr;

    
    /* It looks OK, so map the Elf binary into our address space */
    //ep->mlen = sbuf.st_size;
    ep->maddr = (void*)addr;

    /* The Elf binary begins with the Elf header */
    ep->ehdr = ep->maddr;

    /* 
     * Find the static and dynamic symbol tables and their string
     * tables in the the mapped binary. The sh_link field in symbol
     * table section headers gives the section index of the string
     * table for that symbol table.
     */
    shdr = (Elf32_Shdr *)(ep->maddr + ep->ehdr->e_shoff);
    for (i = 0; i < ep->ehdr->e_shnum; i++) {
	if (shdr[i].sh_type == SHT_SYMTAB) {   /* Static symbol table */
	    ep->symtab = (Elf32_Sym *)(ep->maddr + shdr[i].sh_offset);
	    ep->symtab_end = (Elf32_Sym *)((char *)ep->symtab + shdr[i].sh_size);
	    ep->strtab = (char *)(ep->maddr + shdr[shdr[i].sh_link].sh_offset);
	}
	if (shdr[i].sh_type == SHT_DYNSYM) {   /* Dynamic symbol table */
	    ep->dsymtab = (Elf32_Sym *)(ep->maddr + shdr[i].sh_offset);
	    ep->dsymtab_end = (Elf32_Sym *)((char *)ep->dsymtab + shdr[i].sh_size);
	    ep->dstrtab = (char *)(ep->maddr + shdr[shdr[i].sh_link].sh_offset);
	}
    }
    return ep;
}


/*
 * elf_symname - Return ASCII name of a static symbol
 */
char *elf_symname(Elf_obj *ep, Elf32_Sym *sym)
{
    return &ep->strtab[sym->st_name];
}

/*
 * elf_dsymname - Return ASCII name of a dynamic symbol
 */ 
char *elf_dsymname(Elf_obj *ep, Elf32_Sym *sym)
{
    return &ep->dstrtab[sym->st_name];
}

/*
 * elf_firstsym - Return ptr to first symbol in static symbol table
 */
Elf32_Sym *elf_firstsym(Elf_obj *ep)
{
    return ep->symtab;
}

/*
 * elf_nextsym - Return ptr to next symbol in static symbol table,
 * or NULL if no more symbols.
 */
Elf32_Sym *elf_nextsym(Elf_obj *ep, Elf32_Sym *sym)
{
    sym++;
    if (sym < ep->symtab_end)
	return sym;
    else
	return NULL;
}

/*
 * elf_firstdsym - Return ptr to first symbol in dynamic symbol table
 */
Elf32_Sym *elf_firstdsym(Elf_obj *ep)
{
    return ep->dsymtab;
}

/*
 * elf_nextdsym - Return ptr to next symbol in dynamic symbol table,
 * of NULL if no more symbols.
 */ 
Elf32_Sym *elf_nextdsym(Elf_obj *ep, Elf32_Sym *sym)
{
    sym++;
    if (sym < ep->dsymtab_end)
	return sym;
    else
	return NULL;
}

/*
 * elf_isfunc - Return true if symbol is a static function
 */
int elf_isfunc(Elf_obj *ep, Elf32_Sym *sym) 
{
    return ((ELF32_ST_TYPE(sym->st_info) == STT_FUNC) &&
	    (sym->st_shndx != SHT_NULL));
}

/*
 * elf_isdfunc - Return true if symbol is a dynamic function 
 */
int elf_isdfunc(Elf_obj *ep, Elf32_Sym *sym) 
{
    return ((ELF32_ST_TYPE(sym->st_info) == STT_FUNC));
}
