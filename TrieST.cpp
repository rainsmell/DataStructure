#include <iostream>
#include <vector>
#include <string>

#define R 26

class Node
{
public:
	Node():next(std::vector<Node*>(R))
	{
		val = 0;
		for (int i = 0; i < R; i++)
			next[i] = nullptr;
	}

	~Node()
	{
		for (int i = 0; i < R; i++)
			if (next[i] != nullptr)
				delete next[i];
	}

	int val;
	std::vector<Node*> next;
};

class TrieST
{
public:
	TrieST() :root(nullptr) {}

	int get(std::string& key)
	{
		Node* x = get(root, key, 0);
		if (x == nullptr) return -1;
		return x->val;
	}

	Node* get(Node* x, std::string& key, int d)
	{
		if (x == nullptr) return nullptr;
		if (key.size() == d) return x;
		return get((Node*)(x->next[charAt(key, d)]), key, d + 1);
	}

	void put(std::string& key, int val)
	{
		put(root, key, val, 0);
	}

	// 指针的引用
	void put(Node*& x, const std::string& key, int val, int d)
	{
		if (x == nullptr) x = new Node();
		if (key.size() == d) { x->val = val; return; }
		put((Node*)(x->next[charAt(key, d)]), key, val, d + 1);
	}

	//void put(std::string& key, int val)
	//{
	//	put(&root, key, val, 0);
	//}

	//// 因为要是要修改原指针的值，所以这里需要传递指针的指针
	//// 不然的话所有的操作都作用在临时对象x上
	//void put(Node** const x, std::string& key, int val, int d)
	//{
	//	if (*x == nullptr) 
	//		*x = new Node();
	//	if (d == key.size()) 
	//	{
	//		(*x)->val = val; return;
	//	}
	//	int c = charAt(key, d);
	//	put((Node**)&((*x)->next[c]), key, val, d + 1);
	//}

	std::vector<std::string> keys()
	{
		return keysWithPrefix("");
	}

	std::vector<std::string> keysWithPrefix(std::string pre)
	{
		std::vector<std::string> q;
		collect(get(root, pre, 0), pre, q);
		return q;
	}

	std::vector<std::string> keysThatMatch(std::string pat)
	{
		std::vector<std::string> q;
		collect(root, "", pat, q);
		return q;
	}

	std::string longestPrefixOf(std::string s)
	{
		int length = search(root, s, 0, 0);
		return s.substr(0, length);
	}

	void remove(std::string key)
	{
		remove(root, key, 0);
	}
	
	int size()
	{
		return size(root);
	}

	int size(const Node* n)
	{
		if (n == nullptr) return 0;
		int count = 0;
		if (n->val != 0) count++;
		for (int r = 0; r < R; r++)
			count += size((Node*)n->next[r]);
		return count;
	}

	~TrieST()
	{
		if (root != nullptr)
			delete root;
	}

	Node* root;

private:
	int charAt(const std::string& str, int p) { return (str.at(p) - 'a'); }

	void collect(Node* x, std::string& pre, std::vector<std::string>& q)
	{
		if (x == nullptr) return;
		if (x->val != 0) q.push_back(pre);
		for (char c = 0; c < R; c++)
			collect(x->next[c], pre + char(c + 'a'), q);
	}

	void collect(Node* x, std::string pre, std::string pat, std::vector<std::string>& q)
	{
		int d = pre.size();
		if (x == nullptr) return;
		if (d == pat.size() && (x->val != 0)) q.push_back(pre);
		if (d == pat.size()) return;

		char next = charAt(pat, d) + 'a';
		for (char c = 0; c < R; c++)
			if (next == '.' || next == c + 'a')
				collect(x->next[c], pre + char(c + 'a'), pat, q);
	}

	int search(Node* x, std::string s, int d, int length)
	{
		if (x == nullptr) return length;
		if (x->val != 0) length = d;
		if (s.size() == d) return length;
		return search(x->next[charAt(s, d)], s, d + 1, length);
	}

	void remove(Node*& x, std::string& key, int d)
	{
		if (x == nullptr) return;
		if (d == key.size())
			x->val = 0;
		else
			remove(x->next[d], key, d + 1);

		if (x->val != 0)
			return;
		for (int r = 0; r < R; r++)
			if (x->next[r] != nullptr)
				return;
		delete x;
		x = nullptr;
	}
};

void foo( int** const ppt)
{
	if (*ppt == nullptr)
		*ppt = new int(20);
	int *p = new int(30);
	*ppt = p;
}

int main_(int argc, char* argv[])
{
	std::string s1("abcdff");
	std::string s2("abcd");
	std::string s3("abc");

	TrieST trie;
	trie.put(s1, 1);
	trie.put(s2, 11);
	trie.put(s3, 111);

	std::string s = trie.longestPrefixOf("abcdf");

	trie.remove("abc");
	int r = trie.get(std::string("abc"));

	std::cout << r << std::endl;

	r = trie.get(std::string("abcd"));
	std::cout << r << std::endl;

	return 0;
}