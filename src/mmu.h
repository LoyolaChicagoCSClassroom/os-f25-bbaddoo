#ifndef MMU_H  

#define MMU_H 

#include <stdint.h> 

struct page_directory_entry {  

    unsigned int present : 1;   
    unsigned int  rw : 1;   
    unsigned int  user : 1;   
    unsigned int writethru : 1;  
    unsigned int cachedisabled : 1;  
    unsigned int accessed : 1;   
    unsigned int pagesize : 1;   
    unsigned int ignored : 2;   
    unsigned int  os_specific : 3;  
    unsigned int  frame : 20;

}; 

struct page {  

      unsigned int present : 1;  
      unsigned int rw : 1;  
      unsigned int user : 1;  
      unsigned int accessed : 1;  
      unsigned int dirty : 1;                       
      unsigned int unused : 7; 
      unsigned int frame : 20;   

}; 

#endif 
