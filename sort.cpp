#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>
#include <cassert>

#define SIZE 200000000

int numbers[SIZE];

void numbers_initialize()
{
	//std::srand(time(NULL));

	//for (int i = 0; i < SIZE; i++)
	//{
	//	int tmp = rand() % 100 + 1;
	//	numbers[i] = tmp;
	//}
	//std::random_device rd;
	std::mt19937 mt;
	mt.seed(std::random_device()());

	std::default_random_engine generator(time(NULL));

	std::uniform_int_distribution<std::mt19937::result_type> dist(0, 90000);

	for (int i = 0; i < SIZE; i++)
		numbers[i] = dist(generator);
}

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

void print()
{
	for (int i = 0; i < 500; i++)
	{
		std::cout << numbers[i] << ", ";
		if (i % 10 == 0 && i != 0)
			std::cout << std::endl;
	}
}

void bubblesort()
{
	bool sorted = false;
	while (!sorted)
	{
		sorted = true;
		for (int pos = 1; pos < SIZE; pos++)
		{
			if (numbers[pos - 1] > numbers[pos])
			{
				std::swap(numbers[pos - 1], numbers[pos]);
				sorted = false;
			}
		}
	}
}

void selection()
{
	for (int i = 0; i < SIZE; i++)
	{
		int min = i;
		for (int j = i + 1; j < SIZE; j++)
		{
			if (numbers[j] < numbers[min])
				min = j;
			std::swap(numbers[i], numbers[min]);
		}
	}
}

// 低效的插入排序
void inefficient_insertion()
{
	for (int i = 1; i < SIZE; i++)
		for (int j = i; j > 0 && (numbers[j] < numbers[j - 1]); j--)
			std::swap(numbers[j], numbers[j - 1]);
}

// 高效的插入排序
// 对比低效的插入排序，此版本大大减少了数据交换的次数
void efficient_insertion()
{
	for (int i = 1; i < SIZE; i++) // 类比扑克牌，i所代表的是新摸到、待插入的第i张牌(手里已经有i-1张有序牌)
	{
		int key = numbers[i];
		int j = i;
		for (; j > 0 && (key < numbers[j - 1]); j--) // j所代表的是手里已有的有序牌组，那么将新牌按顺序依次与手牌比对
			numbers[j] = numbers[j - 1]; // 若小于某张牌，则将较大的手牌挪一个位置
		numbers[j] = key; // 确认最终插入位置后，将牌插入其中
	}
}

void efficient_insertion(int* vals, int len)
{
	for (int i = 1; i < len; i++) // 类比扑克牌，i所代表的是新摸到、待插入的第i张牌(手里已经有i-1张有序牌)
	{
		int key = vals[i];
		int j = i;
		for (; j > 0 && (key < vals[j - 1]); j--) // j所代表的是手里已有的有序牌组，那么将新牌按顺序依次与手牌比对
			vals[j] = vals[j - 1]; // 若小于某张牌，则将较大的手牌挪一个位置
		vals[j] = key; // 确认最终插入位置后，将牌插入其中
	}
}

// 希尔排序
// 一定规模时，速度远快于插入排序
void shell()
{
	int h = 1;
	while (h < SIZE / 3) 
		h = h * 3 + 1;

	while (h >= 1)
	{
		for (int i = h; i < SIZE; i++)
			for (int j = i; j >= h && (numbers[j] < numbers[j - h]); j -= h)
				std::swap(numbers[j], numbers[j - h]);
		h = h / 3;
	}
}

// 归并抽象方法
inline void merge(int* pArray, int lo, int mid, int hi, int* tmp)
{
	int i = lo;
	int j = mid + 1;

	// 每次复制整个数组会浪费大量时间
	//std::copy(numbers, numbers + SIZE, tmp);

	// 关于k <= hi,因为归并左右区间均为闭区间[lo, mid][mid+1, hi]
	for (int k = lo; k <= hi; k++)
		if (i > mid)				
			pArray[k] = tmp[j++];
		else if (j > hi)			
			pArray[k] = tmp[i++];
		else if (tmp[i] > tmp[j])	
			pArray[k] = tmp[j++];
		else						
			pArray[k] = tmp[i++];

	// 归并后只复制改变部分，大大降低复制时间
	std::copy(pArray + lo, pArray + hi + 1, tmp + lo);
}

void merge_sort(int* p, int lo, int hi, int* tmp)
{
	if (lo >= hi)
		return;

	// 进一步优化，当区间较小时改用插入排序，可以做到2秒内一千万个数排序,release下可以0.9s内完成
	if ((hi - lo) < 50)
	{
		efficient_insertion(p + lo, hi - lo + 1);
		std::copy(p + lo, p + hi + 1, tmp + lo);
		return;
	}

	int mid = lo + (hi - lo) / 2;

	merge_sort(p, lo, mid, tmp);
	merge_sort(p, mid + 1, hi, tmp);
	merge(p, lo, mid, hi, tmp);
}

void mergeDown_sort()
{
	int* tmp = new int[SIZE];
	std::copy(numbers, numbers + SIZE, tmp);
	merge_sort(numbers, 0, SIZE - 1, tmp);
	delete[] tmp;
}

void mergeUP_sort()
{
	int* tmp = new int[SIZE];
	std::copy(numbers, numbers + SIZE, tmp);
	for (int sz = 1; sz < SIZE; sz += sz)
		for (int lo = 0; lo < SIZE - sz; lo += sz + sz)
			merge(numbers, lo, lo + sz - 1, std::min(lo + sz + sz - 1, SIZE - 1), tmp);
	delete tmp;
}

int partition(int* a, int lo, int hi)
{
	int v = a[lo];
	int i = lo;
	int j = hi + 1;

	while (true)
	{
		while (v > a[++i]) if (i == hi) break;
		while (v < a[--j]);// if (j == lo) break;
		if (i >= j)
			break;
		std::swap(a[i], a[j]);
	}
	std::swap(a[lo], a[j]);
	return j;
}

// 快速排序
// Debug模式下体现不出性能，release下一千万个数排序能做到0.6s
// 标准库的sort算法，同样在release下性能大幅提高，速度和快速排序几乎一样
void quick(int* arr, int lo, int hi)
{
	if (hi <= lo + 10)
	{	
		efficient_insertion(arr + lo, hi - lo + 1);
		return;
	}

	int p = partition(arr, lo, hi);
	quick(arr, lo, p - 1);
	quick(arr, p + 1, hi);
}

// 优先级队列
template <typename T>
class MaxPQ
{
public:
	MaxPQ(int Max) :N(0), max_size(Max)
	{
		pq.push_back(T());
	}
	~MaxPQ()
	{
		//if (pq != nullptr)
			//delete[] pq;
	}

	bool isEmpty()
	{
		return N == 0;
	}

	int size()
	{
		return N;
	}

	// release模式下，一千万个数插入需要~0.4s快于快速排序，
	// 但是它并不是排序，只是将元素按完全二叉树重新排列。当需要取出最大的M个数时，可以再很短时间内完成树的调整并返回需要的值
	void insert(T v)
	{
		pq.push_back(v);
		N++;
		swim(N);
	}

	T delMax()
	{
		T max = pq[1];
		std::swap(pq[1], pq[N]);
		N--;
		sink(1);
		pq.pop_back();
		
		return max;
	}

	void push_back(T v)
	{
		pq.push_back(v);
		N++;
	}

	// 堆排序
	void sort()
	{
		// 关于i>=1，由于采用的是下沉(sink)操作，因此对第一个元素同样有必要比较它的两个叶子节点
		for (int i = N / 2; i >= 1; i--)
			sink(i);
		while (N > 1)
		{
			std::swap(pq[1], pq[N--]);
			sink(1);
		}
	}

	void print(int len = 100)
	{
		if (!(len > 0 && len < pq.size()))
			return;
		for (int i = 0; i < len; i++)
			std::cout << pq[i] << ", ";
	}

private:
	// 上浮
	void swim(int i)
	{
		while (i > 1 && (pq[i] > pq[i / 2]))
		{
			std::swap(pq[i], pq[i / 2]);
			i = i / 2;
		}
	}

	// 下沉
	void sink(int i)
	{
		while (2 * i <= N)
		{
			int j = i * 2;
			//  j<N这条判据的理由，当最末元素为左子节点时，若不加判断j+1的访问会越界
			if (j < N && pq[j] < pq[j + 1])
				j++;
			if (pq[i] > pq[j])
				break;
			std::swap(pq[i], pq[j]);
			i = j;
		}
	}

private:
	std::vector<T> pq;
	int N;
	int max_size;
};

int _main(int argc, char *argv[])
{
	
	//numbers_initialize();
	////print();
	//{
	//	Speed speed;
	//	
	//	quick(numbers, 0, SIZE - 1);
	//}

	//print();

	//numbers_initialize();
	//{
	//	Speed speed;
	//	std::sort(numbers, numbers + SIZE - 1);
	//	//mergeDown_sort();
	//	//bubblesort();
	//	//mergeUP_sort();
	//}

	MaxPQ<int> pq(10000000);

	srand(time(NULL));

	{
		//Speed speed;
		for (int i = 0; i < 10000000; i++)
			pq.push_back(rand() % 50000 + 1);
	}
	
	pq.print();
	
	{
		Speed speed;
		pq.sort();
	}
	pq.print();
	//int max = pq.delMax();
	std::cout << std::endl;

	return 0;
}