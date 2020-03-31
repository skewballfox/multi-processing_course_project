import numpy as np

def generate_adjacency_matrix(number_of_nodes,filename):
    random_graph=np.random.randint(0,9,(number_of_nodes,number_of_nodes))
    for i in range(number_of_nodes):
        random_graph[i,i]=0
    np.savetxt(filename,random_graph,delimiter=" ")

if __name__=="__main__":
    generate_adjacency_matrix(50,"random_array_2.txt")

