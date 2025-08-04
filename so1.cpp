#include <iostream>
#include <list>
#include <vector>
#include <limits>
#include <string>
#include <sstream>
using namespace std;

// �t���X�g
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

    // pos�ʒu��value��}��
    void insert(size_t pos, const T& value) {
        auto currentData = getCurrent();
        auto it = currentData.begin();
        advance(it, min(pos, currentData.size()));
        currentData.insert(it, value);
        save(currentData);
    }

    // pos�ʒu�̗v�f���폜
    void erase(size_t pos) {
        auto currentData = getCurrent();
        if (pos >= currentData.size()) return;
        auto it = currentData.begin();
        advance(it, pos);
        currentData.erase(it);
        save(currentData);
    }

    // �����ɒǉ�
    void append(const T& value) {
        auto currentData = getCurrent();
        currentData.push_back(value);
        save(currentData);
    }

    void undo() {
        if (current > 0) {
            --current;
        }
    }
    void redo() {
        if (current + 1 < (int)history.size()) {
            ++current;
        } else {
            throw runtime_error("redo�ł��܂���i����ȏ�i�߂܂���j");
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
    cout << "�R�}���h: i(incert), e(erase), a(append), s(show history), c(current), u(undo), r(redo), q(quit)\n";
    cout << "�V���[�g�J�b�g�R�}���h: a20, e3, i1,7 \n";
    while (true) {
        try {
            cout << "> ";
            string line;
            getline(cin, line);
            if (line.empty()) continue;

            // �V���[�g�J�b�g�R�}���h����
            if ((line[0] == 'a' || line[0] == 'e' || line[0] == 'i') && line.size() > 1) {
                if (line[0] == 'a') {
                    int val = stoi(line.substr(1));
                    lst.append(val);
                    cout << "append " << val << '\n';
                } else if (line[0] == 'e') {
                    size_t pos = static_cast<size_t>(stoul(line.substr(1)));
                    lst.erase(pos);
                    cout << "erase " << pos << '\n';
                } else if (line[0] == 'i') {
                    // ��: i3,10 �� 3�Ԗڂ�10��insert
                    size_t comma = line.find(',');
                    if (comma == string::npos) {
                        cout << "i<pos>,<val> �̌`���œ��͂��Ă�������\n";
                    } else {
                        size_t pos = static_cast<size_t>(stoul(line.substr(1, comma - 1)));
                        int val = stoi(line.substr(comma + 1));
                        lst.insert(pos, val);
                        cout << "insert " << val << " at " << pos << '\n';
                    }
                }
                continue;
            }

            // �����̃R�}���h����
            istringstream iss(line);
            iss >> cmd;
            if (cmd == 'q') break;
            if (cmd == 'i') {
                size_t pos;
                int val;
                cout << "insert�ʒu: ";
                if (!(cin >> pos)) throw runtime_error("insert�ʒu�̓��͂��s���ł�");
                cout << "�l: ";
                if (!(cin >> val)) throw runtime_error("�l�̓��͂��s���ł�");
                lst.insert(pos, val);
            }
            else if (cmd == 'e') {
                size_t pos;
                cout << "erase�ʒu: ";
                if (!(cin >> pos)) throw runtime_error("erase�ʒu�̓��͂��s���ł�");
                lst.erase(pos);
            }
            else if (cmd == 'a') {
                int val;
                cout << "append�l: ";
                if (!(cin >> val)) throw runtime_error("append�l�̓��͂��s���ł�");
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
                cout << "�s���ȃR�}���h�ł�\n";
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        catch (const out_of_range& e) {
            cout << "�͈͊O�A�N�Z�X��O: " << e.what() << '\n';
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        catch (const exception& e) {
            cout << "��O: " << e.what() << '\n';
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    cout << "�I�����܂�\n";
}