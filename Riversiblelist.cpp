#include <iostream>
#include <list>
#include <vector>
#include <limits>
#include <string>
#include <sstream>
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
        const HistoryState& operator*() const { return (*history)[idx]; }
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

    // 末尾に追加
    void append(const T& value) {
        auto currentData = getCurrent();
        currentData.push_back(value);
        save(currentData);
    }

    // 戻る
    void undo() {
        if (current > 0) {
            --current;
        }
    }

	// 進む
    void redo() {
        if (current + 1 < (int)history.size()) {
            ++current;
        } else {
            throw runtime_error("redoできません（これ以上進めません）");
        }
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

    list<T> getCurrent() const {
        if (current >= 0 && current < (int)history.size())
            return history[current].data;
        return {};
    }

private:
    void save(const list<T>& data) {
        if (current + 1 != (int)history.size()) history.resize(current + 1);
        history.push_back({ data });
        ++current;
    }
};

int main() {
    ReversibleList<int> lst;
    char cmd;
    cout << "コマンド: i(incert), e(erase), a(append), s(show history), c(current), u(undo), r(redo), q(quit)\n";
    cout << "ショートカットコマンド: a20, e3, i1 7 \n";
    try {
            while(true) {
                cout << "> ";
                string line;
                getline(cin, line);
                if (line.empty()) continue;

                // ショートカットコマンド判定
                if ((line[0] == 'a' || line[0] == 'e' || line[0] == 'i') && line.size() > 1) {
                    if (line[0] == 'a') {
                        int val = stoi(line.substr(1));
                        lst.append(val);
                        cout << "append " << val << '\n';
                    }
                    else if (line[0] == 'e') {
                        size_t pos = static_cast<size_t>(stoul(line.substr(1)));
                        lst.erase(pos);
                        cout << "erase " << pos << '\n';
                    }
                    else if (line[0] == 'i') {
                        size_t comma = line.find(' ');
                        if (comma == string::npos) {
                            cout << "i<pos>space<val> の形式で入力してください\n";
                        }
                        else {
                            size_t pos = static_cast<size_t>(stoul(line.substr(1, comma - 1)));
                            int val = stoi(line.substr(comma + 1));
                            lst.insert(pos, val);
                            cout << "insert " << val << " at " << pos << '\n';
                        }
                    }
                    continue;
                }

                // 既存のコマンド入力
                istringstream iss(line);
                iss >> cmd;
                if (cmd == 'q') break;
                if (cmd == 'i') {
                    size_t pos;
                    int val;
                    cout << "insert位置: ";
                    if (!(cin >> pos)) throw runtime_error("insert位置の入力が不正です");
                    cout << "値: ";
                    if (!(cin >> val)) throw runtime_error("値の入力が不正です");
                    lst.insert(pos, val);
                }
                else if (cmd == 'e') {
                    size_t pos;
                    cout << "erase位置: ";
                    if (!(cin >> pos)) throw runtime_error("erase位置の入力が不正です");
                    lst.erase(pos);
                }
                else if (cmd == 'a') {
                    int val;
                    cout << "append値: ";
                    if (!(cin >> val)) throw runtime_error("append値の入力が不正です");
                    lst.append(val);
                }
                else if (cmd == 's') {
                    int i = 0;
                    cout << "show history:\n";
                    for (const auto& state : lst) {
                        cout << "Step " << i++ << ": ";
                        for (int v : state.data) cout << ' ' << v;
                        cout << '\n';
                    }
                }
                else if (cmd == 'c') {
                    cout << "current state: ";
                    auto current = lst.getCurrent();
                    for (int v : current) cout << v << ' ';
                    cout << '\n';
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
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
        catch (const out_of_range& e) {
            cout << "範囲外アクセス例外: " << e.what() << '\n';
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        catch (const exception& e) {
            cout << "例外: " << e.what() << '\n';
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
   
    cout << "終了します\n";
}
