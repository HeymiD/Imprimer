#include "imprimer.h"

typedef struct args{
    char* arg_n;
    struct args *next_arg;

}args;

typedef struct{
    char* prog_name;
    int dest_type_index;
    int src_type_index;
    int number_args;
    char prog_args[1024];
}conv_prog;


/*void run(char* types[],char* types_p,int type_pos);
void usage();
void run_command(unsigned int ind, args* arguments,char* types[],char* types_p,int type_pos);
*/
void run(char* batch_file, int batch_mode);
void usage(FILE* f);
int set_type(int type_pos, char** types, args* arguments);
int create_printer(PRINTER printers[], args* type, int printer_pos);
int valid_type(char* type,char**types,int type_pos);
void set_conversion_matrix(int typ1, int type2,int type_adj_matrix[32][32]);
int minDistance(int dist[], int visited[]);
//char* shortest_pipe_path(int graph[32][32], int src, int dest, int con_p_i, conv_prog prog_arr[]);
void shortest_pipe_path(int graph[32][32], int src, int dest, int con_p_i, conv_prog prog_arr[], char* progs[]);
void printPath(int parent[], int j,int path[], int i);
void printPrograms(int con_p_i, conv_prog prog_arr[],int path_length, int path[], char* progs[]);

void queue_job(JOB* job_pointer, JOB* jobs[], int pos);
JOB* dequeue_job(JOB* job_pointer, JOB* jobs[], int pos);
JOB create_JOB(JOB arr[], int pos,int jobid,JOB_STATUS status,char*filename,
    char* file_type, PRINTER_SET eligible_printers);
void printJobs(FILE* f, JOB jobs_arr[],int job_pos);
void printJob(JOB* job);
void process_Job(FILE* f, char**types,int type_pos,JOB jobs[], int job_pos, PRINTER printers[], int printer_pos,
    int graph[32][32], int con_p_i, conv_prog prog_arr[], char* progs[]);
void pipeline(char* progs[], PRINTER* printer, conv_prog con_progs[], int conv_prog_ind, JOB* job);



//void run_command(unsigned int ind, args* arguments);//,char* types[],int type_pos,char** types_p);