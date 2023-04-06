#include <iostream>


// 1. Implement Binary Search using loop
int BinarySearch(int arr[], int l, int h, int key)
{
	// 0  1  2  3  4  5  6  7  8
	// 1, 2, 3, 4, 5, 6, 7, 8, 9
	// k = 7
	// k = 2
	// k = 0
	// k = 14

	int low = l;
	int high = h;

	while (low < high)
	{
		int mid = (low + high) / 2;

		if (key == arr[mid])
			return mid;
		else if (key < arr[mid])
			high = mid - 1;
		else
			low = mid + 1;
	}

	return -1;
}

// 2. Implement Binary Search using Recursion


int main()
{
	int i { -1 };
	int k { 0 };
	int arr[] { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	int size = sizeof arr / sizeof(arr[0]); // Double check how sizeof works

	i = -1;
	std::cout << "Enter a element to search: ";
	std::cin >> k;

	if (-1 != (i = BinarySearch(arr, 0, size-1, k)))
		std::cout << "key : " << k << " found at index " << i << std::endl;
	else
		std::cout << "key not found" << std::endl;

	return 0;
}
