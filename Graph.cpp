#include <iostream>
#include <list>
#include <vector>
#include <queue>

template <typename Tv>
class Vertex {
public:
    typedef enum { UNDISCOVERED, DISCOVERED, VISITED } VStatus;

    Vertex(const Tv& d = (Tv)0) :
        data(d), inDegree(0), outDegree(0), status(UNDISCOVERED),
        dTime(-1), fTime(-1), parent(-1), priority(INT_MAX) {}

    int inDegree, outDegree;
    Tv data;

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
    Edge(const Te& d, int w, int v_) :
        data(d), weight(w), v(v_), status(UNDETERMINED) {}
    ~Edge() {};

    Te data;
    int v;
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

template <typename Tv, typename Te>
class adjacency_list_graph
{
public:
    int n, e;
public:
    adjacency_list_graph()
    {
        n = e = 0;
    }

    ~adjacency_list_graph()
    {
        for (int i = 0; i < n; i++)
            for (auto iter = E[i].begin(); iter != E[i].end(); iter++)
                delete *iter;
    }

    Tv& vertex(int i) { return V[i].data; }

    int insert(Tv& vertex)
    {
        n++;
        V.push_back(Vertex<Tv>(vertex));
        E.push_back(std::list<Edge<Te>*>());
        return (n - 1);
    }

    Tv remove(int i)
    {
        for (int j = 0; j < n; j++)
            if (exists(i, j)) { delete *atEdge(i, j); V[j].inDegree--; e--; }

        for (int j = 0; j < n; j++)
        {
            auto iter = atEdge(j, i);
            if (iter != E[j].end())
            {
                delete *iter;
                E[j].erase(iter++);
                V[j].outDegree--;
                e--;
            }
            iter = E[j].begin();
            while (iter != E[j].end()) { if ((*iter)->v > i) --((*iter)->v); iter++; } // 删除顶点i后，比i大的顶点序号将会减一，因此在这里更新edge指向的序号
        }

        E.erase(E.begin() + i);
        n--;

        Tv vBak = vertex(i);
        V.erase(V.begin() + i);
        return vBak;
    }

    bool exists(int i, int j)
    {
        return ((0 <= i) && (n >= j) && (0 <= i) && (n >= i) && (atEdge(i, j) != E[i].end()));
    }

    Te& edge(int i, int j)
    {
        if (exists(i, j))
            return (*atEdge(i, j))->data;
    }

    typename Vertex<Tv>::VStatus& status(int i) { return V[i].status; }
    int& dTime(int i) { return V[i].dTime; }
    int& fTime(int i) { return V[i].fTime; }
    int& priority(int i) { return V[i].priority; }
    int& parent(int i) { return V[i].parent; }
    int& weight(int i, int h) { return (*atEdge(i, j))->weight; }
    typename Edge<Te>::EStatus& status(int i, int j) { return (*atEdge(i, j))->status; }

    void insertEdge(Te edge, int weight, int i, int j)
    {
        if (exists(i, j)) return;
        E[i].push_back(new Edge<Te>(edge, weight, j));
        V[i].outDegree++;
        V[j].inDegree++;
        e++;
    }

    Te removeEdge(int i, int j)
    {
        if (!exists(i, j)) return Te(0);
        auto iter = atEdge(i, j);
        delete *iter;
        E[i].erase(iter);
        V[i].outDegree--;
        V[j].inDegree--;
        e--;
    }

    void show()
    {
        for (int i = 0; i < V.size(); i++)
        {
            std::cout << i << "(" << V[i].data << ")";
            int j = 0;
            for (auto iter = E[i].begin(); iter != E[i].end(); iter++, j++)
            {
                std::cout << "->" << (*iter)->v << "(";
                std::cout << (*iter)->data << ")";
            }
            std::cout << std::endl;
        }
    }

    int fistNbr(int s)
    {
        auto iter = E[s].begin();
        if (iter != E[s].end())
            return (*iter)->v;
        return -1;
    }

    int nextNbr(int s, int v)
    {
        auto iter = E[s].begin();
        for (; iter != E[s].end(); iter++)
            if ((*iter)->v == v) { iter++; break; }
        if (iter != E[s].end()) return (*iter)->v;
        return -1;
    }

    // 图相关算法
public:
    void reset()
    {
        for (int i = 0; i < n; i++)
        {
            status(i) = Vertex<Tv>::UNDISCOVERED;
            dTime(i) = fTime(i) = -1;
            priority(i) = INT_MAX;
            for (auto iter = E[i].begin(); iter != E[i].end(); iter++)
                (*iter)->status = Edge<Te>::UNDETERMINED;
        }
    }

    // 广度优先算法，s为起点
    void bfs(int s)
    {
        reset();
        int clock = 0;
        int v = s;
        do
            if (Vertex<Tv>::UNDISCOVERED == status(v))
                BFS(v, clock);
        while (s != (v = (++v % n)));
    }

    // 深度优先算法
    void dfs(int s)
    {
        reset();
        int clock = 0;
        int v = s;
        do
            if (Vertex<Tv>::UNDISCOVERED == status(v))
                DFS(v, clock);
        while (s != (v = (++v % n)));
    }

    // 拓扑排序
    std::stack<Tv>* tSort(int s)
	{
		int clock = 0, v = s;
		std::stack<Tv>* stack = new std::stack<Tv>();
		do
			if (!TSort(v, clock, s)) {
				while (!s->empty())
					s->pop();
				break;
			}
		while (s != (v = (++v % n)));
	}

    // 优先级搜索排序算法(priority-first search)
    template <typename PU>
    void pfs(int s)
    {
        int v = s;
        do
            PFS(s, PU priorUpdater);
        while (s != (v = (++v % n)));
    }

    // 算法实现
private:
    void BFS(int& v, int& clock)
    {
        std::queue<int> queue;
        status(v) = Vertex<Tv>::DISCOVERED;
        queue.push(v);
        while (!queue.empty())
        {
            int q = queue.front();
            dTime(q) = ++clock;
            queue.pop();
            for (int i = fistNbr(q); i >= 0; i = nextNbr(q, i))
            {
                if (Vertex<Tv>::UNDISCOVERED == status(i))
                {
                    status(i) = Vertex<Tv>::DISCOVERED;
                    parent(i) = v;
                    queue.push(i);
                    status(q, i) = Edge<Te>::TREE;
                }
                else
                    status(q, i) = Edge<Te>::CROSS;
            }
            status(q) = Vertex<Tv>::VISITED;
        }
    }

    void DFS(int& v, int& clock)
    {
        status(v) = Vertex<Tv>::DISCOVERED;
        dTime(v) = ++clock;
        for (int u = fistNbr(v); u >= 0; u = nextNbr(v, u))
        {
            switch (status(u))
            {
            case Vertex<Tv>::UNDISCOVERED:
                status(v, u) = Edge<Te>::TREE;
                parent(u) = v;
                DFS(u, clock);
                break;
            case Vertex<Tv>::DISCOVERED:
                status(v, u) = Edge<Te>::BACKWARD;
                break;
            default:
                status(v, u) = (dTime(v) < dTime(u)) ? Edge<Te>::FORWARD : Edge<Te>::CROSS;
                break;
            }          
        }
        status(v) = Vertex<Tv>::VISITED;
        fTime(v) = ++clock;
    }

    // 基于DFS的拓扑排序，一个有向无环图必然存在拓扑排序
    bool TSort(int& v, int& clock, std::stack<Tv>* stack)
	{
		status(v) = Vertex<Tv>::DISCOVERED;
		dTime(v) = ++clock;
		for (int u = fistNbr(v); u > -1; u = nextNbr(v, u)) {
			switch (status(u)) {
			case Vertex<Tv>::UNDISCOVERED:
				parent(u) = v;
				status(v, u) = Edge<Te>::TREE;
				if (!TSort(u, clock, stack))
					return false;
				break;
			case Vertex<Tv>::DISCOVERED:
				status(v, u) = Edge<Te>::BACKWARD;
				return false;
			default:
				status(v, u) = (dTime(v) < dTime(u)) ? Edge<Te>::FORWARD : Edge<Te>::CROSS;
				break;
			}
		}
		status(v) = Edge<Te>::VISITED;
		stack->push(vertex(v));
        return true;
	}

    template <typename PU>
    void PFS(int s, PU priorUpdater)
    {
        status(s) = Vertex<Tv>::VISITED;
        parent(s) = -1;
        priority(s) = 0;
        while (1)
        {
            for (int v = fistNbr(s); v > -1; v = nextNbr(s, v))
                priorUpdater(this, s, v);
            for (int shortest = MAX_INT, w = 0; w < n; w++)
                if (status(w) == Vertex<Tv>::UNDISCOVERED)
                {
                    if (shortest > priority(w))
                    {
                        shortest = priority(w);
                        s = w;
                    }
                }
            if (status(s) == Vertex<Tv>::VISITED)
                break;
            status(s) = VISITED;
            status(parent(s), s) = Edge<Te>::TREE;
        }
    }

private:
    typename std::list<Edge<Te>*>::iterator atEdge(int i, int j)
    {
        auto iter = E[i].begin();
        while (iter != E[i].end() && (*iter)->v != j) iter++;
        if (iter != E[i].end() && (*iter)->v == j) return iter;
        return E[i].end();
    }

    std::vector<Vertex<Tv>> V;
    std::vector<std::list<Edge<Te>*>> E;
};

template <typename Tv, typename Te>
struct PrimPU
{
    void operator() (adjacency_list_graph<Tv, Te>* g, int uk, int v)
    {
        if (g->status(v) == Vertex<Tv>::UNDISCOVERED)
        {
            if (g->weight(uk, v) < g->priority(v))
            {
                g->priority(v) = g->weight(uk, v);
                g->parent(v) = uk;
            }
        }
    }
}

template <typename Tv, typename Te>
struct DijkstraPU
{
    void operator() (adjacency_list_graph<Tv, Te>* g, int uk, int v)
    {
        if (g->status(v) == Vertex<Tv>::UNDISCOVERED)
        {
            if (g->priority(v) > g->priority(uk) + g->weight(uk, v))
            {
                g->priority(v) = g->priority(uk) + g->weight(uk, v);
                g->parent(v) = uk;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    int v[] = { 0, 10, 2, 3, 4, 5, 6 };
    adjacency_list_graph<int, int> graph;

    for (int i = 0; i < sizeof(v) / 4; i++)
        graph.insert(v[i]);

    graph.insertEdge(2, 0, 1, 2);
    graph.insertEdge(10, 0, 1, 3);
    graph.insertEdge(11, 0, 1, 6);
    graph.insertEdge(12, 0, 2, 6);

    graph.insertEdge(22, 0, 2, 1);
    graph.insertEdge(100, 0, 3, 1);
    graph.insertEdge(118, 0, 6, 1);
    graph.insertEdge(120, 0, 6, 2);

    graph.show();

    graph.bfs(1);

    graph.remove(1);

    std::cout << "vertexes: " << graph.n << std::endl;
    std::cout << "edge: " << graph.e << std::endl;

    graph.show();

    graph.removeEdge(5, 1);

    std::cout << "vertexes: " << graph.n << std::endl;
    std::cout << "edge: " << graph.e << std::endl;

    graph.show();

    return 0;
}