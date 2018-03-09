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

// 实现一遍LSD发现里面有些坑，为了避免这些坑饶了一大圈，但真正的实现却十分简洁精巧，恰到好处的避过了所有陷阱
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

// LSD 低位优先排序算法
// 正确的LSD算法格式
void LSD(std::vector<std::string>& strings, int W)
{
	int N = strings.size();
	const int R = 26; //字符集大小，此练习中随机生成的字符串由小写字母组成，当然也可以直接设置为128(标准asicc表大小)或256(扩展asicc字符集大小)
	std::vector<std::string> tmp(N);

	for (int d = W - 1; d >= 0; d--)
	{
		int count[R + 1] = { 0 };
		for (int i = 0; i < N; i++)
			count[strings[i].at(d) - 'a' + 1]++; // 统计当前字符数量
		for (int r = 0; r < R; r++)
			count[r + 1] += count[r]; // 计算排序后的起始位置
		for (int i = 0; i < N; i++)
			tmp[count[strings[i].at(d) - 'a']++] = strings[i]; // 按顺序复制到临时变量中
		for (int i = 0; i < N; i++) 
			strings[i] = tmp[i]; // 回写strings
	}
}

// 高位优先排序算法
// 缺点：
//（1）当字符串数量很大是，需要引入插入排序对小数组排序
// (2) 对于重复字符串处理效率很低
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
		// 此优化可以大幅提高算法性能
		if (hi <= lo + MAX_CUT)
		{
			insertion(a, lo, hi, d);
			return;
		}

		// 关于这里为什么+2。
		// 在LSD算法中+1是为了将排序后的各个插入起始位置往后挪一位，如若不然，
		// 接下来将a复制到aux，将会从终止插入位置复制，而不是起始位置，
		// 自然的这里+2也有相同的考虑。另外一个1是为了考虑一些字符串长度小于d，
		// 在这里+1，这就相当于在最小字符(这里是'a')之前增加一个位置，用以统计长度小于d的字符串数量
		// 当然这也合情合理，短的字符串放在前面。
		int count[26 + 2] = { 0 };
		for (int i = lo; i <= hi; i++)
			count[charAt(a[i], d) - 'a' + 2]++; // 统计当前字符数量

		// 这里+1乍看像是把字符集R扩充了一位，这恰恰体现出上面所说的新增一个位置，用于统计长度小于d的字符串
		for (int r = 0; r < R + 1; r++)
			count[r + 1] += count[r]; // 计算排序后的起始位置

		// 在MSD算法中aux临时变量的作用体现的更为深刻，也更加精妙！
		// 它总会把新的顺序从零位置开始排列，巧妙地是这样虽然有可能覆盖已排序部分字符串，
		// 但是也无关紧要，因为已排序部分加上当前范围(lo,hi)总是大于当前范围，而已经排序的部分之后不会再用到，
		// 当然也不会影响到未排序部分。
		for (int i = lo; i <= hi; i++)
			aux[count[charAt(a[i], d) - 'a' + 1]++] = a[i]; // 按顺序复制到临时变量中

		for (int i = lo; i <= hi; i++)
			a[i] = aux[i - lo]; // 回写strings

		// 经历前面的过程后，按当前d位置排序完成，但count的元素可能会呈现阶梯状（111122444）
		// 每个梯度的范围的值表示该范围内有多少字符串位置d字符相同
		// 下面的循环就是遍历每一个梯度范围(lo, hi)，以下一个位置(d+1)字符进行排序
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