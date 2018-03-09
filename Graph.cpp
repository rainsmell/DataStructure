#include <iostram>
#include <vector>

template <typename Tv>
class Vertex {
public:
    typedef enum { UNDISCOVERED, DISCOVERED, VISITED } VStatus;

    Vertex(const Tv& d = (Tv)0) :
    data(d), inDegree(0), outDegree(0), status(UNDISCOVERED),
    dTime(-1), fTime(-1), parent(-1), priority(INT_MAX) {}

private:
    Tv data; 
    int inDegree, outDegree;
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
	Edge(const Te& d, ine w) :
    data(d), weight(w), status(UNDETERMINED) {}
	~Edge();
private:
    Te data;
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

    int insert(const Tv& vertex)
    {
        for (int i = 0; i < n; i++)
            E[i].push_back(nullptr);
        n++;
        E.push_back(std::vector<Edge<Te>*>(nullptr));
        return V.push_back(Vertex<Tv>(vertex));
    }

    bool exists(int i, int j)
    {
        return ((0 <= i) && (n >= j) && (0 <= i) && (n >= i) && (E[i][j] != nullptr));
    }

    int insertEdge(const Te& edge, int i, int j)
    {
        if (exists(i, j)) return;
        
    }

private:
    int n, e; // n:顶点的规模，e：边的规模
    std::vector<Vertex<Tv>> V; 
    std::vector<std::vector<Edge<Te>*>> E;  // 邻接矩阵，外层为顶点规模，内层为边
}