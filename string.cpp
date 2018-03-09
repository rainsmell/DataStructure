#include <iostream>
#include <random>
#include <ctime>
#include <string>
#include <vector>
#include <chrono>

struct Speed
{
	typedef std::chrono::steady_clock::time_point time_point;

	Speed()
	{
		start_time = std::chrono::high_resolution_clock::now();
	}

	~Speed()
	{
		auto end_time = std::chrono::high_resolution_clock::now();
		auto time = end_time - start_time;

		std::cout << "it tooks "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(time).count()
			<< "ms to run.\n";
	}

	time_point start_time;
};

std::string gen_random_string(int len)
{
	//std::default_random_engine generator(time(NULL));
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dis(97, 122); /* 'a' - 'z' */
	std::string str;

	for (int i = 0; i < len; i++)
		str.push_back(dis(mt));

	return str;
}

// ʵ��һ��LSD����������Щ�ӣ�Ϊ�˱�����Щ������һ��Ȧ����������ʵ��ȴʮ�ּ�ྫ�ɣ�ǡ���ô��ıܹ�����������
void LSD_sort(std::vector<std::string>& strings, int len)
{
	//int* counts = new int[27]; /* a - z */
	int counts[27];
	int pos[27];
	int size = strings.size();
	std::vector<std::string> tmp(size);

	while (len-- > 0)
	{
		memset(counts, 0, 27 * sizeof(int));
		for (int i = 0; i < size; i++)
			counts[strings[i].at(len) - 'a' + 1]++;
		//std::copy(counts, counts + 27, pos);
		for (int i = 1; i < 27; i++)
		{
			int index = i - 1;
			/*if (counts[i] == 0)
				continue;
			while (index > 0 && counts[index] == 0)
				--index;*/
			counts[i] += counts[index];
		}
		for (int i = 0; i < size; i++)
		{
			int p = counts[strings[i].at(len) - 'a']++;
			tmp[p] = strings[i];
		}
			
			
		for (int i = 0; i < size; i++)
			strings[i] = tmp[i];
	}
}

// LSD ��λ���������㷨
// ��ȷ��LSD�㷨��ʽ
void LSD(std::vector<std::string>& strings, int W)
{
	int N = strings.size();
	const int R = 26; //�ַ�����С������ϰ��������ɵ��ַ�����Сд��ĸ��ɣ���ȻҲ����ֱ������Ϊ128(��׼asicc���С)��256(��չasicc�ַ�����С)
	std::vector<std::string> tmp(N);

	for (int d = W - 1; d >= 0; d--)
	{
		int count[R + 1] = { 0 };
		for (int i = 0; i < N; i++)
			count[strings[i].at(d) - 'a' + 1]++; // ͳ�Ƶ�ǰ�ַ�����
		for (int r = 0; r < R; r++)
			count[r + 1] += count[r]; // ������������ʼλ��
		for (int i = 0; i < N; i++)
			tmp[count[strings[i].at(d) - 'a']++] = strings[i]; // ��˳���Ƶ���ʱ������
		for (int i = 0; i < N; i++) 
			strings[i] = tmp[i]; // ��дstrings
	}
}

// ��λ���������㷨
// ȱ�㣺
//��1�����ַ��������ܴ��ǣ���Ҫ������������С��������
// (2) �����ظ��ַ�������Ч�ʺܵ�
struct MSD
{
	enum { MAX_CUT = 10 };
	MSD(int size) :N(size), R(26)
	{
		aux = std::vector<std::string>(size);
	}

	int charAt(std::string& str, int d)
	{
		if (d < str.size())
			return str.at(d);
		else
			return ('a' - 1);
	}

	void sort(std::vector<std::string>& a, int lo, int hi, int d)
	{
		// ���Ż����Դ������㷨����
		if (hi <= lo + MAX_CUT)
		{
			insertion(a, lo, hi, d);
			return;
		}

		// ��������Ϊʲô+2��
		// ��LSD�㷨��+1��Ϊ�˽������ĸ���������ʼλ������Ųһλ��������Ȼ��
		// ��������a���Ƶ�aux���������ֹ����λ�ø��ƣ���������ʼλ�ã�
		// ��Ȼ������+2Ҳ����ͬ�Ŀ��ǡ�����һ��1��Ϊ�˿���һЩ�ַ�������С��d��
		// ������+1������൱������С�ַ�(������'a')֮ǰ����һ��λ�ã�����ͳ�Ƴ���С��d���ַ�������
		// ��Ȼ��Ҳ��������̵��ַ�������ǰ�档
		int count[26 + 2] = { 0 };
		for (int i = lo; i <= hi; i++)
			count[charAt(a[i], d) - 'a' + 2]++; // ͳ�Ƶ�ǰ�ַ�����

		// ����+1է�����ǰ��ַ���R������һλ����ǡǡ���ֳ�������˵������һ��λ�ã�����ͳ�Ƴ���С��d���ַ���
		for (int r = 0; r < R + 1; r++)
			count[r + 1] += count[r]; // ������������ʼλ��

		// ��MSD�㷨��aux��ʱ�������������ֵĸ�Ϊ��̣�Ҳ���Ӿ��
		// ���ܻ���µ�˳�����λ�ÿ�ʼ���У��������������Ȼ�п��ܸ��������򲿷��ַ�����
		// ����Ҳ�޹ؽ�Ҫ����Ϊ�����򲿷ּ��ϵ�ǰ��Χ(lo,hi)���Ǵ��ڵ�ǰ��Χ�����Ѿ�����Ĳ���֮�󲻻����õ���
		// ��ȻҲ����Ӱ�쵽δ���򲿷֡�
		for (int i = lo; i <= hi; i++)
			aux[count[charAt(a[i], d) - 'a' + 1]++] = a[i]; // ��˳���Ƶ���ʱ������

		for (int i = lo; i <= hi; i++)
			a[i] = aux[i - lo]; // ��дstrings

		// ����ǰ��Ĺ��̺󣬰���ǰdλ��������ɣ���count��Ԫ�ؿ��ܻ���ֽ���״��111122444��
		// ÿ���ݶȵķ�Χ��ֵ��ʾ�÷�Χ���ж����ַ���λ��d�ַ���ͬ
		// �����ѭ�����Ǳ���ÿһ���ݶȷ�Χ(lo, hi)������һ��λ��(d+1)�ַ���������
		for (int r = 0; r < R; r++)
			sort(a, lo + count[r], lo + count[r + 1] - 1, d + 1);
	}

	void insertion(std::vector<std::string>& a, int lo, int hi, int d)
	{
		for (int i = lo + 1; i <= hi; i++)
			for (int j = i; j > lo && (charAt(a[i], d) < charAt(a[j - 1], d)); j--)
				exch(a, i, j);
	}

	void exch(std::vector<std::string>& a, int i, int j)
	{
		std::string tmp = a[i];
		a[i] = a[j];
		a[j] = tmp;
	}
	std::vector<std::string> aux;
	const int R;
	int N;
};

int __main(int argc, char* argv[])
{
	std::vector<std::string> strings = { "she", "sells", "seashells", "by", "the", "sea", "shore", "the", "shells", "she", "sells", "are", "surely", "seashells"};

	MSD msd(strings.size());

	msd.sort(strings, 0, msd.N - 1, 0);

	for (auto str : strings)
		std::cout << str << std::endl;

	return 0;
}