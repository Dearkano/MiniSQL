#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include "BPTree.h"
#include<time.h>
#include <string>
#include <cstring>
#include <algorithm>
#include "m_string.h"

//struct m_string {
//private:
//    const static int str_size = 256;
//    char str[str_size]{};
//public:
//    m_string() = default;
//
//    m_string(const m_string &) = default;
//
//    explicit m_string(const char c_ptr[]) {
//        memcpy(str, c_ptr, (size_t) std::min((int) strlen(c_ptr), str_size));
//    }
//
//    explicit m_string(const std::string &std_str) {
//        memcpy(str, std_str.c_str(), (size_t) std::min((int) std_str.size(), str_size));
//    }
//
//    m_string &operator=(const m_string &) = default;
//
//    m_string &operator=(const std::string &std_str) {
//        memcpy(str, std_str.c_str(), (size_t) std::min((int) std_str.size(), str_size));
//        return *this;
//    }
//
//    m_string &operator=(const char *c_str) {
//        memcpy(str, c_str, strlen(c_str));
//        return *this;
//    }
//
//    bool operator!=(const m_string &obj) const {
//        return strcmp(this->str, obj.str) != 0;
//    }
//
//    bool operator==(const m_string &obj) const {
//        return strcmp(this->str, obj.str) == 0;
//    }
//
//    bool operator>(const m_string &obj) const {
//        return strcmp(this->str, obj.str) > 0;
//    }
//
//    bool operator<(const m_string &obj) const {
//        return strcmp(this->str, obj.str) < 0;
//    }
//
//    bool operator>=(const m_string &obj) const {
//        return strcmp(this->str, obj.str) >= 0;
//    }
//
//    bool operator<=(const m_string &obj) const {
//        return strcmp(this->str, obj.str) <= 0;
//    }
//
//    friend std::ostream &operator<<(std::ostream &out, const m_string &obj) {
//        out << obj.str;
//        return out;
//    }
//
//    friend std::istream &operator>>(std::istream &in, m_string &obj) {
//        in >> obj.str;
//        return in;
//    }
//
//    ~m_string() = default;
//};
//
//const int m_string::str_size;


class IndexManager {

public:
    typedef struct data_group {
        int type_indicator;
        int int_value;
        float float_value;
        m_string var_char;
    public:
        data_group() = default;

        data_group(int i) : int_value(i), type_indicator(type_int) {}

        data_group(float f) : float_value(f), type_indicator(type_float) {}

        data_group(const std::string &std_str) : var_char(std_str), type_indicator(static_cast<int>(std_str.size())) {}

        data_group operator=(const int &i) {
            int_value = i;
            type_indicator = type_int;
            return *this;
        }

        data_group operator=(const float &f) {
            float_value = f;
            type_indicator = type_float;
            return *this;
        }

        data_group operator=(const std::string &std_str) {
            var_char = std_str;
            type_indicator = static_cast<int>(std_str.size());
            return *this;
        }

    } dtype;
    static const int type_int = -1;
    static const int type_float = -2;
    static const int max_var_char = 256;

    IndexManager();

    IndexManager(IndexManager &&) = default;

    IndexManager(const IndexManager &) = default;

    IndexManager &operator=(IndexManager &&) = default;

    IndexManager &operator=(const IndexManager &) = default;

    ~IndexManager();

    std::vector<offset> search_equal(const std::string &index_name, const dtype &key);

    std::vector<offset> search_between(const std::string &index_name, const dtype &key_begin, const dtype &key_end);

    std::vector<offset> search_smaller(const std::string &index_name, const dtype &key_end);

    std::vector<offset> search_greater(const std::string &index_name, const dtype &key_begin);


    void create_index(std::string index_name, int type_indicator);

    void insert_index(const std::string &index_name, const dtype &key, const offset &value);

    void
    batch_insert(const std::string &index_name, const std::vector<dtype> &keys, const std::vector<offset> &values);


    void drop_index(const std::string &index_name);

    void delete_index(const std::string &index_name, const dtype &key);

private:
    std::map<std::string, BPTree<int> *> int_tree;
    std::map<std::string, BPTree<float> *> float_tree;
    std::map<std::string, BPTree<m_string> *> char_tree;
    std::map<std::string, int> type_reminder;

};

IndexManager::IndexManager() = default;

IndexManager::~IndexManager() = default;

void IndexManager::create_index(std::string index_name, int type_indicator) {
	/*time_t t = time(NULL);
	t = t % 100;
	stringstream fuck;
	fuck << t;
	string shit;
	fuck >> shit;
	index_name = index_name + shit;
    */
	auto it = type_reminder.find(index_name);
    if (it != type_reminder.end()) {
        throw DuplicateIndex();
        return;
    }
    type_reminder[index_name] = type_indicator;
    if (type_indicator == type_int) {
        int_tree[index_name] = new BPTree<int>(index_name);
    } else if (type_indicator == type_float) {
        float_tree[index_name] = new BPTree<float>(index_name);;
    } else {
        char_tree[index_name] = new BPTree<m_string>(index_name);
    }
}

void IndexManager::drop_index(const std::string &index_name) {
    auto it = type_reminder.find(index_name);
    if (it == type_reminder.end()) {
        throw IndexNotExist();
        return;
    }
    auto data_type = it->second;
    if (data_type == type_int) {
        auto p_tree = int_tree[index_name];
        delete p_tree;
        int_tree.erase(index_name);
    } else if (data_type == type_float) {
        auto p_tree = float_tree[index_name];
        delete p_tree;
        float_tree.erase(index_name);
    } else {
        auto p_tree = char_tree[index_name];
        delete p_tree;
        char_tree.erase(index_name);
    }
}

void IndexManager::insert_index(const std::string &index_name, const IndexManager::dtype &key, const offset &value) {
    auto it = type_reminder.find(index_name);
    if (it == type_reminder.end()) {
        throw IndexNotExist();
        return;
    }
    auto data_type = it->second;
    if (key.type_indicator != data_type) {
        throw TypeDisaccord();
        return;
    }
    if (data_type == type_int) {
        auto p_tree = int_tree[index_name];
        p_tree->insert(key.int_value, value);
    } else if (data_type == type_float) {
        auto p_tree = float_tree[index_name];
        p_tree->insert(key.float_value, value);
    } else {
        auto p_tree = char_tree[index_name];
        p_tree->insert(key.var_char, value);
    }
}

void IndexManager::delete_index(const std::string &index_name, const IndexManager::dtype &key) {
    auto it = type_reminder.find(index_name);
    if (it == type_reminder.end()) {
        throw IndexNotExist();
        return;
    }
    auto data_type = it->second;
    if (key.type_indicator != data_type) {
        throw TypeDisaccord();
        return;
    }
    if (data_type == type_int) {
        auto p_tree = int_tree[index_name];
        p_tree->delete_by_key(key.int_value);
    } else if (data_type == type_float) {
        auto p_tree = float_tree[index_name];
        p_tree->delete_by_key(key.float_value);
    } else {
        auto p_tree = char_tree[index_name];
        p_tree->delete_by_key(key.var_char);
    }
}

std::vector<offset> IndexManager::search_equal(const std::string &index_name, const IndexManager::dtype &data) {
    std::vector<offset> result;
    auto it = type_reminder.find(index_name);
    if (it == type_reminder.end()) {
        throw IndexNotExist();
        return result;
    }
    auto data_type = it->second;
    if (data.type_indicator != data_type) {
        throw TypeDisaccord();
        return result;
    }
    if (data_type == type_int) {
        auto p_tree = int_tree[index_name];
        result.push_back(p_tree->search_by_key(data.int_value));
    } else if (data_type == type_float) {
        auto p_tree = float_tree[index_name];
        result.push_back(p_tree->search_by_key(data.float_value));
    } else {
        auto p_tree = char_tree[index_name];
        result.push_back(p_tree->search_by_key(data.var_char));
    }
    return result;
}

std::vector<offset> IndexManager::search_greater(const std::string &index_name, const IndexManager::dtype &key_begin) {
    auto result = std::vector<offset>();
    auto it = type_reminder.find(index_name);
    if (it == type_reminder.end()) {
        throw IndexNotExist();
        return result;
    }
    auto data_type = it->second;
    if (key_begin.type_indicator != data_type) {
        throw TypeDisaccord();
        return result;
    }
    if (data_type == type_int) {
        auto p_tree = int_tree[index_name];
        return p_tree->search_greater(key_begin.int_value);
    } else if (data_type == type_float) {
        auto p_tree = float_tree[index_name];
        return p_tree->search_greater(key_begin.float_value);
    } else {
        auto p_tree = char_tree[index_name];
        return p_tree->search_greater(key_begin.var_char);
    }
}

std::vector<offset> IndexManager::search_smaller(const std::string &index_name, const IndexManager::dtype &key_end) {
    auto result = std::vector<offset>();
    auto it = type_reminder.find(index_name);
    if (it == type_reminder.end()) {
        throw IndexNotExist();
        return result;
    }
    auto data_type = it->second;
    if (key_end.type_indicator != data_type) {
        throw TypeDisaccord();
        return result;
    }
    if (data_type == type_int) {
        auto p_tree = int_tree[index_name];
        return p_tree->search_greater(key_end.int_value);
    } else if (data_type == type_float) {
        auto p_tree = float_tree[index_name];
        return p_tree->search_greater(key_end.float_value);
    } else {
        auto p_tree = char_tree[index_name];
        return p_tree->search_greater(key_end.var_char);
    }
}

std::vector<offset> IndexManager::search_between(const std::string &index_name, const IndexManager::dtype &key_begin,
                                                 const IndexManager::dtype &key_end) {
    auto result = std::vector<offset>();
    auto it = type_reminder.find(index_name);
    if (it == type_reminder.end()) {
        throw IndexNotExist();
        return result;
    }
    auto data_type = it->second;
    if (key_begin.type_indicator != data_type || key_end.type_indicator != data_type) {
        throw TypeDisaccord();
        return result;
    }
    if (data_type == type_int) {
        auto p_tree = int_tree[index_name];
        return p_tree->search_between(key_begin.int_value, key_end.int_value);
    } else if (data_type == type_float) {
        auto p_tree = float_tree[index_name];
        return p_tree->search_between(key_begin.float_value, key_end.float_value);
    } else {
        auto p_tree = char_tree[index_name];
        return p_tree->search_between(key_begin.var_char, key_end.var_char);
    }
}

void IndexManager::batch_insert(const std::string &index_name, const std::vector<IndexManager::dtype> &keys,
                                const std::vector<offset> &values) {
    if (keys.size() != values.size()) {
        throw BatchSizeNotEqual();
    }
    for (int i = 0; i < keys.size(); i++) {
        insert_index(index_name, keys[i], values[i]);
    }
}

#endif