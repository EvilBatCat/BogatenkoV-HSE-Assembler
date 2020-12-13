#include <iostream>
#include <ctime>
#include <cmath>
#include <thread>
#include <mutex>
#include <fstream>
#include <string>
using namespace std;

//4. Найти обратную матрицу для матрицы А.Входные данные : целое
//положительное число n, произвольная матрица А размерности n х n.
//Количество потоков является входным параметром, при этом размерность
//матриц может быть не кратна количеству потоков.

template <typename T> void FreeMemMat(T** matr, int n);
template <typename T> void PrintMtx(T** matr, int n);
template <typename T> void SaveMtx(T** matr, int n, string path);
template <typename T> void SetMtx(T** matr, int n, string path);
template <typename T> void TransponMtx(T** matr, T** tMatr, int n);
void Get_matr(int** matr, int n, int** temp_matr, int indRow, int indCol);
int Det(int** matr, int n);
void Thread(int** matr, double** obr_matr, int det, int& c, int n, int q);

int main(int argc, char* argv[])
{
	string input, output;
	int th;
	if (argc > 1)
		input = argv[1];
	else input = "input.txt";
	if (argc > 2)
		output = argv[2];
	else output = "output.txt";
	if (argc > 3)
		try {
		th = stoi(argv[3]);
	}
	catch (exception) { th = 3; }
	else th = 3;

	srand((unsigned)time(NULL));
	setlocale(0, "");
	int c = 0, n, det;
	ifstream in(input);
	in >> n;
	if (n < 1)
	{
		cout << "Неверный размер матрицы!";
		return 0;
	}
	int** matr = new int* [n];
	double** obr_matr = new double* [n];
	double** tobr_matr = new double* [n];
	for (int i = 0; i < n; i++) {
		matr[i] = new int[n];
		obr_matr[i] = new double[n];
		tobr_matr[i] = new double[n];
	}
	SetMtx(matr, n, input);
	PrintMtx(matr, n);
	det = Det(matr, n);
	cout << "Определитель матрицы = " << det << endl;
	if (det) {
		in.open(input);
		in >> th;
		in.close();
		thread* threads = new thread[0];
		if (th < 1)
		{
			cout << "Потоков должно быть >= 1";
			return 0;
		}
		if (th > 1)
			threads = new thread[th - 1];
		for (int i = 0; i < th - 1; i++)
			threads[i] = thread(Thread, matr, obr_matr, det, std::ref(c), n, i + 1);

		// Распаралелить.
		for (int i = 0; i < th - 1; i++)
			threads[i].join();
		Thread(matr, obr_matr, det, c, n, 0);

		//Транспонирование матрицы
		TransponMtx(obr_matr, tobr_matr, n);
		//Печать обратной матрицы после транспонирования
		PrintMtx(tobr_matr, n);
		SaveMtx(tobr_matr, n, output);
	}
	else {
		ofstream out(output);
		out.close();
		cout << "Т.к. определитель матрицы = 0,\nто матрица вырожденная и обратной не имеет!!!" << endl;
	}
	FreeMemMat(tobr_matr, n);
	FreeMemMat(matr, n);
	FreeMemMat(obr_matr, n);

	return 0;
}
mutex mute;
//Функция потока
void Thread(int** matr, double** obr_matr, int det, int& c, int n, int q) {
	while (1) {
		mute.lock();
		int i = c % n, j = c / n;
		c++;
		mute.unlock();
		if (j >= n)
			return;

		int m = n - 1;
		double res = 0;
		int** temp_matr = new int* [m];
		for (int k = 0; k < m; k++)
			temp_matr[k] = new int[m];
		Get_matr(matr, n, temp_matr, i, j);
		if (m > 0)
			res = pow(-1.0, i + j + 2) * Det(temp_matr, m) / det;
		else
			res = 1.0 / det;
		mute.lock();
		obr_matr[i][j] = res;
		mute.unlock();
		FreeMemMat(temp_matr, m);
	}
}
//Функция транспонирования матрицы
template <typename T> void TransponMtx(T** matr, T** tMatr, int n) {//
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			tMatr[j][i] = matr[i][j];
}
//Функция освобождения памяти
template <typename T> void FreeMemMat(T** matr, int n)
{
	for (int i = 0; i < n; i++)
		delete[] matr[i];
	delete[] matr;
}

//функция заполнения матрицы
template <typename T> void SetMtx(T** matr, int n, string path)
{
	try {
		ifstream in;
		in.open(path);
		in >> n;
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				in >> matr[i][j];
		in.close();
	}
	catch (exception e) {}
}

//функция сохранения матрицы в файл
template <typename T> void SaveMtx(T** matr, int n, string path)
{
	try {
		ofstream out;
		out.open(path);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++)
				out << matr[i][j] << " ";
			out << endl;
		}
		out.close();
	}
	catch (exception e) {}
}
//функция печати матрицы
template <typename T> void PrintMtx(T** matr, int n)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			cout << matr[i][j] << " ";
		cout << endl;
	}
}
//функция вычеркивания строки и столбца
void Get_matr(int** matr, int n, int** temp_matr, int indRow, int indCol)
{
	int ki = 0;
	for (int i = 0; i < n; i++) {
		if (i != indRow) {
			for (int j = 0, kj = 0; j < n; j++) {
				if (j != indCol) {
					temp_matr[ki][kj] = matr[i][j];
					kj++;
				}
			}
			ki++;
		}
	}
}
//функция вычисления определителя матрицы
int Det(int** matr, int n)
{
	int temp = 0;   //временная переменная для хранения определителя
	int k = 1;      //степень
	if (n < 1) {
		cout << "Неверный размер матрицы!!!" << endl;
		return 0;
	}
	else if (n == 1)
		temp = matr[0][0];
	else if (n == 2)
		temp = matr[0][0] * matr[1][1] - matr[1][0] * matr[0][1];
	else {
		for (int i = 0; i < n; i++) {
			int m = n - 1;
			int** temp_matr = new int* [m];
			for (int j = 0; j < m; j++)
				temp_matr[j] = new int[m];
			Get_matr(matr, n, temp_matr, 0, i);
			temp = temp + k * matr[0][i] * Det(temp_matr, m);
			k = -k;
			FreeMemMat(temp_matr, m);
		}
	}
	return temp;
}