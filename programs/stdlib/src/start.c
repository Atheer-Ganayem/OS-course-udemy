#include "peachos.h"

extern int main(int, char**);
void c_start() {
  struct process_arguments args;
  peachos_process_get_arguments(&args);
  
  int res = main(args.argc, args.argv);
  if (res == 0) {

  }
}