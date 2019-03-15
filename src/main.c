#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <readline/readline.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "imprimer.h"
#include "imprimer_helpers.h"


#define HELP             0
#define QUIT             1
#define TYPE             2
#define PRINTER_C        3
#define CONVERSION       4
#define PRINTERS         5
#define JOBS             6
#define PRINT            7
#define CANCEL           8
#define PAUSE            9
#define RESUME          10
#define DISABLE         11
#define ENABLE          12



static struct option_info {
        unsigned int val;
        char *name;
        char used;
        int has_arg;
        char *argname;
        //char *descr;
} option_table[] = {
 {HELP,         "help",       0,        no_argument, NULL},

 {QUIT,         "quit",       0,        no_argument, NULL},

 {TYPE,         "type",       0,        required_argument, "file_type"},

 {PRINTER_C,      "printer",    0,        required_argument, "{printer_name,file_type}"},

 {CONVERSION,   "conversion", 0,        required_argument, "{file_type1, file_type2, conversion_prog[arg1,arg2,..]}"},

 {PRINTERS,     "printers",   0,        no_argument, NULL},

 {JOBS,         "jobs",       0,        no_argument, NULL},

 {PRINT,        "print",      0,        required_argument, "file_name {printer1 printer2 ...}"},

 {CANCEL,       "cancel",     0,        required_argument, "job_number"},

 {PAUSE,        "pause",      0,        required_argument, "job_number"},

 {RESUME,       "resume",     0,        required_argument, "job_number"},

 {DISABLE,      "disable",    0,       required_argument, "printer_name"},

 {ENABLE,       "enable",     0,       required_argument, "printer_name"},

 {0,0,0,0,0}
};

#define NUM_OPTIONS (13)

//static struct option long_options[NUM_OPTIONS];
/*static void init_options() {
    for(unsigned int i = 0; i < NUM_OPTIONS; i++) {
        struct option_info *oip = &option_table[i];
        struct option *op = &long_options[i];
        op->name = oip->name;
        op->has_arg = oip->has_arg;
        op->flag = NULL;
        op->val = oip->val;
    }
}*/








//char* types[32];
//int type_pos = 0;

/*int inc_type_pos(){
    return type_pos=type_pos+1;
}*/

static int quit;

//int vars[] = {help,quit};
/*
 * "Imprimer" printer spooler.
 */

int main(int argc, char *argv[])
{
    //init_options();
    char optval;
    char* batch_filename;
    int batch_mode=0;
    //printf("%d\n", optind);

    //char* types[32];
    //char* types_p=malloc(sizeof(types));
    //int type_pos = 0;

    while(optind < argc) {
	if((optval = getopt(argc, argv, "i:o:")) != -1) {
	    switch(optval) {
        case 'i':
        batch_filename = argv[2];
        batch_mode=1;
        break;
        case 'o':
        batch_filename=argv[2];
        batch_mode=2;
        break;
	    case '?':
		fprintf(stderr, "Usage: %s [-i <cmd_file>] [-o <out_file>]\n", argv[0]);
		exit(EXIT_FAILURE);
		break;
	    default:
		break;
	    }
	}
    }

    //run(types, types_p,type_pos);
    run(batch_filename,batch_mode);
    exit(EXIT_SUCCESS);
}

void run(char* batch_filename, int batch_mode){//char* types[],char* types_p, int type_pos){

    FILE* batch_file;
    FILE* batch_file_out=stdout;
    if(batch_mode==1){
         batch_file= fopen( batch_filename,"r");
         if(batch_file==NULL){
            char err_buff[100];
            printf("%s\n",imp_format_error_message("File cannot be opened", err_buff, 100));
            return;
         }
    }
    if(batch_mode==2){
        batch_file_out=fopen(batch_filename,"w");
        //batch_mode=0;
    }



    quit=0;




    //unsigned int ind=-1;

    //char str[100];


    int type_pos=0;
    char* types[32];

    PRINTER printers[32];
    int printer_pos=0;

    int type_adj_matrix[32][32];

    for(int i=0;i<32;i++){
                for(int j=0;j<32;j++){
                    type_adj_matrix[i][j]=0;
                }
            }


    conv_prog conversion_programs[32*32];
    int conversion_programs_index=0;

    //PRINTER_SET printer_set = 0x00000000;

    JOB jobs_arr[100];
    int job_arr_pos=0;




    //char* pointer=malloc(32*sizeof(char*));


    while(!quit ){

        unsigned int ind=-1;


        char *str_p;

        //printf("batch mode: %d\n",batch_mode);

        args arg_struct;
        args* arg_struct_p=malloc(sizeof(arg_struct));
        args* arg_struct_head = arg_struct_p;
        arg_struct.arg_n=NULL;

        char buffer_command[100];

        if(batch_mode==1){
            char temp_buff[100];
            //char* temp_str;
            str_p = fgets(temp_buff, 100, batch_file );
            if(str_p==NULL){
                batch_mode=0;
            }
            else{
                //printf("%s", str_p);
                str_p[strcspn(str_p, "\n")] = 0;
            //printf("%s",buffer_command);
                strcpy(buffer_command,str_p);
            }
            //strcpy(buffer_command, str_p);

        }

        //if(batch_mode){printf("%s\n", str_p);}


        if(batch_mode!=1){
            str_p=readline("imp>");
        }




        //if(batch_mode==1){printf("%s\n", str_p);}


        //str_p=malloc(strlen(str));



        //char buffer_arg1[100];


        int number_args=0;



        if(batch_mode!=1){strcpy(buffer_command,str_p);}

        if(strcmp(str_p,"")==0){
            continue;
        }


        //printf("%s\n", str_p);
        //break;


        for(unsigned int i = 0; i < NUM_OPTIONS; i++){

            char* com = strtok(str_p," ");
            if(strcmp(com,option_table[i].name)==0){

                char* p = strtok(buffer_command," ");



                while(p!=NULL){
                    //printf("%s\n", p);
                    p = strtok(NULL," ");
                    number_args++;
                    args next;
                    args* next_p=malloc(sizeof(next));
                    next_p->arg_n=p;
                    arg_struct_head->next_arg=next_p;

                    arg_struct_head=arg_struct_head->next_arg;

                }



                ind=option_table[i].val;

                break;
            }


            }


        //printf("Ind: %d\n", ind);
        if(ind==-1){
            char bb[100];
            fprintf(batch_file_out, "%s\n", imp_format_error_message("Invalid Command.\n", bb, 100));
            usage(batch_file_out);
        }

        if(ind==PAUSE){

            char* arg1 = malloc(sizeof(arg_struct_p->next_arg->arg_n));
            strcpy(arg1,arg_struct_p->next_arg->arg_n);

            int job_number = atoi(arg1);

            for(int i=0;i<job_arr_pos;i++){
                if(jobs_arr[i].jobid==job_number){
                    killpg(jobs_arr[i].pgid,SIGSTOP);
                }
            }
        }

        if(ind==RESUME){
            char* arg1 = malloc(sizeof(arg_struct_p->next_arg->arg_n));
            strcpy(arg1,arg_struct_p->next_arg->arg_n);

            int job_number = atoi(arg1);

            for(int i=0;i<job_arr_pos;i++){
                if(jobs_arr[i].jobid==job_number){
                    killpg(jobs_arr[i].pgid,SIGCONT);
                }
            }
        }

        if(ind==CANCEL){
            char* arg1 = malloc(sizeof(arg_struct_p->next_arg->arg_n));
            strcpy(arg1,arg_struct_p->next_arg->arg_n);

            int job_number = atoi(arg1);



            for(int i=0;i<job_arr_pos;i++){
                if(jobs_arr[i].jobid==job_number){
                    jobs_arr[i].status=ABORTED;
                    killpg(jobs_arr[i].pgid,SIGTERM);
                }
            }
        }



        if(ind==HELP){

            usage(batch_file_out);
        }
        if(ind==QUIT){
            if(batch_mode>0){
            if(batch_file!=NULL){
                fclose(batch_file);
            }

            if(batch_file_out!=NULL){
                fclose(batch_file_out);
            }

    }
        //free(types_p);
            quit=1;
        }
        if(ind==TYPE){
            //printf("%d\n", number_args);
            if(number_args!=2){
                char bb[100];
                fprintf(batch_file_out, "%s\n", imp_format_error_message("Wrong number of arguments.", bb, 100));
                //usage(batch_file_out);
            }
            else{
                if(set_type(type_pos,types,arg_struct_p)){
                type_pos++;}
            }
        }
        if(ind==PRINTER_C){
            //printf("%d\n", number_args);
            if(number_args!=3){
                char bb[100];
                fprintf(batch_file_out, "%s\n", imp_format_error_message("Wrong number of arguments.", bb, 100));
                //usage(batch_file_out);
            }
            else{
                if(create_printer(printers, arg_struct_p,printer_pos)){
                    char pb[200];
                    fprintf(batch_file_out, "%s\n", imp_format_printer_status(&printers[printer_pos], pb, 200));
                    //printer_set = printer_set | (0x1 << printers[printer_pos].id);
                    //printf("%0x\n",printer_set);
                    printer_pos++;
                }
                else{
                    usage(batch_file_out);
                }

            }
        }
        if(ind==CONVERSION){

            int conv_err=0;

            if(number_args<3){
                //usage(batch_file_out);
                char bb[100];
                fprintf(batch_file_out, "%s\n", imp_format_error_message("Wrong number of arguments.", bb, 100));
            }

            char* arg1 = malloc(sizeof(arg_struct_p->next_arg->arg_n));
            strcpy(arg1,arg_struct_p->next_arg->arg_n);
            int type1 = valid_type(arg1,types,type_pos);

            if(type1==-1){
                //printf("%s has not been declared before.\n",arg1);
                conv_err=1;
            }

            char* arg2 = malloc(sizeof(arg_struct_p->next_arg->next_arg->arg_n));
            strcpy(arg2,arg_struct_p->next_arg->next_arg->arg_n);

            int type2 = valid_type(arg2,types,type_pos);
            if(type2==-1){
                //printf("%s has not been declared before.\n",arg2);
                conv_err=1;
            }

            if(conv_err==0){

            set_conversion_matrix(type1,type2,type_adj_matrix);


            char* arg3 = malloc(sizeof(arg_struct_p->next_arg->next_arg->next_arg->arg_n));
            strcpy(arg3,arg_struct_p->next_arg->next_arg->next_arg->arg_n);

            //char* cnv_prg_args[number_args-4];

           // printf("number_args: %d\n", number_args-4);

            args* cur_arg = arg_struct_p->next_arg->next_arg->next_arg->next_arg;

            //int len=0;

            //conversion_programs[conversion_programs_index].prog_args=cur_arg;
            int len=0;

            for(int i=0;i<number_args-4;i++){


                //->arg_n
                char* prg_arg = malloc(sizeof(cur_arg->arg_n));
                strcpy(prg_arg,cur_arg->arg_n);
                strcpy(conversion_programs[conversion_programs_index].prog_args+len
                    ,prg_arg);
                len+=strlen(prg_arg);
                //printf("length: %d\n", len);
                strcpy(conversion_programs[conversion_programs_index].prog_args+len
                    ," ");
                len++;

                /*strcpy(prg_arg,cur_arg->arg_n);
                strcpy(cnv_prg_args+len,prg_arg);
                len++;
                strcpy(cnv_prg_args+len," ");
                len = strlen(prg_arg);*/


                cur_arg=cur_arg->next_arg;
            }

            //conv_prog prog1;

            //for(int i=0;i<number_args-4;i++){
                //prog1.prog_args[i]=cnv_prg_args[i];
                //printf("cnv_prg_args[%d]=%s\n", i,cnv_prg_args[i]);
            //}
            //strcpy(conversion_programs[conversion_programs_index].prog_args,cnv_prg_args);
            conversion_programs[conversion_programs_index].prog_name=arg3;
            conversion_programs[conversion_programs_index].dest_type_index=type2;
            conversion_programs[conversion_programs_index].src_type_index=type1;
            conversion_programs[conversion_programs_index].number_args=number_args-4;
            //prog1.prog_name=arg3;
            //prog1.dest_type_index=type2;
            //prog1.src_type_index=type1;
            //prog1.number_args=number_args-4;

            //printf("args: %d\n", prog1.number_args);



            //conversion_programs[conversion_programs_index]=prog1;

            //for(int n=0;n<number_args-4;n++){
              //  conversion_programs[conversion_programs_index].prog_args[n]=cnv_prg_args[n];
                //printf("cnv_prg_args[%d]=%s\n", i,conversion_programs[conversion_programs_index].prog_args[i]);
            //}

            conversion_programs_index++;
            }
            else{
                char bb[100];
                fprintf(batch_file_out, "%s\n", imp_format_error_message("Type not declared.", bb, 100));
            }





            //free(arg1);
            //free(arg2);
            //free(arg3);
            /*for(int i=0;i<conversion_programs_index;i++){
                printf("program: %s type_i:%d \n", conversion_programs[i].prog_name,conversion_programs[i].dest_type_index);
            }*/


            /*for(int i=0;i<32;i++){
                for(int j=0;j<32;j++){
                    if(j==31){
                        printf("%d\n", type_adj_matrix[i][j]);
                        continue;
                    }
                    printf("%d ", type_adj_matrix[i][j]);
                }
            }*/



            }

        if(ind==PRINTERS){

            for(int i=0;i<printer_pos;i++){

                char buff[100];
                PRINTER* p = &printers[i];
                char * print_stat = imp_format_printer_status(p, buff, 100);
                fprintf(batch_file_out, "%s\n", print_stat);
                /*printf("PRINTER, ");
                printf("%d, ",printers[i].id);
                printf("%s, ",printers[i].name);
                printf("%s, ",printers[i].type);
                if(printers[i].busy){printf("busy, ");}
                else{printf("not busy,");}
                if(printers[i].enabled){printf("enabled");}
                else{printf("disabled");}
                printf("\n");*/
            }
        }

        if(ind==JOBS){
            //printf("%d\n", job_arr_pos);
            //printJob(jobs_arr[job_arr_pos]);
            printJobs(batch_file_out, jobs_arr,job_arr_pos);
        }

        if(ind==ENABLE){
            if(number_args==2){
                char* printer_name = malloc(sizeof(arg_struct_p->next_arg->arg_n));
                strcpy(printer_name,arg_struct_p->next_arg->arg_n);

                for(int i=0;i<printer_pos;i++){
                    if(strcmp(printers[i].name,printer_name)==0){
                        printers[i].enabled=1;
                    }
                }

                char* progs[32];
                for(int j=0;j<32;j++){progs[j]="";}

                process_Job(batch_file_out,types,type_pos,jobs_arr, job_arr_pos, printers, printer_pos,
                        type_adj_matrix, conversion_programs_index, conversion_programs, progs);
            free(printer_name);
            }
            else{
                char b[100];
                fprintf(batch_file_out, "%s\n", imp_format_error_message("Wrong number of arguments.", b, 100));
                //usage(batch_file_out);
            }
        }
        if(ind==DISABLE){
            if(number_args==2){
                char* printer_name = malloc(sizeof(arg_struct_p->next_arg->arg_n));
                strcpy(printer_name,arg_struct_p->next_arg->arg_n);


                for(int i=0;i<printer_pos;i++){
                    if(strcmp(printers[i].name,printer_name)==0){
                        printers[i].enabled=0;
                    }
                }
                char* progs[32];
                for(int j=0;j<32;j++){progs[j]="";}

                process_Job(batch_file_out,types,type_pos,jobs_arr, job_arr_pos, printers, printer_pos,
                        type_adj_matrix, conversion_programs_index, conversion_programs, progs);
            free(printer_name);
            }
            else{
                //usage(batch_file_out);
                char b[100];
                fprintf(batch_file_out, "%s\n", imp_format_error_message("Wrong number of arguments.", b, 100));

            }
        }
        if(ind==PRINT){

            char buffer_arg[100];

            if(number_args<1){
                usage(batch_file_out);
            }

            char* filename = malloc(sizeof(arg_struct_p->next_arg->arg_n));
            strcpy(filename,arg_struct_p->next_arg->arg_n);
            strcpy(buffer_arg,filename);

            //printf("filename: %s\n", filename);
            char* ext = malloc(10);

            char* ext_t = strtok(buffer_arg,".");

                while(ext_t!=NULL){

                    //printf("%s\n", p);

                    strcpy(ext,ext_t);
                    ext_t = strtok(NULL,".");

                }

            //printf("extension is: %s\n", ext);




            //int number_printers = number_args-1;

            int src =valid_type(ext,types,type_pos);

            if(number_args==2){
                PRINTER_SET eligible_printer_set = (0xffffffff);

                JOB* new_job = malloc(sizeof(JOB));

                printf("filename: %s\n", filename);

                *new_job = create_JOB(jobs_arr,job_arr_pos,job_arr_pos,QUEUED,filename, ext, eligible_printer_set);

                job_arr_pos++;

                char* progs[32];
                for(int j=0;j<32;j++){progs[j]="";}

                process_Job(batch_file_out,types,type_pos,jobs_arr, job_arr_pos, printers, printer_pos,
                        type_adj_matrix, conversion_programs_index, conversion_programs, progs);

            //free(new_job);
            //free(filename);


                /*for(int i=0;i<printer_pos;i++){
                    char* dest=printers[i].type;
                    int dest_type = valid_type(dest,types,type_pos);
                    //printf("Dest type: %d\n", dest_type);

                if(src==-1 || dest_type==-1){
                    printf("type not valid.\n");
                }
                else{//printf("%s",ext);

                    printf("i=%d\n", i);
                    //if(i==1){
                        //printf("Dest=%d\n", dest_type);
                        //printf("Src=%d\n", src);
                    //}
                    shortest_pipe_path(type_adj_matrix,src,dest_type,conversion_programs_index,conversion_programs);
                    printf("\n");
                    }
                }*/


            }
            else{

            int err = 0;

            int number_of_print_args = number_args - 2;

            PRINTER_SET eligible_printer_set = 0x00000000;

            //PRINTER p;
            //PRINTER* p_p=&p;

            for(int i=0;i<number_of_print_args;i++){

                char* progs[32];
                for(int k=0;k<32;k++){
                    progs[k]="";
                }

            args* current_arg = arg_struct_p->next_arg->next_arg;
            int counter=0;
            while(counter<i){
                current_arg=current_arg->next_arg;
                counter++;
            }
            char* printer_input = malloc(sizeof(current_arg->arg_n));
            strcpy(printer_input,current_arg->arg_n);

            char* dest="";





            for(int j=0;j<printer_pos;j++){
                if(strcmp(printer_input,printers[j].name)==0){
                    dest=printers[j].type;
                    //eligible_printer_set=eligible_printer_set & (0x1 << printers[i].id);
                    //p_p=&printers[i];
                }
            }



            if(strcmp(dest,"")==0){
                printf("no type matched.\n");
                err=1;

            }
            else{
                int dest_type = valid_type(dest,types,type_pos);

                if(src==-1 || dest_type==-1){
                    printf("type not valid.\n");
                    err=1;
                }
                else{//printf("%s",ext);

                    if(src==dest_type){
                         eligible_printer_set=eligible_printer_set | (0x1 << printers[dest_type].id);
                    }


                    shortest_pipe_path(type_adj_matrix,src,dest_type,conversion_programs_index,conversion_programs,progs);
                    if(strcmp(progs[0],"")!=0){
                        eligible_printer_set=eligible_printer_set | (0x1 << printers[dest_type].id);
                        //printf("eligible printer set: %0x", eligible_printer_set);
                    }
                    free(printer_input);
                    //printf("\n");

                    //printJob(new_job);

                    }
                }
                if(eligible_printer_set==0){
                char buff[100];
                printf("%s\n", imp_format_error_message("Printers are not eligible", buff, 100));
                err=1;
                }
            }


            if(err==0){
            JOB* new_job = malloc(sizeof(JOB));
            //printf("eligible printer set: %0x\n", eligible_printer_set);
            //printf("filename: %s\n", filename);
            *new_job =create_JOB(jobs_arr,job_arr_pos,job_arr_pos,QUEUED,filename, ext, eligible_printer_set);
            job_arr_pos++;

            char* progs[32];
            for(int j=0;j<32;j++){progs[j]="";}

            process_Job(batch_file_out,types,type_pos,jobs_arr, job_arr_pos, printers, printer_pos,
                        type_adj_matrix, conversion_programs_index, conversion_programs, progs);
        //free(new_job);
        //free(filename);

                }

            }

        //free(filename);

        }
    }



    return;
}

void usage(FILE* f)
//char *name;
{
        //struct option_info *opt;

        //fprintf(stderr, "Usage: %s [options] <data file>\n", name);
        //fprintf(stderr, "Valid options are:\n");

        for(unsigned int i = 0; i < NUM_OPTIONS; i++) {
                if(option_table[i].has_arg){printf("--%s\t\t<%s>\n", option_table[i].name,option_table[i].argname);}
                else{fprintf(f,"--%s\t\t<no arguments>\n", option_table[i].name);}
        }
        //exit(EXIT_FAILURE);
}

int set_type(int type_pos, char** types, args* arguments){

            char* arg_str = malloc(sizeof(arguments->next_arg->arg_n));
            strcpy(arg_str,arguments->next_arg->arg_n);

            if(type_pos<1){
                types[type_pos]=arg_str;
                return 1;
            }

            for(int i=0; i<type_pos;i++){
                if(strcmp(arg_str,types[i])==0){
                    //free(arg_str);
                    return 0;
                }
            }

            types[type_pos]=arg_str;

            //for(int i=0; i<=type_pos;i++){
              //  printf("types[%d]=%s\n", i,types[i]);}
            //free(arg_str);
            return 1;
}

int create_printer(PRINTER printers[],args* arguments,int printer_pos){

    //printf("%s\n", arguments->next_arg->arg_n);
    //printf("%s\n", arguments->next_arg->next_arg->arg_n);

    if(arguments->next_arg->arg_n==NULL){
        return 0;
    }

    if(arguments->next_arg->next_arg->arg_n==NULL){
        return 0;
    }

    char* arg_str = malloc(sizeof(arguments->next_arg->arg_n));
    strcpy(arg_str,arguments->next_arg->arg_n);

    PRINTER p;
    p.name=arg_str;

    char* arg_str2 = malloc(sizeof(arguments->next_arg->next_arg->arg_n));
    strcpy(arg_str2,arguments->next_arg->next_arg->arg_n);

    p.type=arg_str2;
    p.id=printer_pos;
    p.busy=0;
    p.enabled=0;

    printers[printer_pos]=p;

    /*if(printer_pos>0){
        for(int i=0;i<=printer_pos;i++){
        printf("printer[%d]_name:%s type: %s\n",i, printers[i].name,printers[i].type);
        }
    }*/



    return 1;
}

int valid_type(char* type,char**types,int type_pos){

    for(int i=0;i<type_pos;i++){
        if(strcmp(types[i],type)==0){
            return i;
        }
    }

    return -1;

}

void set_conversion_matrix(int type1, int type2,int type_adj_matrix[32][32]){

    type_adj_matrix[type1][type2]=1;
}

int minDistance(int dist[], int visited[])
{

   int min = 50;
   int min_index;

   for (int v = 0; v < 32; v++)
     if (visited[v] == 0 && dist[v] <= min)
         min = dist[v], min_index = v;

   return min_index;
}
void printPath(int parent[], int dest, int path[],int i){

    if (parent[dest] == - 1){
        //printf("%d ", dest);
        path[i]=dest;
        return;
    }

    path[i]=dest;

    i++;

    printPath(parent, parent[dest],path,i);


    //printf("%d ", dest);


}


void shortest_pipe_path(int graph[32][32], int src, int dest, int con_p_i, conv_prog prog_arr[], char* progs[]){

    int path[32];
    for(int i=0;i<32;i++){
        path[i]=-1;
    }
    int dist[32];

    int visited[32];


    int parent[32];

    for (int i = 0; i < 32; i++){
        parent[0] = -1;
        dist[i] = 50;
        visited[i] = 0;
    }

    dist[src] = 0;


    for (int count = 0; count < 32 - 1; count++){

        int u = minDistance(dist, visited);

        visited[u] = 1;

        for (int v = 0; v < 32; v++)

            if (!visited[v] && graph[u][v] &&
                dist[u] + graph[u][v] < dist[v])
            {
                parent[v] = u;
                dist[v] = dist[u] + graph[u][v];
            }
    }

    printPath(parent,dest,path,0);
    int path_length=0;
    for(int i=0;i<32;i++){
        if(path[i]>-1){
            path_length++;
        }
    }
    printPrograms(con_p_i,prog_arr,path_length,path,progs);



}

void printPrograms(int con_p_i, conv_prog prog_arr[],int path_length, int path[], char* progs[]){
    /*printf("Path length: %d\n", path_length);
    for(int i=0;i<con_p_i;i++){
        printf("progname: %s dest: %d\n", prog_arr[i].prog_name,prog_arr[i].dest_type_index);
    }*/
    /*for(int i=0;i<path_length;i++){
        printf("path: %d\n", path[i]);
    }*/

    //char* progs[path_length];

    for(int i=0;i<path_length;i++){
        progs[i]="";
    }

    int k=0;
    for(int i=(path_length-1);i>=0;i--){
        for(int j=0;j<con_p_i;j++){
            if(path[i]==prog_arr[j].src_type_index && prog_arr[j].dest_type_index==path[i-1]){
                //printf("%s ", prog_arr[j].prog_name);
                progs[k]=prog_arr[j].prog_name;
                k++;
            }
        }
    }
    //return progs[0];

}

void queue_job(JOB* job_pointer, JOB* jobs[], int pos){
    jobs[pos]=job_pointer;
    //pos++;
}
JOB* dequeue_job(JOB* job_pointer, JOB* jobs[],int pos){
    JOB* deq = jobs[0];



    for(int i=0;i<pos-1;i++){
        jobs[i]=jobs[i+1];

    }

    return deq;
}

JOB create_JOB(JOB arr[], int pos,int jobid,JOB_STATUS status,char*filename,
    char* file_type, PRINTER_SET eligible_printers){

    JOB new_job;
    //JOB* new_job=arr[pos];


    new_job.jobid=jobid;
    new_job.status=status;
    new_job.pgid=0;
    new_job.file_name=filename;
    new_job.file_type=file_type;
    new_job.eligible_printers=eligible_printers;
    //new_job.chosen_printer=chosen_printer;
    gettimeofday(&new_job.creation_time,NULL);
    gettimeofday(&new_job.change_time,NULL);

    arr[pos]=new_job;

    return new_job;

}

void printJob(JOB* job){
    printf("JOB, %d, %d, %d, %s, %s, %0x, %s, \n",
    job->jobid,
    job->status,
    job->pgid,
    job->file_name,
    job->file_type,
    job->eligible_printers,
    job->chosen_printer->name
);

}

void printJobs(FILE* f, JOB jobs_arr[],int job_pos){
    for(int i=0;i<job_pos;i++){
        JOB* j = &jobs_arr[i];
        //printJob(j);
        char buff[100];
        //fprintf(f, "file_name: %s\n", j->file_name);
        fprintf(f, "%s\n",imp_format_job_status(j, buff, 100));
    }
}

void process_Job(FILE* f, char**types,int type_pos,JOB jobs[], int job_pos, PRINTER printers[], int printer_pos,
    int graph[32][32], int con_p_i, conv_prog prog_arr[], char* progs[]){

    for(int i=0;i<job_pos;i++){
        printf("Processing job: %d ... \n", i);
        if(jobs[i].status==QUEUED){
            for(int k=0;k<printer_pos;k++){
                if(printers[k].busy==0 && printers[k].enabled==1){
                int src =valid_type(jobs[i].file_type,types,type_pos);
                int dest = valid_type(printers[k].type,types,type_pos);
                //printf("src:%d\ndest:%d\n", src,dest);
                if(src==dest){
                    printers[k].busy=1;
                    jobs[i].chosen_printer=&printers[k];
                    jobs[i].status=RUNNING;
                    gettimeofday(&jobs[i].change_time,NULL);
                    //char buff[100];
                    //fprintf(f, "filename: %s\n", jobs[i].file_name);
                    char* buff = malloc(100);
                    fprintf(f, "%s\n",imp_format_job_status(&jobs[i], buff, 100));
                    char buf_p[200];
                    fprintf(f, "%s\n",imp_format_printer_status(&printers[i], buf_p, 200));
                    pipeline(progs,jobs[i].chosen_printer, prog_arr,con_p_i,&jobs[i]);
                    free(buff);
                    break;

                }
                else{
                    shortest_pipe_path(graph,src,dest,con_p_i,prog_arr,progs);
                /*for(int j=0;j<32;j++){
                    if(strcmp(progs[j],"")!=0){
                        printf("Prog[0]=%s\n", progs[j]);
                    }
                }*/
                if(strcmp(progs[0],"")!=0){
                        //eligible_printer_set=eligible_printer_set | (0x1 << printers[dest_type].id);
                        //printf("eligible printer set: %0x", eligible_printer_set);
                    printers[k].busy=1;
                    jobs[i].chosen_printer=&printers[k];
                    jobs[i].status=RUNNING;
                    gettimeofday(&jobs[i].change_time,NULL);
                    //fprintf(f, "filename: %s\n", jobs[i].file_name);
                    char* buff = malloc(100);
                    fprintf(f, "%s\n",imp_format_job_status(&jobs[i], buff, 100));
                    char buf_p[200];
                    fprintf(f, "%s\n",imp_format_printer_status(&printers[i], buf_p, 200));
                    pipeline(progs,jobs[i].chosen_printer, prog_arr,con_p_i,&jobs[i]);
                    free(buff);
                    break;
                        }
                    }

                }

            }
        }

    }

}


void pipeline(char* progs[], PRINTER* printer,conv_prog con_progs[], int conv_prog_ind, JOB* job){

    pid_t master_id;

    int path_length=0;

    //int* fd_printer = malloc(sizeof(int));

    int fd_last =imp_connect_to_printer(printer, PRINTER_DELAY);
    //*fd_printer = //imp_connect_to_printer(printer, 0);

    //fd_printer =&fd_last;

    int fd_first = open(job->file_name,O_RDONLY);


    int fd1[2];
    int fd2[2];

    /*if (pipe(fd1)==-1) {
        fprintf(stderr, "Pipe Failed" );
        return;}

        if (pipe(fd2)==-1) {
        fprintf(stderr, "Pipe Failed" );
        return;}

    for(int i=0;i<32;i++){
        if(strcmp(progs[i],"")!=0){
            path_length++;
        }
    }*/



    /*int pipes[path_length][2];

    for(int i=0;i<path_length;i++){
        //int fd[2];
        //pipes[i]=fd;
        if (pipe(pipes[i])==-1) {
        fprintf(stderr, "Pipe Failed" );
        return;}
    }*/

    if((master_id=fork())==0){//MASTER PROCESS

        setpgid(master_id,master_id);

        if(path_length==0){



        pid_t group_id;

        group_id =fork();

        //printf("groupid: %d\n", group_id);
          //  }

        //setpgid(group_id,master_id);

        if(group_id==0){

            //setpgid(master_id,master_id);


            //printf("file descriptor_printer: %d\n", fd_last);//)*fd_printer;
            //printf("file descriptor_file: %d\n", fd_first);
            dup2(fd_first,STDIN_FILENO);
            //dup2(fd_last,STDOUT_FILENO);
            dup2(fd_last,STDOUT_FILENO);//*fd_printer
            char* params[] ={"/bin/cat",NULL};
            execv("/bin/cat",params);
            //close(fd_first);
            //close(fd_last);
            exit(EXIT_FAILURE);
            //return;
                }
        if(group_id>0){
            int status;
            waitpid(group_id,&status, WUNTRACED | WCONTINUED);
            exit(0);
        }



            }
            if(path_length>0){

                //int printer_fd = fd_last;
                //int file_fd = fd_first;


            for(int i=0;i<path_length;i++){

                if(i%2==0){
                    pipe(fd2);
                }
                else{
                    pipe(fd1);
                }

                 conv_prog cur_prog;

                for(int k=0;k<conv_prog_ind;k++){
                    if(strcmp(progs[i],con_progs[k].prog_name)==0){
                    cur_prog=con_progs[k];
                    break;
                        }
                    }

                    char arg1[] = "/bin/";
                    strcpy(arg1+strlen(arg1),cur_prog.prog_name);
                    char* paramlist[]={arg1};

                    if(cur_prog.number_args>0){
                        int arg_i=1;

                    char* token = strtok(cur_prog.prog_args, " ");

                    while (token != NULL) {
        //printf("token : %s\n", token);
                    paramlist[arg_i++]=token;
                    token = strtok(NULL, " ");}

                    paramlist[cur_prog.number_args+1]=NULL;
                    }
                    else{paramlist[1]=NULL;}

            //printf("paramlist[0]: %s\n", paramlist[0]);

            //char* args[cur_prog.number_args];



                pid_t pipeline_p =fork();
                //setpgid(pipeline_p,master_id);

                if(pipeline_p>0){

                    if(i==0){
                        close(fd2[1]);
                        if(i==path_length-1){
                            close(fd1[1]);
                        }
                    }
                    else if(i==path_length-1){
                        if(i==0){
                            close(fd1[1]);
                        }
                        else if(i%2==0){
                            close(fd2[1]);
                        }
                        else{
                            close(fd1[1]);
                        }
                    }
                    else{
                        if(i%2==0){
                            close(fd1[0]);
                            close(fd2[1]);
                        }
                        else{
                            close(fd2[0]);
                            close(fd1[1]);
                        }
                    }

                    int status=0;

                    waitpid(pipeline_p,&status, WUNTRACED | WCONTINUED);

                    //exit(0);


                }
                if(pipeline_p==0){
                    if(i==0){
                        dup2(fd_first,STDIN_FILENO);
                        //close(fd_first);
                        dup2(fd2[1],STDOUT_FILENO);
                        if(i==path_length-1){
                            dup2(fd_last,STDOUT_FILENO);
                        }
                    }
                    else if(i==path_length-1 ){



                        dup2(fd_last,STDOUT_FILENO);
                        //close(fd_last);
                        if(i==0){
                            dup2(fd1[0],STDIN_FILENO);
                        }
                        else if(i%2==0){
                            dup2(fd2[0],STDIN_FILENO);
                        }
                        else{
                            dup2(fd1[0],STDIN_FILENO);
                        }
                    }
                    else{
                        if(i%2==0){
                            dup2(fd1[0],STDIN_FILENO);
                            dup2(fd2[1],STDOUT_FILENO);
                        }
                        else{
                            dup2(fd2[0],STDIN_FILENO);
                            dup2(fd1[1],STDOUT_FILENO);
                        }
                    }

                    execv(paramlist[0],paramlist);
                    //printf("Return not expected. Must be an execv error.n");
                    exit(EXIT_FAILURE);


                }

            }
            //printf("Process ended.\n");
            //exit(0);

        }


    }

    else{//MAIN PROCESS

        setpgid(master_id,0);

        int status=0;
        setpgid(master_id, master_id);
        job->pgid=master_id;
        printer->busy=1;
        printf("group id : %d\n", master_id);

        //pid_t id =
        waitpid(master_id,&status, WUNTRACED | WCONTINUED);
        //printf("wait pid: %d\n", id);
        if(status==0){
            job->status=COMPLETED;
            job->chosen_printer=printer;
                gettimeofday(&job->change_time,NULL);
                char buff[150];
                printf("%s\n", imp_format_job_status(job, buff, 150));
                printer->busy=0;
        }
        else if(WIFSTOPPED(status)){
            job->status=status;
        }
        else if(WIFCONTINUED(status)){job->status=status;}
        else{
            char b[100];
            printf("%s exit statis: %d\n",imp_format_error_message("pipeline failed", b, 100),WEXITSTATUS(status));

        }
        //while(id==0){
                //sleep(1);
          //  wait(0);
            //id=waitpid(master_id,&status,WUNTRACED | WCONTINUED);
                //}

            //if(WIFEXITED(status)){
                /*job->status=ABORTED;
                gettimeofday(&job->change_time,NULL);
                char buff[150];
                printf("%s\n", imp_format_job_status(job, buff, 150));
                printer->busy=0;*/
                //job->status=ABORTED;
                //}

            /*

             if (WIFSTOPPED(status)){
                job->status=PAUSED;
                }
            else if(WIFEXITED(status)){job->status=COMPLETED;
                job->chosen_printer=printer;
                gettimeofday(&job->change_time,NULL);
                char buff[150];
                printf("%s\n", imp_format_job_status(job, buff, 150));
                printer->busy=0;}
                //job->status=PAUSED;}

            else if (WIFCONTINUED(status)){job->status=RUNNING;}
            else{job->status=ABORTED;}*/
            //else {job->status=ABORTED;}

            //free(fd_printer);

            return;
    }

}



/*

//printf("progs: %s\n", progs[0]);

   // pid_t group_id;



    int path_length=0;

    for(int i=0;i<32;i++){
        if(strcmp(progs[i],"")!=0){
            path_length++;
        }
    }

    printf("Path length: %d\n", path_length);

    char printer_buf[100];

    printf("%s\n",imp_format_printer_status(printer, printer_buf, 100));



    int fd1[2];
    int fd2[2];

    if (pipe(fd1)==-1) {
        fprintf(stderr, "Pipe Failed" );
        return;}

        if (pipe(fd2)==-1) {
        fprintf(stderr, "Pipe Failed" );
        return;}

    //group_id = fork();




    //if(group_id>0){
        //job->pgid=group_id;
        //int status;
        //printf("groupid: %d\n", group_id);
        //write(fd2[0],fd_first,sizeof(fd_first));
        //wait(0);
        //wait(0);
        //pid_t id = waitpid(group_id,&status,WUNTRACED);
        //printf("wait pid: %d\n", id);

        //while(id==0){
            //sleep(1);
          //  wait(0);
            //id=waitpid(group_id,&status,0);
        //}

        //printf("wait pid: %d\n", id);

        //if(id>0){


        //job->status=COMPLETED;
        //gettimeofday(&job->change_time,NULL);
        //char buff[150];
        //printf("%s\n", imp_format_job_status(job, buff, 150));
        //printer->busy=0;
        //return;


       // }


        //wait(0);
    //}


    //waitpid(0);

    //if(group_id==0){


        if(path_length>0){

            int fd_first = open(job->file_name,O_RDONLY);
            int fd_last = imp_connect_to_printer(printer, 0);



            for(int i=0; i<path_length;i++){



                 if(i>0){
                    //close(fd2[1]);
                    fd1[0]=fd2[1];
                    //fd2[1]=fd1[1];
                }



            conv_prog cur_prog;

            for(int k=0;k<conv_prog_ind;k++){
                if(strcmp(progs[i],con_progs[k].prog_name)==0){
                cur_prog=con_progs[k];
                break;
            }
        }



            pid_t group_id;

            //int fd_last = imp_connect_to_printer(printer, 0);

            group_id =fork();


            if(group_id>0){


                job->pgid=group_id;
                int status;
        //printf("groupid: %d\n", group_id);

                pid_t id = waitpid(group_id,&status,WUNTRACED);
        //printf("wait pid: %d\n", id);

                while(id==0){
                //sleep(1);
                    wait(0);
                    id=waitpid(group_id,&status,WUNTRACED);
                }

                //if(i!=0){fd1[0]=fd2[1];}

                printf("master_i=%d\n", i);

                printf("fd1[0]: %d\n", fd1[0]);
                printf("fd1[1]: %d\n", fd1[1]);
                printf("fd2[1]: %d\n", fd2[1]);
                printf("fd2[0]: %d\n", fd2[0]);



                if(i==path_length-1){
                    job->status=COMPLETED;
                gettimeofday(&job->change_time,NULL);
                char buff[150];
                printf("%s\n", imp_format_job_status(job, buff, 150));
                printer->busy=0;
                return;

                }



            }

            if(group_id==0){

                close(fd2[0]);
                close(fd1[1]);

                //printf("child_i=%d\n", i);
                if(i==0){

                    //int fd_first = open(job->file_name,O_RDONLY);

                    dup2(fd_first,STDIN_FILENO);
                    close(fd_first);


                }
                else{

                    //fd1[0]=fd2[1];
                    dup2(fd1[0],STDIN_FILENO);
                    close(fd1[0]);
                }

                if(i==path_length-1){



                    dup2(fd_last,STDOUT_FILENO);
                    //close(fd_last);
                    //close(fd1[0]);
                    //close(fd2[1]);
                }
                else{
                    dup2(fd2[1],STDOUT_FILENO);
                    //close(fd2[1]);
                }


                char arg1[] = "/bin/";
            strcpy(arg1+strlen(arg1),cur_prog.prog_name);
            char* paramlist[]={arg1,NULL};
            //printf("paramlist[0]: %s\n", paramlist[0]);

            //char* args[cur_prog.number_args];

        int arg_i=1;

        char* token = strtok(cur_prog.prog_args, " ");

        while (token != NULL) {
        //printf("token : %s\n", token);
        paramlist[arg_i++]=token;
        token = strtok(NULL, " ");}

        //printf("fd1[0]: %d\n", fd1[0]);
        execv(paramlist[0],paramlist);

        exit(EXIT_FAILURE);


            }




        for(int j=0;j<cur_prog.number_args+1;j++){
            printf("path[%d],args[%d]: %s\n", i,j,paramlist[j]);
            }*/


/*

        }
    }
        else{
            int fd_last = imp_connect_to_printer(printer, 0);
            int fd_first = open(job->file_name,O_RDONLY);

            pid_t group_id;

            group_id =fork();

            if(group_id>0){

                job->pgid=group_id;
                int status;
        //printf("groupid: %d\n", group_id);

                pid_t id = waitpid(group_id,&status,WUNTRACED);
        //printf("wait pid: %d\n", id);

                while(id==0){
                //sleep(1);
                    wait(EXIT_SUCCESS);
                    id=waitpid(group_id,&status,WUNTRACED);
                }

                job->status=COMPLETED;
                gettimeofday(&job->change_time,NULL);
                char buff[150];
                printf("%s\n", imp_format_job_status(job, buff, 150));
                printer->busy=0;
                return;


            }

            if(group_id==0){
                printf("file descriptor_printer: %d\n", fd_last);
            printf("file descriptor_file: %d\n", fd_first);
            dup2(fd_first,0);
            dup2(fd_last,1);
            char* params[] ={"/bin/cat",NULL};
            execv("/bin/cat",params);
            //close(fd_first);
            //close(fd_last);
            exit(EXIT_FAILURE);
            //return;
            }



        }


        //pid_t p =fork();

        //if(p>0){
            //printf("child of master : %d\n", p);
            //pid_t n = waitpid(0);
            //wait(0);
            //continue;
            //if(i==path_length-1){
              //  exit(0);
            //}

            //exit(0);
            //job->status=COMPLETED;
            //gettimeofday(&job->change_time,NULL);
        //}

        //if(p==0){

            //printf("Hi, child of child!\n");

            //fd1[0]=fd2[1];
            //printf("cur_prog: %s\n", cur_prog.prog_name);










        char* args[cur_prog.number_args];

        int arg_i=0;

        char* token = strtok(cur_prog.prog_args, " ");

        while (token != NULL) {
        //printf("token : %s\n", token);
        args[arg_i++]=token;
        token = strtok(NULL, " ");}

        for(int j=0;j<cur_prog.number_args;j++){
            printf("path[%d],args[%d]: %s\n", i,j,args[j]);
        }*/

        //int file_desc = open(job.file_name,O_RDONLY);

        //}
        //}


    //}

    //printf("path_length: %d\n", path_length);


    //printf("args[0]:%s\n", con_progs[conv_prog_ind-1].prog_args[0]);

    /*for(int i=0;i<conv_prog_ind;i++){
        conv_prog cur = con_progs[i];
        printf("number_args: %d\n", cur.number_args);
        for(int k=0;k<cur.number_args;k++){
            printf("args[%d]: %s\n", k,cur.prog_args[k]);
        }
    }*/






    //pid_t p;
    //p=fork();













