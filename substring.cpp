#include <iostream>
#include <string>

#define R 26

// 暴力搜索版本一
static int search1(std::string pat, std::string text)
{
	int M = pat.size();
	int N = text.size();

	for (int i = 0; i <= N - M; i++)
	{
		int j;
		for (j = 0; j < M; j++)
			if (pat[j] != text[i + j])
				break;
		if (j == M)
			return i;
	}
	return N;
}

// 暴力搜索版本二
static int search2(std::string pat, std::string text)
{
	int i, N = text.size();
	int j, M = pat.size();

	for (i = 0; i < N && j < M; i++)
	{
		if (pat[j] == text[i])
			j++;
		else { i -= j; j = 0; }
	}

	if (j == M) return (i - M);
	else return N;
}

int charAt(const std::string& str, const int i)
{
	return str.at(i) - 'a';
}

// 确定有限状态自动机(DFA)生成算法，精巧复杂！！！
// 对pat字符串预处理
void DFA(int* (&dfa)[R], std::string pat, int w)
{
	dfa[charAt(pat, 0)][0] = 1;
	for (int x = 0, j = 1; j < w; j++)
	{
		for (int r = 0; r < R; r++)
			dfa[r][j] = dfa[r][x];
		dfa[charAt(pat, j)][j] = j + 1;
		x = dfa[charAt(pat, j)][x];
	}
}

// Knuth-Morris-Pratt查找算法
// 优点在于不用回退文本指针，每次只需比较一个字符，搜索最多M+N次比较
int KMP(std::string pat, std::string text)
{
	int i, M = pat.size();
	int j, N = text.size();
	int *dfa[R] = { nullptr };

	int w = pat.size();

	for (int k = 0; k < R; k++)
	{
		dfa[k] = new int[w];
		memset(dfa[k], 0, sizeof(int) * w);
	}

	DFA(dfa, pat, w);

	for (j = 0, i = 0; j < N && i < M; j++)
		i = dfa[charAt(text, j)][i];

	for (int k = 0; k < R; k++)
		delete [] dfa[k];

	if (i == M)
		return (j - M);
	return N;
}

// 用于Boyer-Moore算法的跳跃表计算
void SKIP(const std::string& pat, int* right)
{
	int n = pat.size();
	for (int i = 0; i < n; i++)
		right[charAt(pat, i)] = i;
}

int BM(std::string txt, std::string pat)
{
	int right[R] = { -1 };
	int N = txt.size();
	int M = pat.size();
	int skip = 0;

	memset(right, -1, sizeof(int));

	SKIP(pat, right);

	for (int i = 0; i < N; i += skip)
	{
		skip = 0;
		for (int j = M - 1; j >= 0; j--)
			if (charAt(txt, i + j) != charAt(pat, j))
			{
				skip = j - right[charAt(txt, i + j)]; 
				// 出现skip小于1的情况：比较的两个字符出现逆序，
				// txt的当前字符在pat中第一次出现位置早于pat[j]
				// 故只能做最小步进
				if (skip < 1)
					skip = 1;
				break;
			}
		if (skip == 0)
			return i;
	}

	return N;
}

int main(int argc, char* argv[])
{
	int i = BM("ababaaaaaababacc", "ababac");
	return 0;
}