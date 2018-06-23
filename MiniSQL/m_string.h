#pragma once
class m_string {
public:
    static const int str_size = 256;
    char str[str_size];
public:
    m_string() = default;

    m_string(const m_string &) = default;

    m_string &operator=(const m_string &) = default;

    m_string &operator=(const std::string &std_str) {
        memcpy(str, std_str.c_str(), std_str.size());
        return *this;
    }

    m_string &operator=(const char *c_str) {
        memcpy(str, c_str, strlen(c_str));
        return *this;
    }

    bool operator!=(const m_string &obj) {
        return strcmp(this->str, obj.str) != 0;
    }

    bool operator==(const m_string &obj) {
        return strcmp(this->str, obj.str) == 0;
    }

    bool operator>(const m_string &obj) {
        return strcmp(this->str, obj.str) > 0;
    }

    bool operator<(const m_string &obj) {
        return strcmp(this->str, obj.str) < 0;
    }

    bool operator>=(const m_string &obj) {
        return strcmp(this->str, obj.str) >= 0;
    }

    bool operator<=(const m_string &obj) {
        return strcmp(this->str, obj.str) <= 0;
    }

    friend std::ostream &operator<<(std::ostream &out, const m_string &obj) {
        out << obj.str;
        return out;
    }

    friend std::istream &operator>>(std::istream &in, m_string &obj) {
        in >> obj.str;
        return in;
    }

    ~m_string() = default;
};