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

// ��Ч�Ĳ�������
void inefficient_insertion()
{
	for (int i = 1; i < SIZE; i++)
		for (int j = i; j > 0 && (numbers[j] < numbers[j - 1]); j--)
			std::swap(numbers[j], numbers[j - 1]);
}

// ��Ч�Ĳ�������
// �Աȵ�Ч�Ĳ������򣬴˰汾�����������ݽ����Ĵ���
void efficient_insertion()
{
	for (int i = 1; i < SIZE; i++) // ����˿��ƣ�i�����������������������ĵ�i����(�����Ѿ���i-1��������)
	{
		int key = numbers[i];
		int j = i;
		for (; j > 0 && (key < numbers[j - 1]); j--) // j����������������е��������飬��ô�����ư�˳�����������Ʊȶ�
			numbers[j] = numbers[j - 1]; // ��С��ĳ���ƣ��򽫽ϴ������Ųһ��λ��
		numbers[j] = key; // ȷ�����ղ���λ�ú󣬽��Ʋ�������
	}
}

void efficient_insertion(int* vals, int len)
{
	for (int i = 1; i < len; i++) // ����˿��ƣ�i�����������������������ĵ�i����(�����Ѿ���i-1��������)
	{
		int key = vals[i];
		int j = i;
		for (; j > 0 && (key < vals[j - 1]); j--) // j����������������е��������飬��ô�����ư�˳�����������Ʊȶ�
			vals[j] = vals[j - 1]; // ��С��ĳ���ƣ��򽫽ϴ������Ųһ��λ��
		vals[j] = key; // ȷ�����ղ���λ�ú󣬽��Ʋ�������
	}
}

// ϣ������
// һ����ģʱ���ٶ�Զ���ڲ�������
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

// �鲢���󷽷�
inline void merge(int* pArray, int lo, int mid, int hi, int* tmp)
{
	int i = lo;
	int j = mid + 1;

	// ÿ�θ�������������˷Ѵ���ʱ��
	//std::copy(numbers, numbers + SIZE, tmp);

	// ����k <= hi,��Ϊ�鲢���������Ϊ������[lo, mid][mid+1, hi]
	for (int k = lo; k <= hi; k++)
		if (i > mid)				
			pArray[k] = tmp[j++];
		else if (j > hi)			
			pArray[k] = tmp[i++];
		else if (tmp[i] > tmp[j])	
			pArray[k] = tmp[j++];
		else						
			pArray[k] = tmp[i++];

	// �鲢��ֻ���Ƹı䲿�֣���󽵵͸���ʱ��
	std::copy(pArray + lo, pArray + hi + 1, tmp + lo);
}

void merge_sort(int* p, int lo, int hi, int* tmp)
{
	if (lo >= hi)
		return;

	// ��һ���Ż����������Сʱ���ò������򣬿�������2����һǧ���������,release�¿���0.9s�����
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

// ��������
// Debugģʽ�����ֲ������ܣ�release��һǧ���������������0.6s
// ��׼���sort�㷨��ͬ����release�����ܴ����ߣ��ٶȺͿ������򼸺�һ��
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

// ���ȼ�����
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

	// releaseģʽ�£�һǧ�����������Ҫ~0.4s���ڿ�������
	// ����������������ֻ�ǽ�Ԫ�ذ���ȫ�������������С�����Ҫȡ������M����ʱ�������ٺܶ�ʱ����������ĵ�����������Ҫ��ֵ
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

	// ������
	void sort()
	{
		// ����i>=1�����ڲ��õ����³�(sink)��������˶Ե�һ��Ԫ��ͬ���б�Ҫ�Ƚ���������Ҷ�ӽڵ�
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
	// �ϸ�
	void swim(int i)
	{
		while (i > 1 && (pq[i] > pq[i / 2]))
		{
			std::swap(pq[i], pq[i / 2]);
			i = i / 2;
		}
	}

	// �³�
	void sink(int i)
	{
		while (2 * i <= N)
		{
			int j = i * 2;
			//  j<N�����оݵ����ɣ�����ĩԪ��Ϊ���ӽڵ�ʱ���������ж�j+1�ķ��ʻ�Խ��
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