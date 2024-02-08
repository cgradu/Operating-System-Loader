	/*
	* Loader Implementation
	*
	* 2022, Operating Systems
	*/

	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>

	#include <signal.h>
	#include <sys/mman.h>
	#include <fcntl.h>
	#include <sys/stat.h>
	#include <unistd.h>

	#include "exec_parser.h"

	static struct sigaction old_action;
	static int fd;
	static so_exec_t *exec;

	void segv_handler(int signum, siginfo_t *info, void *context)
	{	
		/* TODO - actual loader implementation */
		//daca semnalul nu este segmentation fault -> default handler
		if (signum != SIGSEGV) {
			old_action.sa_sigaction(signum, info, context);
			return;
		}
		//daca nu exista info despre semnal -> default handler
		if (info == NULL){
			old_action.sa_sigaction(signum, info, context);
			return;
		}
		//se itereaza prin segmentele fisierului
		for (int i = 0; i < exec->segments_no; i++) {
			so_seg_t *curr_seg = (exec->segments) + i;
			//se retine adresa de inceput a segmentului si adresa fault-ului de tip int pt a fi comparate cu usurinta
			unsigned int i_vaddr = curr_seg->vaddr;
			unsigned int fault_addr = (int)info->si_addr;
			//pentru simplitate se retine page size-ul intr-o variabila
			int page_size = getpagesize();
			//daca nu a fost initializata zona de data, se creeaza un vector caracteristic pt a retine paginile mapate
			if (curr_seg->data == NULL) {
				int page_no = (fault_addr + i_vaddr) / page_size;
				
				curr_seg->data = (void *)calloc( page_no, sizeof(char));
			}		
			//se verifica daca fault-ul face parte din segmentul luat
			if (fault_addr >= i_vaddr && fault_addr < i_vaddr + curr_seg->mem_size){	
				//se gaseste index-ul paginii cu fault
				int page_index = (fault_addr - i_vaddr) / page_size;
				//daca pagina a fost deja mapata -> default handler
				if(((char *)(curr_seg->data))[page_index] == 1) {
					old_action.sa_sigaction(signum, info, context);
					return;
				}
				//se mapeaza pagina cu 0 mai intai si se lasa permisiuni de scriere pentru a putea fi modificata pentru diferite cazuri
				char *page = mmap((void *)(i_vaddr + page_index * page_size), page_size, PROT_WRITE, MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
				//daca maparea a esuat -> default handler
				if (page == MAP_FAILED) {
					old_action.sa_sigaction(signum, info, context);
					return;
				}
				//in functie de size stim in ce zona din segment ne aflam
				int size = curr_seg->file_size - page_index * page_size;
				//ne aflam in fisier pt size > 0
				if (size > 0) {
					//ne mutam la adresa paginii cu fault in fisier
					int err = lseek(fd, curr_seg->offset + page_index * page_size, SEEK_SET);
					if (err == -1) {
						old_action.sa_sigaction(signum, info, context);
						return;
					}
					//file size nu este neaparat multiplu de pagini asa ca,
					//in cazul in care fault-ul se afla in "pagina" de baraj trebuie sa citim in memorie bucata de pagina din file size si restul ramane 0
					if (size < page_size)
						err = read(fd, (void *) page, size);
					//se citeste in memorie pagina din fiser
					else if(size >= page_size)
						err = read(fd, (void *) page, page_size);
					if (err == -1)  {
						old_action.sa_sigaction(signum, info, context);
						return;
					}
				}
				//se noteaza pagina ca fiind mapata
				((char *)(curr_seg->data))[page_index] = 1;
				//se pun permisiunile segmentului pe pagina respectiva
				mprotect(page, page_size, curr_seg->perm);
				return;
			} 
		}
		signal(SIGSEGV, NULL);
	}

	int so_init_loader(void)
	{	
			int rc;
			struct sigaction sa;

			memset(&sa, 0, sizeof(sa));
			sa.sa_sigaction = segv_handler;
			sa.sa_flags = SA_SIGINFO;
			rc = sigaction(SIGSEGV, &sa, &old_action);
			if (rc < 0) {
					perror("sigaction");
					return -1;
			}
			return 0;
			
	}

	int so_execute(char *path, char *argv[])
	{
		fd = open(path, O_RDONLY);
		if (fd < 0)
			return -1;

		exec = so_parse_exec(path);
		if (!exec)
			return -1;
		
		so_start_exec(exec, argv);
		close(fd);
		return -1;
	}