#ifndef __LEVOS_PROCESS_H
#define __LEVOS_PROCESS_H

typedef signed int pid_t;

struct process {
	pid_t pid;
	char *comm; /* the name of the process */
	
};

#endif /* __LEVOS_PROCESS_H */
