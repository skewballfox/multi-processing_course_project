/* Purpose: serial implementation of floyd's algorithm. finds all the shortest path from
 * every node to every other node.
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

//Function declarations
const int getGraphSize(char *);
void getGraph(char * ,const int nodeCount,int Graph[nodeCount][nodeCount]);
void printGraph(const int nodeCount,int Graph[][nodeCount]);

void FloydsAlgorithm(const int nodeCount, int Graph[nodeCount][nodeCount]);
void storeElapsedTime(time_t);

int main(int argc, char** argv)
{
    time_t start, finish;
    //IMO a rather hacky solution to handling arbituarily sized graphs
    //not the most elegant but tis the nature of C
    char *Graphfile="array_1.txt";

    //get the number of nodes in the graph    
    const int nodeCount=getGraphSize(Graphfile);

    printf("\nGenerating Graph\n\n");
    //instantiate the graph so it can be modified in Functions
    int Graph[nodeCount][nodeCount];

    //get the edge weights from a file
    getGraph("array_1.txt",nodeCount,Graph);
    printGraph(nodeCount,Graph);

    //find the shortest path using Floyd's Algorithm
    printf("\nstarting floyd's Algorithm\n\n");

    time(&start);
    FloydsAlgorithm(nodeCount,Graph);
    time(&finish);

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

void FloydsAlgorithm(const int nodeCount, int Graph[nodeCount][nodeCount]){
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
    const char *format="serial runtime : %E";
    FILE* file = fopen("serial_runtime.txt","w");
    assert(file);
    printf(format,elapsedTime);
    fprintf(file,format,elapsedTime);
    if (ferror(file)) {
        puts("I/O error when writing");
    }
    fclose(file);
}
