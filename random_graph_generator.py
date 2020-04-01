import numpy as np
import random

def generate_adjacency_matrix(number_of_nodes,density,filename):
    random_graph=np.random.randint(0,density,(number_of_nodes,number_of_nodes))
    for i in range(number_of_nodes):
        random_graph[i,i]=0
        for j in range(number_of_nodes):
            if random_graph[i,j]!=0:
                random_graph[i,j]=random.randint(1,9)
    np.savetxt(filename,random_graph,fmt="%d",delimiter=" ")

if __name__=="__main__":
    random.seed()
    generate_adjacency_matrix(50,1,"sparse_random_graph_2.txt")

