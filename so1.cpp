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

	// pos位置にvalueを挿入
	void insert(size_t pos, const T& value) {
		auto currentData = getCurrent();
		auto it = currentData.begin();
		advance(it, min(pos, currentData.size()));
		currentData.insert(it, value);
		save(currentData);
	}

	// pos位置の要素を削除
	void erase(size_t pos) {
		auto currentData = getCurrent();
		if (pos >= currentData.size()) return;
		auto it = currentData.begin();
		advance(it, pos);
		currentData.erase(it);
		save(currentData);
	}

	// 末尾に追加（pop_backの代わりに新設）
	void append(const T& value) {
		auto currentData = getCurrent();
		currentData.push_back(value);
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
	char cmd;
	cout << "コマンド: i(incert), e(erase), a(append), s(show history), u(undo), r(redo), q(quit)\n";
	while (true) {
		cout << "> ";
		cin >> cmd;
		if (cmd == 'q') break;
		if (cmd == 'i') {
			size_t pos;
			int val;
			cout << "insert位置: ";
			cin >> pos;
			cout << "値: ";
			cin >> val;
			lst.insert(pos, val);
		}
		else if (cmd == 'e') {
			size_t pos;
			cout << "erase位置: ";
			cin >> pos;
			lst.erase(pos);
		}
		else if (cmd == 'a') {
			int val;
			cout << "append値: ";
			cin >> val;
			lst.append(val);
		}
		else if (cmd == 's') {
			int i = 0;
			cout << "show history:\n";
			for (const auto& state : lst) {
				cout << "Step " << i++ << ": ";
				for (int v : state) cout << ' ' << v;
				cout << '\n';
			}
		}
		else if (cmd == 'u') {
			lst.undo();
			cout << "undo\n";
		}
		else if (cmd == 'r') {
			lst.redo();
			cout << "redo\n";
		}
		else {
			cout << "不明なコマンドです\n";
		}
	}
	cout << "終了します\n";
}