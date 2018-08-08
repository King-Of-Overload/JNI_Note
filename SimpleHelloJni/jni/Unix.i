%module Unix

%header%{
#include <unistd.h>
%}

typedef unsigned int uid_t;
extern int counter;
extern uid_t getuid(void);

