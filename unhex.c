#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#define ZEROS 1
#define HELP 2

size_t bufsize = 1;
unsigned char conf = 0;

int changeBufSize(int argi, int argc, char ** argv) {
	if(argi >= argc) {
		fprintf(stderr, "Unexpected end of argument list");
		return -1;
	}
	char * bs = argv[argi];
	int s;
	if(sscanf(bs, "%d", &s) != 1) {
		fprintf(stderr, "Reverse buffer size should be integer");
		return -1;
	}
	if(s < 1) {
		fprintf(stderr, "Reverse buffer size cannot be less than 1");
		return -1;
	}
	bufsize = (size_t)s;
	return 0;
}

int main(int argc, char ** argv) {
	int argi, retval;
	for(argi = 1; argi < argc; argi++) {
		char * arg = argv[argi];
		if(arg[0] == '-') {
			if(arg[1] == '-') {
				arg += 2;
				if(!strcmp(arg, "fill-zeros")) {
					conf |= ZEROS;
				} else if(!strcmp(arg, "help")) {
					conf |= HELP;
				} else if(!strcmp(arg, "rev-buffer")) {
					retval = changeBufSize(++argi, argc, argv);
					if(retval) return retval;
				} else {
					fprintf(stderr, "Unknown argument: \"%s\"\n", arg - 2);
					return -1;
				}
			} else {
				while(arg[1] != 0) {
					++arg;
					switch(*arg) {
						case 'h': conf |= HELP; break;
						case 'z': conf |= ZEROS; break;
						case 'r': 
							retval = changeBufSize(++argi, argc, argv);
							if(retval) return retval;
							break;
						default: fprintf(stderr, "Unknown argument: \"-%c\"\n", *arg);
							 return -1;
					}
				}
			}
		} else {
			fprintf(stderr, "Argument %i: key  expected", argi);
			return -1;
		}
	}
	if(conf & HELP) {
		printf("Usage: %s [OPTIONS]\n\
  -h, --help     display this help and exit\n\
  -r <size>, --rev-buffer <size>\n\
                 save <size> of bytes to buffer and reverse it before output\n\
  -z, --fill-zeros\n\
                 fill the last buffer with zeros (otherwise do not output it)\n\
", argv[0]);
		return 0;
	} else {
		char * buf = (char*) malloc(bufsize);
		int i, j, c[2];
		char loop = 1;
		while(loop) {
			for(i = 0; i < bufsize; i++) {
				for(j = 0; j < 2; j++) {
					while(1) {
						c[j] = getchar();
						if(c[j] == EOF) {
							c[j] = '0';
							loop = 0;
							if(!(conf & ZEROS)) break;
						}
						if(c[j] >= '0' && c[j] <= '9') {
							c[j] -= '0';
						} else if(c[j] >= 'a' && c[j] <= 'f') {
							c[j] -= 'a' - 10;
						} else if(c[j] >= 'A' && c[j] <= 'F') {
							c[j] -= 'A' - 10;
						} else continue;
						break;
					}
				}
				buf[i] = (char) (c[0] * 16 + c[1]);
			}
			if(loop || (conf & ZEROS)) for(i = bufsize - 1; i >= 0; i--) {
				putchar(buf[i]);
			}
		}
		return 0;
	}
}
