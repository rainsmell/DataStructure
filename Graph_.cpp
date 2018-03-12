#include <iostream>
#include <vector>
#include <list>

template <typename Tv>
class undirected_graph
{
public:
    undirected_graph(int V) :
        n(V), e(0)
    {
        for (int i = 0; i < n; i++)
            adjacent.push_back(new std::list<Tv>());
    };

    // 返回顶点数量
    int V() { return n; } 
    int E() { return e; }
    std::list<int> adj(int v) 
    {
        return *adjacent[v];
    }

    void addEdge(int v, int w)
    {
        adjacent[v]->push_back(w);
        adjacent[w]->push_back(v);
        e++;
    }

    // 返回G图中顶点v的度数
    static int degree(undirected_graph G, int v)
    {
        return G.adj(v).size();
    }

    // 返回G图中最大的度数
    static int maxDegree(undirected_graph& G)
    {
        int max = 0;
        for (int i = 0; i < G.V(); i++)
            if (degree(G, i) > max)
                max = degree(G, i);
        return max;
    }

    static double avgDegree(undirected_graph& G)
    {
        return (2.0 * G.V() / G.E());
    }

    static int numberOfSelfLoops(undirected_graph& G)
    {
        int count = 0;
        for (int i = 0; i < G.V(); i++)
        {
            std::vector<int> vertexes = G.adj(i);
            for (auto v : vertexes)
                if (v == i)
                    count++;
        }
        return count;
    }

private:
    std::vector<std::list<Tv>*> adjacent;
    int n;
    int e;
};

int main(int argc, char* argv[])
{
    undirected_graph<int> graph(10);

    graph.addEdge(2, 3);
    graph.addEdge(2, 6);

    std::cout << undirected_graph<int>::degree(graph, 2) << std::endl;

    return 0;
}