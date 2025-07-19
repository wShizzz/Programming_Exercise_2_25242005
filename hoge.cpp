#include <iostream>
#include <list>
#include <vector>

template <typename T>
class ReversibleList {
    struct HistoryState {
        std::list<T> data;
    };
    std::vector<HistoryState> history;
    int current = 0;

   public:
    class Iterator {
        const std::vector<HistoryState> *history;
        int idx;

       public:
        Iterator(const std::vector<HistoryState> *h, int i)
            : history(h), idx(i) {}
        Iterator &operator++() {
            ++idx;
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++*this;
            return tmp;
        }
        bool operator!=(const Iterator &other) const {
            return idx != other.idx;
        }
        const std::list<T> &operator*() const { return (*history)[idx].data; }
    };

    ReversibleList() {
        history.push_back(HistoryState{});
        current = 0;
    }

    // 末尾に追加
    void push_back(const T &value) {
        auto currentData = getCurrent();
        currentData.push_back(value);
        save(currentData);
    }

    // 先頭に追加
    void push_front(const T &value) {
        auto currentData = getCurrent();
        currentData.push_front(value);
        save(currentData);
    }

    // 末尾を削除
    void pop_back() {
        auto currentData = getCurrent();
        if (!currentData.empty()) currentData.pop_back();
        save(currentData);
    }

    // 先頭を削除
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

    // 直接要素アクセス: 現在のリストのn番目(0始まり)を返す
    // 範囲外の場合は例外を投げる
    T get(size_t n) const {
        const auto &lst = getCurrent();
        if (n >= lst.size())
            throw std::out_of_range("ReversibleList::get: out of range");
        auto it = lst.begin();
        std::advance(it, n);
        return *it;
    }

   private:
    std::list<T> getCurrent() const {
        if (current >= 0 && current < (int)history.size())
            return history[current].data;
        return {};
    }
    void save(const std::list<T> &data) {
        if (current + 1 != (int)history.size()) history.resize(current + 1);
        history.push_back({data});
        ++current;
    }
};

int main() {
    ReversibleList<int> lst;
    lst.push_back(10);
    lst.push_front(1);
    lst.push_back(20);
    lst.pop_front();
    std::cout << "show history:\n";
    int i = 0;
    for (const auto &state : lst) {
        std::cout << "Step " << i++ << ":";
        for (int v : state) std::cout << ' ' << v;
        std::cout << '\n';
    }
    lst.undo();  // 1,10,20の状態に戻る
    lst.push_front(99);

    std::cout << "show history:\n";
    int j = 0;
    for (const auto &state : lst) {
        std::cout << "Step " << j++ << ":";
        for (int v : state) std::cout << ' ' << v;
        std::cout << '\n';
    }

    std::cout << "current 2nd element: " << lst.get(1) << '\n';  // 10を出力
    try {
        std::cout << "current 5th element: " << lst.get(5)
                  << '\n';  // 範囲外なので例外
    } catch (const std::out_of_range &e) {
        std::cout << "exception: " << e.what() << '\n';
    }
}