/*
 *
 * Student Name:wulianyi
 * Student ID:516030910252
 *
 */
#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#define true 1
#define false 0

typedef int bool;

typedef struct cache_line
{
    bool valid;
    unsigned long long cache_tag;
    clock_t update_time;
} line_t;

typedef struct cache_set
{
    line_t* line;
} set_t;

long s=0;
long b=0;
long set_num=0;
long line_num=0;
long block_size=0;
char *tracefile=NULL;
bool verbose=false;
set_t *set=NULL;

long bits=64;


void init_cache()
{
    set=(set_t *)malloc(set_num * sizeof(set_t));

    for (int i=0;i<set_num;i++)
    {
	set[i].line=(line_t *)malloc(line_num * sizeof(line_t));

    	for (int j=0;j<line_num;j++)
	{
	    set[i].line[j].valid=false;
	    set[i].line[j].update_time=clock();
	}
    }
}

void free_cache()
{
    for (int i=0;i<set_num;i++)
	free(set[i].line);

    free(set);
}

void hit_cache(unsigned long long address,int *hit,int *miss,int *eviction)
{
    long t=bits-(s+b);
    unsigned long long tag=(address)>>(b+s);
    int set_index=((address)<<t)>>(b+t);

    for (int i=0;i<line_num;i++)
    {
	if (set[set_index].line[i].valid == true && set[set_index].line[i].cache_tag == tag)
	{
	    set[set_index].line[i].update_time=clock();
	    (*hit)++;
	    return;
	}
    }

    for (int i=0;i<line_num;i++)  
	if (set[set_index].line[i].valid == false)
	{
	    set[set_index].line[i].valid = true;
	    set[set_index].line[i].cache_tag = tag;
	    set[set_index].line[i].update_time = clock();
	    (*miss)++;
	    return;
	}

    double earliest_time=(double)set[set_index].line[0].update_time;
    int earliest_index=0;
    for (int i=1;i<line_num;i++)
	if ((double)set[set_index].line[i].update_time<earliest_time)
	{
	    earliest_index=i;
	    earliest_time=(double)set[set_index].line[i].update_time;
	}
    set[set_index].line[earliest_index].cache_tag=tag;
    set[set_index].line[earliest_index].update_time=clock();
    (*miss)++;
    (*eviction)++;
}

void usage()
{
    printf("Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n\n");
    printf("Examples:\n");
    printf("  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

int main(int argc, char * argv[])
{
    char ch;
    while ((ch = getopt(argc,argv,"hvs:E:b:t:")) != -1)
    {
	switch(ch){
	case 'h':
	    usage();
	    break;
	case 'v':
	    verbose=true;
	    break;
	case 's':
	    s=atoi(optarg);
	    set_num=1<<s;
	    break;
	case 'E':
	    line_num=atoi(optarg);
	    break;
	case 'b':
	    b=atoi(optarg);
	    block_size=1<<b;
	    break;
	case 't':
	    tracefile=optarg;
	    break;
	default:
	    usage();
	    break;
	}
    }
    int hit=0;
    int miss=0;
    int eviction=0;
    init_cache();
    FILE *fin=NULL;

    if (!(fin=fopen(tracefile,"r")))
	printf("Can not open the file.");

    char line[256];
    while(fgets(line,256,fin) != NULL)
    {
	if (line[0] != ' ')
	    continue;
	char operation;
	unsigned long long address;
	int size;
	sscanf(line," %c %llx,%d",&operation,&address,&size);
	
	if (operation == 'S' || operation == 'L')
	    hit_cache(address,&hit,&miss,&eviction);

	if (operation == 'M')
	{
	    hit_cache(address,&hit,&miss,&eviction);
	    hit_cache(address,&hit,&miss,&eviction);
	}
    }

    fclose(fin);
    free_cache();

    printSummary(hit, miss, eviction);
    return 0;
}
