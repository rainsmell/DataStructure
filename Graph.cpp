#include <iostream>
#include <vector>

template <typename Tv>
class Vertex {
public:
    typedef enum { UNDISCOVERED, DISCOVERED, VISITED } VStatus;

    Vertex(const Tv& d = (Tv)0) :
    data(d), inDegree(0), outDegree(0), status(UNDISCOVERED),
    dTime(-1), fTime(-1), parent(-1), priority(INT_MAX) {}

    int inDegree, outDegree;
    Tv data; 

private:
    
    
    int dTime, fTime;
    int parent;
    int priority;
    VStatus status;
};

template <typename Te>
class Edge
{
public:
    typedef enum { UNDETERMINED, TREE, CROSS, FORWARD, BACKWARD } EStatus;
	Edge(const Te& d, int w) :
    data(d), weight(w), status(UNDETERMINED) {}
	~Edge() {};

    Te data;
private:
    
    int weight;
    EStatus status;
};

template <typename Tv, typename Te>
class GraphMatrix
{
public:
    GraphMatrix() { n = e = 0; }
    ~GraphMatrix() 
    {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                delete E[i][j];
    }

    Tv& vertex(int i) { return V[i].data; }

    int insert(const Tv& vertex)
    {
        for (int i = 0; i < n; i++)
            E[i].push_back(nullptr);
        n++;
        E.push_back(std::vector<Edge<Te>*>());
        V.push_back(Vertex<Tv>(vertex));
        return (n - 1);
    }

    Tv remove(int i)
    {
        for (int j = 0; j < n; j++)
            if (exists(i, j)) { delete E[i][j]; V[j].inDegree--; }

        E.remove(i);
        n--;

        for (int j = 0; j < n; j++)
            if (exists(j, i)) { delete E[j].erase(i); V[j].outDegree--; }
        
        Tv vBak = vertex(i);
        V.erase(i);
        return vBak;
    }

    bool exists(int i, int j)
    {
        return ((0 <= i) && (n >= j) && (0 <= i) && (n >= i) && (E[i][j] != nullptr));
    }

    Te& edge(int i, int j) { return E[i][j]->data; }
    typename Edge<Te>::EStatus& status(int i, int j) { return E[i][j]->status; }
    int& weight(int i, int j) { return E[i][j]->weight; }
    int degree(int i) { return (V[i].outDegree + V[i].inDegree); }

    void insertEdge(const Te& edge, int w, int i, int j)
    {
        if (exists(i, j)) return;
        E[i][j] = new Edge<Te>(edge, w);
        e++;
        V[i].outDegree++;
        V[j].inDegree++;
    }

    Te removeEdge(int i, int j)
    {
        if (!exists(i, j)) return Te(0);
        Te eBak = edge(i, j);
        delete E[i][j];
        E[i][j] = nullptr;
        e--;
        V[i].outDegree--;
        V[j].inDegree--;
        return eBak;
    }

private:
    int n, e; // n:顶点的规模，e：边的规模
    std::vector<Vertex<Tv>> V; 
    std::vector<std::vector<Edge<Te>*>> E; 
};

int main(int argc, char* argv[])
{
    int v[] = { 0, 10, 2, 3, 4, 5, 6 };
    GraphMatrix<int, int> graph;

    for (int i = 0; i < sizeof(v); i++)
        graph.insert(v[i]);

    graph.insertEdge(2, 0, 1, 2);
    graph.insertEdge(10, 0, 1, 3);
    graph.insertEdge(11, 0, 1, 6);
    graph.insertEdge(12, 0, 2, 6);
    
    int data = graph.edge(1, 6);
    int degree = graph.degree(2);
    int vdata = graph.vertex(1);

    std::cout << data << std::endl;
    std::cout << degree << std::endl;
    std::cout << vdata << std::endl;

    return 0;
}