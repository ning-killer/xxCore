//#include <unistd.h>
//#include <stdio.h>
//#include "env_default.h"
//#include "fw_env_private.h"
//static char *default_environment;
//static int default_environment_size;
//
//int open_default_environment() {
//    char line[1024];
//    FILE *fp = fopen(CONFIG_FILE,"rt");
//    if(!fp){
//        printf("config file %s open failed\n",CONFIG_FILE);
//        return -1;
//    }
//    while(fgets(line,1024,fp)){
//
//    }
//    return 0;
//}
//
//char *get_default_environment() {
//    return default_environment;
//}
//
//int get_default_environment_size() {
//    return default_environment_size;
//}
//
//
//void close_default_environment() {
//
//}