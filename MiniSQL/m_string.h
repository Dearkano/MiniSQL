#pragma once
#include <string>

#include <cstring>

#include <algorithm>
struct m_string {

private:

	const static int str_size = 256;

	

public:

	m_string() = default;

	char str[str_size]{};


	m_string(const m_string &) = default;



	explicit m_string(const char c_ptr[]) {

		memcpy(str, c_ptr, (size_t)std::min((int)strlen(c_ptr), str_size));

	}



	explicit m_string(const std::string &std_str) {

		memcpy(str, std_str.c_str(), (size_t)std::min((int)std_str.size(), str_size));

	}



	m_string &operator=(const m_string &) = default;



	m_string &operator=(const std::string &std_str) {

		memcpy(str, std_str.c_str(), (size_t)std::min((int)std_str.size(), str_size));

		return *this;

	}



	m_string &operator=(const char *c_str) {

		memcpy(str, c_str, strlen(c_str));

		return *this;

	}



	bool operator!=(const m_string &obj) const {

		return strcmp(this->str, obj.str) != 0;

	}



	bool operator==(const m_string &obj) const {

		return strcmp(this->str, obj.str) == 0;

	}



	bool operator>(const m_string &obj) const {

		return strcmp(this->str, obj.str) > 0;

	}



	bool operator<(const m_string &obj) const {

		return strcmp(this->str, obj.str) < 0;

	}



	bool operator>=(const m_string &obj) const {

		return strcmp(this->str, obj.str) >= 0;

	}



	bool operator<=(const m_string &obj) const {

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



const int m_string::str_size;

