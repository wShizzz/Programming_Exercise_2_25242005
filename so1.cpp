#include <iostream>
#include <list>
#include <vector>
using namespace std;

template <typename T>
class ReversibleList {
	struct HistoryState {
		list<T> data;
	};
	vector<HistoryState> history;
	int current = 0;

public:
	class Iterator {
		const vector<HistoryState>* history;
		int idx;

	public:
		Iterator(const vector<HistoryState>* h, int i) : history(h), idx(i) {}
		Iterator& operator++() {
			++idx;
			return *this;
		}
		Iterator operator++(int) {
			Iterator tmp = *this;
			++*this;
			return tmp;
		}
		bool operator!=(const Iterator& other) const {
			return idx != other.idx;
		}
		const list<T>& operator*() const { return (*history)[idx].data; }
	};

	ReversibleList() {
		history.push_back(HistoryState{});
		current = 0;
	}

	void push_back(const T& value) {
		auto currentData = getCurrent();
		currentData.push_back(value);
		save(currentData);
	}

	void push_front(const T& value) {
		auto currentData = getCurrent();
		currentData.push_front(value);
		save(currentData);
	}

	void pop_back() {
		auto currentData = getCurrent();
		if (!currentData.empty()) currentData.pop_back();
		save(currentData);
	}

	void pop_front() {
		auto currentData = getCurrent();
		if (!currentData.empty()) currentData.pop_front();
		save(currentData);
	}

	void undo() {
		if (current > 0) --current;
	}
	void redo() {
		if (current + 1 < (int)history.size()) ++current;
	}

	Iterator begin() const { return Iterator(&history, 0); }
	Iterator end() const { return Iterator(&history, history.size()); }

	T get(size_t n) const {
		const auto& lst = getCurrent();
		if (n >= lst.size())
			throw out_of_range("ReversibleList::get: out of range");
		auto it = lst.begin();
		advance(it, n);
		return *it;
	}

private:
	list<T> getCurrent() const {
		if (current >= 0 && current < (int)history.size())
			return history[current].data;
		return {};
	}

	void save(const list<T>& data) {
		if (current + 1 != (int)history.size()) history.resize(current + 1);
		history.push_back({ data });
		++current;
	}
};

int main() {
	ReversibleList<int> lst;
	lst.push_back(10);
	lst.push_front(1);
	lst.push_back(20);
	lst.pop_front();
	cout << "show history:\n";
	int i = 0;
	for (const auto& state : lst) {
		cout << "Step " << i++ << ":";
		for (int v : state) cout << ' ' << v;
		cout << '\n';
	}
	lst.undo();
	lst.push_front(99);

	cout << "show history:\n";
	int j = 0;
	for (const auto& state : lst) {
		cout << "Step " << j++ << ":";
		for (int v : state) cout << ' ' << v;
		cout << '\n';
	}

	cout << "current 2nd element: " << lst.get(1) << '\n';
	try {
		cout << "current 5th element: " << lst.get(5) << '\n';
	}
	catch (const out_of_range& e) {
		cout << "exception: " << e.what() << '\n';
	}

	cout << "Ž„‚ª‘‚«‚Ü‚µ‚½" << endl;
}
#s