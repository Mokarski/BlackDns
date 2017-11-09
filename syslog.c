#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>

int main(){
openlog("имя вашей программы", 0, LOG_USER);
syslog(LOG_NOTICE, "Can not open file for writing.");
closelog();
return 0;
}

