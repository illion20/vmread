#include "hlapi/hlapi.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

FILE* dfile;

__attribute__((constructor))
void init()
{
	FILE* out = stdout;
	pid_t pid;
#if (LMODE() == MODE_EXTERNAL())
	FILE* pipe = popen("pidof qemu-system-x86_64", "r");
	fscanf(pipe, "%d", &pid);
	pclose(pipe);
#else
	out = fopen("/tmp/testr.txt", "w");
	pid = getpid();
#endif
	fprintf(out, "Using Mode: %s\n", TOSTRING(LMODE));

	dfile = out;

	try {
		WinContext ctx(pid);
		ctx.processList.Refresh();

		for (auto& i : ctx.processList) {
			fprintf(out, "%s\n", i.proc.name);
			if (!strcmp("csgo.exe", i.proc.name))
				for (auto& o : i.modules) {
					fprintf(out, "\t%s\n", o.info.name);
					if (!strcmp("serverbrowser.dll", o.info.name))
						for (auto& u : o.exports)
							fprintf(out, "\t\t%lx\t%s\n", u.address, u.name);
				}
		}

	} catch (VMException& e) {
		fprintf(out, "Initialization error: %d\n", e.value);
	}
	fclose(out);
}

int main()
{
	return 0;
}
