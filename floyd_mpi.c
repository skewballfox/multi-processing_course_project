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
#include <mpi.h>

//Function declarations
int getGraphSize(char *);
void getGraph(char * ,const int nodeCount,int Graph[nodeCount][nodeCount]);
void printGraph(const int nodeCount,int Graph[][nodeCount]);

int getLoopCount(const int nodeCount,int numberOfProcesses);
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
    int nc
    if (processRank==0) {
        char *Graphfile=argVector[1];

        //get the number of nodes in the graph    
        nc=getGraphSize(Graphfile);
    }
    MPI_Bcast(nc,1,MPI_INT,0,MPI_COMM_WORLD);
    const int nodeCount=(const int)nc;
    int sizes[nodeCount]
    int Graph[nodeCount][nodeCount];
    if (numberOfProcesses<NodeCount){//if simple parallization will work

        MPI_Aint base;
        MPI_Address(Graph[0], &base);
        MPI_Aint* displacements = new int[nodeCount];
        for (int i=0; i<nodeCount; ++i)
        {
            MPI_Address(nodeCount[i], &displacements[i]);
            displacements[i] -= base;
        }

        MPI_Datatype matrixType;
        MPI_Type_hindexed(nodeCount, sizes, displacements, MPI_INTEGER, &matrixType);
        MPI_Type_commit(&matrixType);
    } else {
        printf("please try again with a process count less than the node count");    
    }
    if (processRank==0){
        getGraph(nodeCount,Graph)
    }
    MPI_Bcast(Graph,1,matrixType,0,MPI_COMM_WORLD);
    printf("the graph for process %d\n\n",processRank);
    printGraph(nodeCount,Graph);


    printf("%lf %lf\n",finish,start);

    //display the results
		printf("Resulting Graph: \n");
    printGraph(nodeCount,Graph);
    storeElapsedTime(finish-start);
    return 0;
}

//Functions
int getGraphSize(char *arrayFile){
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
    return lineCount;
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
