#include <iostream>

using namespace std;

string decimal_to_binary(int n) {
    if (n == 0) return "0";

    string binary;
    while (n > 0) {
        binary += (n % 2) ? '1' : '0';
        n /= 2;
    }

    reverse(binary.begin(), binary.end());
    return binary;
}

class timsort {
private:
    struct Run {
        int start;
        int length;
        Run() : start(0), length(0) {}
        Run(int s, int l) : start(s), length(l) {}
    };

    static const int MAX_RUNS = 100;
    static const int MIN_GALLOP = 7;

    static int gallop_binary_search(int arr[], int key, int low, int high, bool find_right) {
        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (key < arr[mid] || (find_right && key == arr[mid])) {
                high = mid - 1;
            }
            else {
                low = mid + 1;
            }
        }
        return low;
    }

    static int gallop(int arr[], int key, int start, int length, bool find_right) {
        int last_offset = 0;
        int offset = 1;

        int max_offset = start + length;
        while (start + offset < max_offset &&
            (find_right ? (arr[start + offset] <= key) : (arr[start + offset] < key))) {
            last_offset = offset;
            offset = (offset << 1) + 1; 
            if (offset <= 0) offset = max_offset;  
        }

        if (offset > max_offset) offset = max_offset;

        return gallop_binary_search(arr, key, start + last_offset, start + offset - 1, find_right);
    }

    static void merge_with_gallop(int arr[], int l, int m, int r) {
        int n1 = m - l + 1;
        int n2 = r - m;

        int* left = new int[n1];
        int* right = new int[n2];

        for (int i = 0; i < n1; i++)
            left[i] = arr[l + i];
        for (int j = 0; j < n2; j++)
            right[j] = arr[m + 1 + j];

        int i = 0, j = 0, k = l;
        int left_count = 0, right_count = 0;

        while (i < n1 && j < n2) {
            if (left_count >= MIN_GALLOP || right_count >= MIN_GALLOP) {
                if (left_count >= MIN_GALLOP) {
                    int gallop_pos = gallop(right, left[i], j, n2 - j, true);
                    int elements_to_copy = gallop_pos - j;

                    for (int x = 0; x < elements_to_copy; x++) {
                        arr[k++] = right[j++];
                    }
                    right_count += elements_to_copy;
                    left_count = 0;
                }
                else {
                    int gallop_pos = gallop(left, right[j], i, n1 - i, false);
                    int elements_to_copy = gallop_pos - i;

                    for (int x = 0; x < elements_to_copy; x++) {
                        arr[k++] = left[i++];
                    }
                    left_count += elements_to_copy;
                    right_count = 0;
                }
                continue;
            }

            if (left[i] <= right[j]) {
                arr[k++] = left[i++];
                left_count++;
                right_count = 0;
            }
            else {
                arr[k++] = right[j++];
                right_count++;
                left_count = 0;
            }
        }

        while (i < n1) {
            arr[k++] = left[i++];
        }

        while (j < n2) {
            arr[k++] = right[j++];
        }

        delete[] left;
        delete[] right;
    }

    static void merge_collapse(int arr[], Run runs[], int& runs_count) {
        while (runs_count > 1) {
            int n = runs_count - 1;

            if (n >= 1 && runs[n - 1].length <= runs[n].length) {
                merge_runs(arr, runs, runs_count, n - 1, n);
            }

            else if (n >= 2 && runs[n - 2].length <= runs[n - 1].length + runs[n].length) {
                if (runs[n - 2].length < runs[n].length) {
                    merge_runs(arr, runs, runs_count, n - 2, n - 1);
                }
                else {
                    merge_runs(arr, runs, runs_count, n - 1, n);
                }
            }
            else {
                break;
            }
        }
    }

    static void merge_runs(int arr[], Run runs[], int& runs_count, int i, int j) {
        cout << "Сливаем раны: [" << runs[i].start << "-" << runs[i].start + runs[i].length - 1
            << "] и [" << runs[j].start << "-" << runs[j].start + runs[j].length - 1 << "]" << endl;

        int start1 = runs[i].start;
        int end1 = runs[i].start + runs[i].length - 1;
        int start2 = runs[j].start;
        int end2 = runs[j].start + runs[j].length - 1;

        merge_with_gallop(arr, start1, end1, end2);

        runs[i].length = runs[i].length + runs[j].length;

        for (int k = j; k < runs_count - 1; k++) {
            runs[k] = runs[k + 1];
        }
        runs_count--;
    }

public:
    static int compute_minrun(int n) {
        int number_of_bits = decimal_to_binary(n).length();
        cout << "n = " << n << " (двоичное: " << decimal_to_binary(n) << ")" << endl;

        int high6bits = n >> (number_of_bits - 6);
        cout << "Старшие 6 бит: " << decimal_to_binary(high6bits) << " = " << high6bits << endl;

        int shift_amount = number_of_bits - 6;
        int low6bits = n & ((1 << shift_amount) - 1);
        cout << "Оставшиеся биты: " << decimal_to_binary(low6bits) << " = " << low6bits << endl;

        bool has_ones = (low6bits != 0);
        int minrun = high6bits + (has_ones ? 1 : 0);
        cout << "minrun = " << high6bits << " + " << (has_ones ? 1 : 0) << " = " << minrun << endl;

        if (minrun < 32) {
            minrun = 32;
            cout << "Корректируем до " << minrun << " (слишком мало)" << endl;
        }
        if (minrun > 64) {
            minrun = 64;
            cout << "Корректируем до " << minrun << " (слишком много)" << endl;
        }

        return minrun;
    }

    static int find_run(int arr[], int n, int start) {
        if (start >= n - 1) return n;

        int i = start + 1;

        if (arr[start] > arr[i]) {

            while (i < n && arr[i - 1] > arr[i]) {
                i++;
            }
            for (int j = start, k = i - 1; j < k; j++, k--) {
                int temp = arr[j];
                arr[j] = arr[k];
                arr[k] = temp;
            }
        }
        else {
            while (i < n && arr[i - 1] <= arr[i]) {
                i++;
            }
        }

        return i;
    }

    static void insertion_sort(int arr[], int left, int right) {
        for (int i = left + 1; i <= right; ++i) {
            int key = arr[i];
            int j = i - 1;
            while (j >= left && arr[j] > key) {
                arr[j + 1] = arr[j];
                --j;
            }
            arr[j + 1] = key;
        }
    }

    static void sort(int arr[], int n) {
        if (n <= 1) return;

        int minrun = compute_minrun(n);
        cout << "Размер массива: " << n << ", Minrun: " << minrun << endl;

        Run runs[MAX_RUNS];
        int runs_count = 0;
        int current = 0;

        while (current < n) {
            int run_start = current;
            current = find_run(arr, n, run_start);
            int run_length = current - run_start;

            cout << "Найден run: [" << run_start << "-" << current - 1 << "], длина: " << run_length << endl;

            if (run_length < minrun) {
                int new_end = (run_start + minrun < n) ? run_start + minrun : n;
                cout << "Расширяем до " << new_end << " элементов" << endl;

                insertion_sort(arr, run_start, new_end - 1);

                run_length = new_end - run_start;
                current = new_end;
            }

            if (runs_count < MAX_RUNS) {
                runs[runs_count] = Run(run_start, run_length);
                runs_count++;
                cout << "Добавили в стек. Всего run: " << runs_count << endl;
            }

            merge_collapse(arr, runs, runs_count);
        }

        while (runs_count > 1) {
            merge_runs(arr, runs, runs_count, runs_count - 2, runs_count - 1);
        }

    }
};

int main() {
    setlocale(0, "");

    int arr[] = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19,  
             2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
    int n = sizeof(arr) / sizeof(arr[0]);

    cout << "Исходный массив: ";
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    timsort::sort(arr, n);

    cout << "\nОтсортированный массив: ";
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    return 0;
}
