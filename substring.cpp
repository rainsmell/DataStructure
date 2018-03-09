#include <iostream>
#include <string>

#define R 26

// ���������汾һ
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

// ���������汾��
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

// ȷ������״̬�Զ���(DFA)�����㷨�����ɸ��ӣ�����
// ��pat�ַ���Ԥ����
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

// Knuth-Morris-Pratt�����㷨
// �ŵ����ڲ��û����ı�ָ�룬ÿ��ֻ��Ƚ�һ���ַ����������M+N�αȽ�
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

// ����Boyer-Moore�㷨����Ծ�����
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
				// ����skipС��1��������Ƚϵ������ַ���������
				// txt�ĵ�ǰ�ַ���pat�е�һ�γ���λ������pat[j]
				// ��ֻ������С����
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