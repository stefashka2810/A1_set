#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <functional>
#include <map>
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iomanip>

// Класс для подсчета операций сравнения
class ComparisonCounter {
public:
  static long long characterComparisons;

  static void reset() {
    characterComparisons = 0;
  }

  static long long getCount() {
    return characterComparisons;
  }

  // Функция сравнения строк с подсчетом посимвольных сравнений
  static int compareStrings(const std::string& a, const std::string& b) {
    size_t minLen = std::min(a.length(), b.length());
    for (size_t i = 0; i < minLen; ++i) {
      characterComparisons++;
      if (a[i] < b[i]) return -1;
      if (a[i] > b[i]) return 1;
    }
    if (a.length() < b.length()) return -1;
    if (a.length() > b.length()) return 1;
    return 0;
  }

  // Вычисление длины общего префикса
  static int commonPrefixLength(const std::string& a, const std::string& b) {
    int len = 0;
    size_t minLen = std::min(a.length(), b.length());
    for (size_t i = 0; i < minLen; ++i) {
      characterComparisons++;
      if (a[i] != b[i]) break;
      len++;
    }
    return len;
  }
};

long long ComparisonCounter::characterComparisons = 0;

// Класс для генерации тестовых данных
class StringGenerator {
private:
  std::mt19937 rng;
  std::string alphabet;
  std::uniform_int_distribution<int> lengthDist;
  std::uniform_int_distribution<int> charDist;

public:
  StringGenerator() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
    // Алфавит: A-Z, a-z, 0-9, специальные символы
    alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#%:;^&*()-.";
    lengthDist = std::uniform_int_distribution<int>(10, 200);
    charDist = std::uniform_int_distribution<int>(0, alphabet.length() - 1);
  }

  std::string generateString() {
    int length = lengthDist(rng);
    std::string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i) {
      result += alphabet[charDist(rng)];
    }
    return result;
  }

  std::vector<std::string> generateRandomArray(int size) {
    std::vector<std::string> result;
    result.reserve(size);
    for (int i = 0; i < size; ++i) {
      result.push_back(generateString());
    }
    return result;
  }

  std::vector<std::string> generateReverseSortedArray(int size) {
    auto result = generateRandomArray(size);
    std::sort(result.begin(), result.end(), std::greater<std::string>());
    return result;
  }

  std::vector<std::string> generateNearlySortedArray(int size) {
    auto result = generateRandomArray(size);
    std::sort(result.begin(), result.end());

    // Делаем небольшое количество случайных перестановок (5% от размера)
    int swaps = std::max(1, size / 20);
    std::uniform_int_distribution<int> indexDist(0, size - 1);

    for (int i = 0; i < swaps; ++i) {
      int idx1 = indexDist(rng);
      int idx2 = indexDist(rng);
      std::swap(result[idx1], result[idx2]);
    }

    return result;
  }

  std::vector<std::string> generateArrayWithCommonPrefixes(int size, const std::string& prefix) {
    std::vector<std::string> result;
    result.reserve(size);

    for (int i = 0; i < size; ++i) {
      std::string str = prefix + generateString();
      result.push_back(str);
    }

    return result;
  }
};

// Класс для тестирования алгоритмов сортировки
class StringSortTester {
private:
  struct TestResult {
    double timeMs;
    long long comparisons;
    std::string algorithmName;
    int arraySize;
    std::string arrayType;
  };

  std::vector<TestResult> results;

  // Измерение времени выполнения функции
  template<typename Func>
  double measureTime(Func func, int iterations = 5) {
    double totalTime = 0;

    for (int i = 0; i < iterations; ++i) {
      auto start = std::chrono::high_resolution_clock::now();
      func();
      auto end = std::chrono::high_resolution_clock::now();

      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      totalTime += duration.count() / 1000.0; // Переводим в миллисекунды
    }

    return totalTime / iterations;
  }

public:
  // Стандартная быстрая сортировка
  void quickSort(std::vector<std::string>& arr, int low, int high) {
    if (low < high) {
      int pi = partition(arr, low, high);
      quickSort(arr, low, pi - 1);
      quickSort(arr, pi + 1, high);
    }
  }

  int partition(std::vector<std::string>& arr, int low, int high) {
    std::string pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
      if (ComparisonCounter::compareStrings(arr[j], pivot) <= 0) {
        i++;
        std::swap(arr[i], arr[j]);
      }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
  }

  // Стандартная сортировка слиянием
  void mergeSort(std::vector<std::string>& arr, int left, int right) {
    if (left < right) {
      int mid = left + (right - left) / 2;
      mergeSort(arr, left, mid);
      mergeSort(arr, mid + 1, right);
      merge(arr, left, mid, right);
    }
  }

  void merge(std::vector<std::string>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<std::string> leftArr(n1);
    std::vector<std::string> rightArr(n2);

    for (int i = 0; i < n1; ++i)
      leftArr[i] = arr[left + i];
    for (int j = 0; j < n2; ++j)
      rightArr[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
      if (ComparisonCounter::compareStrings(leftArr[i], rightArr[j]) <= 0) {
        arr[k] = leftArr[i];
        i++;
      }
      else {
        arr[k] = rightArr[j];
        j++;
      }
      k++;
    }

    while (i < n1) {
      arr[k] = leftArr[i];
      i++;
      k++;
    }

    while (j < n2) {
      arr[k] = rightArr[j];
      j++;
      k++;
    }
  }

  // Тернарная быстрая сортировка строк
  void ternaryStringQuickSort(std::vector<std::string>& arr, int low, int high, int d = 0) {
    if (high <= low) return;

    int lt = low, gt = high;
    int pivot = charAt(arr[low], d);
    int i = low + 1;

    while (i <= gt) {
      int t = charAt(arr[i], d);
      if (t < pivot) {
        std::swap(arr[lt++], arr[i++]);
      }
      else if (t > pivot) {
        std::swap(arr[i], arr[gt--]);
      }
      else {
        i++;
      }
    }

    ternaryStringQuickSort(arr, low, lt - 1, d);
    if (pivot >= 0) {
      ternaryStringQuickSort(arr, lt, gt, d + 1);
    }
    ternaryStringQuickSort(arr, gt + 1, high, d);
  }

  // Получение символа по позиции (или -1 если позиция вне строки)
  int charAt(const std::string& s, int d) {
    if (d < static_cast<int>(s.length())) {
      ComparisonCounter::characterComparisons++;
      return static_cast<unsigned char>(s[d]);
    }
    return -1;
  }

  // String MergeSort с учетом общих префиксов
  void stringMergeSort(std::vector<std::string>& arr, int left, int right) {
    if (left < right) {
      int mid = left + (right - left) / 2;
      stringMergeSort(arr, left, mid);
      stringMergeSort(arr, mid + 1, right);
      stringMerge(arr, left, mid, right);
    }
  }

  void stringMerge(std::vector<std::string>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<std::string> leftArr(n1);
    std::vector<std::string> rightArr(n2);

    for (int i = 0; i < n1; ++i)
      leftArr[i] = arr[left + i];
    for (int j = 0; j < n2; ++j)
      rightArr[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
      // Используем улучшенное сравнение с учетом префиксов
      if (ComparisonCounter::compareStrings(leftArr[i], rightArr[j]) <= 0) {
        arr[k] = leftArr[i];
        i++;
      }
      else {
        arr[k] = rightArr[j];
        j++;
      }
      k++;
    }

    while (i < n1) {
      arr[k] = leftArr[i];
      i++;
      k++;
    }

    while (j < n2) {
      arr[k] = rightArr[j];
      j++;
      k++;
    }
  }

  // MSD Radix Sort
  void msdRadixSort(std::vector<std::string>& arr, int low, int high, int d) {
    if (high <= low) return;

    const int R = 256; // Размер алфавита
    std::vector<int> count(R + 2, 0);
    std::vector<std::string> aux(high - low + 1);

    // Подсчет частот
    for (int i = low; i <= high; ++i) {
      count[charAt(arr[i], d) + 2]++;
    }

    // Префиксные суммы
    for (int r = 0; r < R + 1; ++r) {
      count[r + 1] += count[r];
    }

    // Распределение
    for (int i = low; i <= high; ++i) {
      aux[count[charAt(arr[i], d) + 1]++] = arr[i];
    }

    // Копирование обратно
    for (int i = low; i <= high; ++i) {
      arr[i] = aux[i - low];
    }

    // Рекурсивная сортировка для каждого символа
    for (int r = 0; r < R; ++r) {
      msdRadixSort(arr, low + count[r], low + count[r + 1] - 1, d + 1);
    }
  }

  // MSD Radix Sort с переключением на быструю сортировку
  void msdRadixSortWithCutoff(std::vector<std::string>& arr, int low, int high, int d) {
    const int CUTOFF = 74; // Размер алфавита

    if (high - low <= CUTOFF) {
      ternaryStringQuickSort(arr, low, high, d);
      return;
    }

    const int R = 256;
    std::vector<int> count(R + 2, 0);
    std::vector<std::string> aux(high - low + 1);

    for (int i = low; i <= high; ++i) {
      count[charAt(arr[i], d) + 2]++;
    }

    for (int r = 0; r < R + 1; ++r) {
      count[r + 1] += count[r];
    }

    for (int i = low; i <= high; ++i) {
      aux[count[charAt(arr[i], d) + 1]++] = arr[i];
    }

    for (int i = low; i <= high; ++i) {
      arr[i] = aux[i - low];
    }

    for (int r = 0; r < R; ++r) {
      msdRadixSortWithCutoff(arr, low + count[r], low + count[r + 1] - 1, d + 1);
    }
  }

  // Тестирование алгоритма
  void testAlgorithm(const std::string& algorithmName,
    std::function<void(std::vector<std::string>&)> sortFunc,
    const std::vector<std::string>& testData,
    const std::string& arrayType) {

    auto data = testData; // Копия для сортировки

    ComparisonCounter::reset();

    double time = measureTime([&]() {
      data = testData; // Восстанавливаем данные для каждого измерения
      ComparisonCounter::reset();
      sortFunc(data);
      });

    // Проверяем корректность сортировки
    assert(std::is_sorted(data.begin(), data.end()));

    TestResult result;
    result.algorithmName = algorithmName;
    result.timeMs = time;
    result.comparisons = ComparisonCounter::getCount();
    result.arraySize = testData.size();
    result.arrayType = arrayType;

    results.push_back(result);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << algorithmName << " (" << arrayType << ", size=" << testData.size()
      << "): " << time << "ms, " << result.comparisons << " comparisons\n";
  }

  // Проведение полного тестирования
  void runCompleteTest() {
    StringGenerator generator;
    std::vector<int> sizes = { 100, 200, 300, 500, 1000, 1500, 2000, 2500, 3000 };
    std::vector<std::string> arrayTypes = { "random", "reverse", "nearly_sorted", "common_prefix" };

    for (int size : sizes) {
      std::cout << "\n=== Testing with array size: " << size << " ===\n";

      // Генерируем тестовые данные
      auto randomData = generator.generateRandomArray(size);
      auto reverseData = generator.generateReverseSortedArray(size);
      auto nearlyData = generator.generateNearlySortedArray(size);
      auto prefixData = generator.generateArrayWithCommonPrefixes(size, "PREFIX_");

      std::vector<std::vector<std::string>> testArrays = {
          randomData, reverseData, nearlyData, prefixData
      };

      for (size_t i = 0; i < testArrays.size(); ++i) {
        const auto& testData = testArrays[i];
        const std::string& arrayType = arrayTypes[i];

        std::cout << "\n--- " << arrayType << " array ---\n";

        // Стандартные алгоритмы
        testAlgorithm("QuickSort", [this](std::vector<std::string>& arr) {
          quickSort(arr, 0, arr.size() - 1);
          }, testData, arrayType);

        testAlgorithm("MergeSort", [this](std::vector<std::string>& arr) {
          mergeSort(arr, 0, arr.size() - 1);
          }, testData, arrayType);

        // Специализированные алгоритмы
        testAlgorithm("Ternary String QuickSort", [this](std::vector<std::string>& arr) {
          ternaryStringQuickSort(arr, 0, arr.size() - 1);
          }, testData, arrayType);

        testAlgorithm("String MergeSort", [this](std::vector<std::string>& arr) {
          stringMergeSort(arr, 0, arr.size() - 1);
          }, testData, arrayType);

        testAlgorithm("MSD Radix Sort", [this](std::vector<std::string>& arr) {
          msdRadixSort(arr, 0, arr.size() - 1, 0);
          }, testData, arrayType);

        testAlgorithm("MSD Radix Sort with Cutoff", [this](std::vector<std::string>& arr) {
          msdRadixSortWithCutoff(arr, 0, arr.size() - 1, 0);
          }, testData, arrayType);
      }
    }
  }

  // Сохранение результатов в CSV
  void saveResultsToCSV(const std::string& filename) {
    std::ofstream file(filename);
    file << "Algorithm,ArrayType,Size,TimeMs,Comparisons\n";

    for (const auto& result : results) {
      file << result.algorithmName << ","
        << result.arrayType << ","
        << result.arraySize << ","
        << result.timeMs << ","
        << result.comparisons << "\n";
    }

    file.close();
    std::cout << "\nResults saved to " << filename << std::endl;
  }

  void printSummary() {
    std::cout << "\n=== SUMMARY ===\n";
    std::cout << "Total tests conducted: " << results.size() << std::endl;

    // Группировка по алгоритмам
    std::map<std::string, std::vector<TestResult>> byAlgorithm;
    for (const auto& result : results) {
      byAlgorithm[result.algorithmName].push_back(result);
    }

    for (const auto& [algorithm, results] : byAlgorithm) {
      double avgTime = 0;
      long long avgComparisons = 0;

      for (const auto& result : results) {
        avgTime += result.timeMs;
        avgComparisons += result.comparisons;
      }

      avgTime /= results.size();
      avgComparisons /= results.size();

      std::cout << algorithm << ": avg " << std::fixed << std::setprecision(2)
        << avgTime << "ms, " << avgComparisons << " comparisons\n";
    }
  }
};

// Главная функция
int main() {
  std::cout << "String Sorting Algorithms Performance Research\n";
  std::cout << "==============================================\n";

  StringSortTester tester;

  // Запуск полного тестирования
  tester.runCompleteTest();

  // Сохранение результатов
  tester.saveResultsToCSV("sorting_results.csv");

  // Вывод сводки
  tester.printSummary();

  return 0;
}