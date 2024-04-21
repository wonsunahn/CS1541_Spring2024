/** Code by @author Wonsun Ahn
 *
 * Utility program to generate a trace file of your own.  Takes as argument the
 * name of the file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "CPU.h"
#include "trace.h"

int main(int argc, char **argv)
{
  size_t size;
  char *trace_file_name;
  instruction inst;
  dynamic_inst dinst = {{0}};

  unsigned int t_sReg_a;
  unsigned int t_sReg_b;
  unsigned int t_dReg;
  int trcount, i, repeat;
  char itype ;


  if (argc == 1) {
    fprintf(stdout, "\nMissing argument: the name of the file to be generated\n");
    exit(0);
  }
  trace_file_name = argv[1];
  trace_fd = fopen(trace_file_name, "wb");
  fclose(trace_fd);

  printf("\nEnter the number of instructions you want to have in the file \"%s\": ", trace_file_name);
  scanf("%d", &trcount );
  printf("\n") ;
  printf("For each instruction, enter PC, itype, sReg_a, sReg_b, d_Reg and addr, \n");
  printf("where PC, sReg_a, sReg_b, d_Reg and addr are integers and \n");
  printf("itype is a character: R for R-type, L for load, S for store, B for branch or N for No-op \n");

  for (i = 0 ; i < trcount ; i++) {
    printf("\nEnter exactly 6 fields for instruction %d: ", i);
    scanf("%d %c %d %d %d %d",  &inst.PC, &itype, &t_sReg_a, &t_sReg_b, &t_dReg, &inst.Addr) ;

    //set the register values after casting
    inst.sReg_a = (char) t_sReg_a;
    inst.sReg_b = (char) t_sReg_b;
    inst.dReg = (char) t_dReg;

    repeat = 0 ;
    if(itype == 'R') {inst.type = ti_RTYPE ;}
    else if(itype == 'I') {inst.type = ti_ITYPE ;}
    else if (itype == 'L') {inst.type = ti_LOAD;}
    else if (itype == 'S') {inst.type = ti_STORE;}
    else if (itype == 'B') {inst.type = ti_BRANCH ;}
    else if (itype == 'N') {inst.type = ti_NOP ;}
    else {printf("unrecognized instruction type -- try again ") ; repeat = 1;  i-- ; }

    //write the instruction into the trace file
    if (repeat == 0) write_trace(inst, trace_file_name);
  }
  printf("Now, the file \"%s\" contains the following instructions: \n", trace_file_name);
  trace_fd = fopen(trace_file_name, "rb");
  trace_init();
  while(1) {
    instruction *tr_entry = NULL;
    size = trace_get_item(&tr_entry);

    if (!size)
      break;

    // Display the generated trace
    dinst.seq++;
    dinst.inst = *tr_entry;

    printf("%s\n", get_instruction_string(dinst, LONG_FORM));
  }

  trace_uninit();

  exit(0);
}


