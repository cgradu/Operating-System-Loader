Tema 1 - Sisteme de Operare:
Autor: Constantin Radu-Giorgian 
Grupa: 321
Seria: CC

Implementarea loaderului a fost impartita in mai multe zile , dar timpul efectiv de lucru a fost de aproximativ 10 ore, majoritar, cu ajutorul laboratoarelor vechi.

In functia segv_handler, handlerul pentru semnalele de tip SIGSEGV(Segmentation Fault), care este transmis functiei prin signum. In info sunt date diferite detalii despre cauza aparitiei semnalului, iar context este unde a fost intalnit a fost implementata astfel:

Daca handlerul primeste alt semnal decat SIGSEGV se ruleaza handlerul default pt acel semnal. La fel si pentru lipsa info-ului.
Am iterat prin segmentele fisierului create de exec_parser si am salvat adresa fault-ului si adresa de inceput a segmentului in variabile de tip int pentru a putea fi comparate usor. In data am initializat un vector caracteristic pentru a marca paginile mapate.
Daca adresa faultului se afla in segmentul luat trebuie mapata pagina cu fault. Daca pagina cu fault a fost deja mapata -> default handler. 
Altfel, exista 3 cazuri:
1. Pagina se afla intre file_size si mem_size(precum o zona .bss)insemnand o pagina cu zerouri si se mapeaza astfel.
2. Pagina se afla in file_size si se mapeaza direct.
3.Pagina se afla si in file_size si in mem_size si trb stiut cat sa fie mapat in memorie si restul ramane cu 0.
Astfel am mapat pagina cu fault direct cu 0 si am citit direct in memorie cat am avut nevoie / daca am avut nevoie cu ajutorul variabilei size.
Pagina este marcata ca fiind mapata in vectorul data.# Operating-System-Loader
