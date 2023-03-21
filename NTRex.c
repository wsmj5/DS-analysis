//include

#include "std.h"
#include "stdchara.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

//typedef

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

//struct

//variables

void *SpiritTracks;
char odir[5] = {0};
u32 fpos = 0;
char f[0x80] = {0};
u16 flen = 5;
u16 find = 0;
char cmd[4096] = {0};
u16 dirpath[1 << 16] = {0xF000, 0};
u16 dirdep = 0;

//functions

int main(int argc, char **argv){
	
	int fd = open(argv[1], O_RDONLY);
	struct stat statbuf;
	if(fstat(fd, &statbuf)) return 1;
	SpiritTracks = mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	memcpy(odir, SpiritTracks + 0xC, 4);
	memcpy(f, SpiritTracks + 0xC, 4); f[4] = '/';
	mkdir(odir, 0777);
	memcpy(cmd + 3, odir, 5);
	odir[4] = '/';
	u32 FNT = *(u32*)(SpiritTracks + 0x40);
	u32 FAT = *(u32*)(SpiritTracks + 0x48);
	
	fpos = FNT + *(u32*)(SpiritTracks + FNT);
	find = *(u16*)(SpiritTracks + FNT + 4);
	
	while(~dirdep)
	{
		while(*(u8*)(SpiritTracks + fpos))
		{
			
			memcpy(f + flen, (u8*)SpiritTracks + fpos + 1, *(u8*)(SpiritTracks + fpos) & 0x7F);
			if(*(u8*)(SpiritTracks + fpos) & 0x80)
			{
				if(!mkdir(f, 0777))
				{
					dirpath[++dirdep] = *(u16*)(SpiritTracks + fpos + 1 + (*(u8*)(SpiritTracks + fpos) & 0x7F));
					flen += *(u8*)(SpiritTracks + fpos) & 0x7F; f[flen++] = '/';
					find = *(u16*)(SpiritTracks + FNT + (dirpath[dirdep] ^ 0xF000) * 8 + 4);
					fpos = FNT + *(u32*)(SpiritTracks + FNT + (dirpath[dirdep] ^ 0xF000) * 8);
				}
				else fpos += 3 + *(u8*)(SpiritTracks + fpos) & 0x7F;
				
			}else{
				int ofile = open(f, "wb");
				printf("%04X %s\n", find, f);
				write(ofile, (SpiritTracks + *(u32*)(SpiritTracks + FAT + find * 8)), *(u32*)(SpiritTracks + FAT + find * 8 + 4) - *(u32*)(SpiritTracks + FAT + find * 8));
				close(ofile);
				find++;
				fpos  += 1 + *(u8*)(SpiritTracks + fpos) & 0x7F;
			}
			
			{char i = flen;
				while(f[i]) f[i++] = 0;}
		}
		fpos = FNT + *(u32*)(SpiritTracks + FNT + (dirpath[--dirdep] ^ 0xF000) * 8);
		find = *(u16*)(SpiritTracks + FNT + (dirpath[dirdep] ^ 0xF000) * 8 + 4);
		do{
			f[flen--] = 0;
		}while(f[flen - 1] != '/');
		f[flen] = 0;
	}
	
	return 0;}
