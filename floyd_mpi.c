/* Purpose: mpi implementation of floyd's algorithm that relies on the fact that the inner
 * two loops of FWA are independent of the outer loop, thus parallizes those steps. I want to
 * try the pipelined 2d block implementation, but Assignments are stacking up and my brain feels like soup
 * 
 * 
 * 
 * Author: Joshua Ferguson
 */
 
//precompiler macro for finding min value
//I only wish I could claim credit for this
//for reference: https://stackoverflow.com/questions/3437404/min-and-max-in-c#3437484
#define min(a,b) \
  ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <assert.h>
#include <openmpi.h>

//Function declarations
const int getGraphSize(char *);
void getGraph(char * ,const int nodeCount,int Graph[nodeCount][nodeCount]);
void printGraph(const int nodeCount,int Graph[][nodeCount]);

bool check_fox_conditions(const int nodeCount,int numberOfProcesses);
void FloydsAlgorithm(const int nodeCount, int Graph[nodeCount][nodeCount]);
void storeElapsedTime(time_t);

int main(int argCount, char** argVector)
{
    //necessary variables and initializations
    time_t start, finish, localElapsed, elapsed;
    int numberOfProcesses, processRank;

    MPI_Init(&argCount,&argVector);
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);
    MPI_Comm_size(MPI_COMM_WORLD,&numberOfProcesses);
    
    //IMO a rather hacky solution to handling arbituarily sized graphs
    //not the most elegant but tis the nature of C
    if (processRank==0) {
        char *Graphfile=argVector[1];

        //get the number of nodes in the graph    
        const int nodeCount=getGraphSize(Graphfile);

        printf("\nGenerating Graph\n\n");
        //instantiate the graph so it can be modified in Functions
        int Graph[nodeCount][nodeCount];

        //get the edge weights from a file
        getGraph(Graphfile,nodeCount,Graph);
        printGraph(nodeCount,Graph);

        //Divide the graph into chunks
        //MPI_Scatter(Graph,);
    }
    //find the shortest path using Floyd's Algorithm
    printf("\nstarting floyd's Algorithm\n\n");

    //set barrier for performance evalutation
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Wtime(&start);
    FloydsAlgorithm(nodeCount,Graph);
    MPI_Wtime(&finish);
    printf("%lf %lf\n",finish,start);

    //display the results
		printf("Resulting Graph: \n");
    printGraph(nodeCount,Graph);
    storeElapsedTime(finish-start);
    return 0;
}

//Functions
const int getGraphSize(char *arrayFile){
    FILE* file = fopen(arrayFile,"r");
    if (!file) {
        perror("issue opening file containing array");
        exit(EXIT_FAILURE);
    }
    int character;
    int lineCount=0;
    while ((character=fgetc(file)) != EOF) {
        if ('\n'== character) {
            lineCount++;
        }
    }
    if (ferror(file)) {
        puts("I/O error when reading");
    }
    else if (feof(file)) {
        printf("based on the line count the graph has %d nodes\n",lineCount);
    }
    fclose(file);
    return (const int)lineCount;
}
 
void getGraph(char *arrayFile,const int nodeCount, int Graph[nodeCount][nodeCount]){
    FILE* file = fopen(arrayFile,"r");
    if (!file) {
        perror("issue opening file containing array");
        exit(EXIT_FAILURE);
    }
    int value=0;
    int row=0;
    int column=0;
    int ret;
    do {
        if (column==nodeCount) {
            column=0;
            row++;
        }
        int ret=fscanf(file,"%d",&value);
        if (row!=nodeCount) {
            if (row!=column && value==0) {
                Graph[row][column++] = INT_MAX;
            }else {
                Graph[row][column++] = value;
            }
            //printf("column: %d row: %d value: %d graph: %d\n",column-1,row,value,Graph[column-1][row]);
        } 
    } while (!feof(file));
    if (ferror(file)) {
        puts("I/O error grabbing values for nodes\n");
    }
    else if (feof(file)) {
        printf("The graph has been completed\n");
    }
    fclose(file);
    return;
}
int checkConditions(const int nodeCount,int numberOfProcesses,char * condititon){
    /* This int function is used to decide how to best divide the work
     * for the parallel implementation of the floyd algorithm
     * a Note on conditions for fox algorithm
     * b means block mapping which is the preferable approach
     * */
    int pRoot;
    pRoot=sqrt(numberOfProcesses);
    if (pRoot*pRoot==numberOfProcesses) {
        if (nodeCount%pRoot==0) {
            condition='b';
            return pRoot; 
        }
    }
}
void printGraph(const int nodeCount,int Graph[][nodeCount]){
    int row,column;
    for (row = 0; row < nodeCount; row++) {//row
        for (column = 0; column < nodeCount; column++) {//column
            if (Graph[row][column]==INT_MAX) {
                printf("x ");
            } else { 
                printf("%d ",Graph[row][column]);
            }
        }
        printf("\n");
    }
}


void FloydsAlgorithm(int processRank,int numberOfProcesses,const int nodeCount, int Graph[nodeCount][nodeCount]){
    int pRoot=sqrt(numberOfProcesses);
    int blockSize=nodeCount/pRoot;
    int initialRow=(processRank/pRoot)*blockSize;
    int lastRow=initialRow+blockSize;
    int initialColumn=(processRank%pRoot)*blockSize;
    int lastColumn=initialColumn+blockSize;
    int i,j,k;
    for (k = 0; k < nodeCount; k++) {
        for (i = 0; i < nodeCount; i++) {
            for (j = 0; j < nodeCount; j++) {
                if (Graph[i][k]!=INT_MAX && Graph[k][j]!=INT_MAX) {
                    //NOTE: fun things happen when you add INT_MAX to INT_MAX 
                    Graph[i][j]=min(Graph[i][j], Graph[i][k]+Graph[k][j]);
                }
            }   
        }
    }
}


void storeElapsedTime(time_t elapsedTime) {
    const char *format="MPI runtime with core count %d : %E";
    FILE* file = fopen("MPI_runtime.txt","a");
    assert(file);
    printf(format,elapsedTime);
    fprintf(file,format,elapsedTime);
    if (ferror(file)) {
        puts("I/O error when writing");
    }
    fclose(file);
}
