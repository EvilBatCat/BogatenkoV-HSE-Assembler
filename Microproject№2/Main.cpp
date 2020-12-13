#include <iostream>
#include <thread>
#include <mutex>
#include <ctime>
#include <random>
#include <string>
using namespace std;

static atomic<int>* forkState;
static mutex _mutex;
static int n, m;
static bool isEnd;
static time_t endTime, startTime;
static mt19937 rnd;

void eating(int num);
void wait(int index);
void ping(int index, bool isReturned);

int main(int argc, char* argv[]) {
	setlocale(LC_CTYPE, "rus");
	isEnd = false;
	if (argc > 1)
		try {
		n = stoi(argv[1]);
		if (n < 1)
			throw exception("Illegal argument!");
	}
	catch(exception)
	{
		cout << "wrong input";
		return -1;
	}

	if (argc > 2)
		try {
		m = stoi(argv[2]);
		if (m < 1)
			throw exception("Illegal argument!");
	}
	catch (exception)
	{
		cout << "wrong input";
		return -1;
	}
	thread* oldMans = new thread[n];
	forkState = new atomic<int>[n];

	rnd.seed((int)(time(0)));
	time(&startTime);
	for (int i = 0; i < n; i++)
		forkState[i] = 2;

	for (int i = 0; i < n; i++)
		oldMans[i] = thread(eating, i);

	this_thread::sleep_for(chrono::seconds(m));
	isEnd = true;
	cout << "Конец трапезы";
}

void eating(int num) {
	while (!isEnd) {
		wait(num); // Филосовствование.

		_mutex.lock();
		if (forkState[num] == 2) {
			forkState[(num - 1 + n) % n] --;
			forkState[(num + 1) % n] --;
			forkState[num] -= 2;
			time(&endTime);
			cout << difftime(endTime, startTime) << " Секунда: " "филосов № " << num + 1 << " начал есть" << endl;
			_mutex.unlock();

			wait(num); // Употребление пищи.

			_mutex.lock();
			forkState[(num - 1 + n) % n] ++;
			forkState[(num + 1) % n] ++;
			forkState[num] += 2;
			time(&endTime);
			cout << difftime(endTime, startTime) << " Секунда: " "филосов № " << num + 1 << " закончил есть" << endl;
			_mutex.unlock();


		}
		else {
			_mutex.unlock();
			this_thread::sleep_for(chrono::milliseconds(500));
		}
	}
}
void wait(int index) {
	int sleepTime = rnd() % 10 + 1;
	this_thread::sleep_for(chrono::seconds(sleepTime));
}
void ping(int index, bool isReturned) {
	forkState[index] += isReturned ? 1 : -1;
}