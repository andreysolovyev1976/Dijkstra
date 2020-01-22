#pragma once

#include <string>
using std::string;
using std::to_string;
#include <iostream>
using std::ostream;
using std::istream;
using std::cout;
using std::cerr;
using std::endl;

#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <vector>
#include <deque>
#include <list>
using std::map;
using std::unordered_map;
using std::pair;
using std::set;
using std::unordered_set;
using std::vector;
using std::deque;
using std::list;

#include<stdexcept>
using std::runtime_error;
using std::invalid_argument;
using std::out_of_range;
using std::exception;

#include <sstream>
using std::stringstream;
using std::ostringstream;
using std::istringstream;

/*
 * First block - containers out fucntions
 * Second - Assert Funs
 * Third - TestRunner class
 *
 *
 *
 */



template <class T>
ostream& operator << (ostream& os, const set<T>& s) {
	os << "{";
	bool first = true;
	for (const auto& x : s) {
		if (!first) {
			os << ", ";
		}
		first = false;
		os << x;
	}
	return os << "}";
}

template <class T>
ostream& operator << (ostream& os, const unordered_set<T>& s) {
	os << "{";
	bool first = true;
	for (const auto& x : s) {
		if (!first) {
			os << ", ";
		}
		first = false;
		os << x;
	}
	return os << "}";
}

template <class First, class Second>
ostream& operator << (ostream& out, const pair<First, Second>& p) {
	return out << '(' << p.first << ',' << p.second << ')';
}


template <class K, class V>
ostream& operator << (ostream& os, const map<K, V>& m) {
	os << "{";
	bool first = true;
	for (const auto& kv : m) {
		if (!first) {
			os << ", ";
		}
		first = false;
		os << kv.first << ": " << kv.second;
	}
	return os << "}";
}


template <class K, class V>
ostream& operator << (ostream& os, const unordered_map<K, V>& m) {
	os << "{";
	bool first = true;
	for (const auto& kv : m) {
		if (!first) {
			os << ", ";
		}
		first = false;
		os << kv.first << ": " << kv.second;
	}
	return os << "}";
}

template <class T>
ostream& operator << (ostream& os, const vector<T>& s) {
	os << "{";
	bool first = true;
	for ( const auto &x : s )
	{
		if ( !first )
		{
			os << ", ";
		}
		first = false;
		os << x;
	}
	return os << "}";
}

template <class T>
ostream& operator << (ostream& os, const deque<T>& s) {
	os << "{";
	bool first = true;
	for ( const auto &x : s )
	{
		if ( !first )
		{
			os << ", ";
		}
		first = false;
		os << x;
	}
	return os << "}";
}

template <class T>
ostream& operator << (ostream& os, const list<T>& s) {
	os << "{";
	bool first = true;
	for ( const auto &x : s )
	{
		if ( !first )
		{
			os << ", ";
		}
		first = false;
		os << x;
	}
	return os << "}";
}



template<class T, class U>
void AssertEqual(const T& t, const U& u, const string& hint = {}) {
	if (!(t == u)) {
		ostringstream os;
		os << "Assertion failed: " << t << " != " << u;
		if (!hint.empty()) {
			os << " hint: " << hint;
		}
		throw runtime_error(os.str());
	}
}

inline void Assert(bool b, const string& hint) {
	AssertEqual(b, true, hint);
}







class TestRunner {
public:
	template <class TestFunc>
	void RunTest(TestFunc func, const string& test_name) {
		try {
			func();
			cerr << test_name << " OK" << endl;
		} catch (exception& e) {
			++fail_count;
			cerr << test_name << " fail: " << e.what() << endl;
		} catch (...) {
			++fail_count;
			cerr << "Unknown exception caught" << endl;
		}
	}

	~TestRunner() {
		if (fail_count > 0) {
			cerr << fail_count << " unit tests failed. Terminate" << endl;
			exit(1);
		}
	}

private:
	int fail_count = 0;
};

#define ASSERT_EQUAL(x, y) {            	\
  ostringstream __os__;                     \
  __os__ << #x << " != " << #y << ", "      \
    << __FILE__ << ":" << __LINE__;     	\
  AssertEqual(x, y, __os__.str());          \
}

#define ASSERT(x) {                     \
  ostringstream os;                     \
  os << #x << " is false, "             \
    << __FILE__ << ":" << __LINE__;     \
  Assert(x, os.str());                  \
}

#define RUN_TEST(tr, func) \
  tr.RunTest(func, #func)


//Iterator range

template <typename Iterator>
struct IteratorRange {
	IteratorRange(Iterator First, Iterator Last) : first(First), last(Last) {}
	Iterator first , last;
	Iterator begin () const {
		return first;
	}
	Iterator end () const {
		return last;}

	size_t size(){ return last - first;}
};



//
// Created by Andrey Solovyev on 2019-07-06.
//

/*
 * Usage:
 * in figure brackets:
 * {
 * LOG_DURATION(message)
 * ...
 * some code
 * ...
 * }
 *
 * */



#include <chrono>
#include <iostream>
//#include <string>

using namespace std::chrono;

class LogDuration {
public:
	explicit LogDuration(const string& msg = "")
			: message(msg + ": ")
			, start(steady_clock::now())
	{
	}

	~LogDuration() {
		auto finish = steady_clock::now();
		auto dur = finish - start;
		cerr << message
			 << duration_cast<seconds>(dur).count()
			 << " seconds" << endl;
	}
private:
	string message;
	steady_clock::time_point start;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(message) \
LogDuration UNIQ_ID(__LINE__){message};






/*  APPLICATION EXAMLPE

istream &ReadLine(istream &input, string &s, TotalDuration &dest) {
    ADD_DURATION(dest);
    return getline(input, s);
}

 int main() {
    TotalDuration read("Total read");
    TotalDuration parse("Total parse");
    for (string line; ReadLine(cin, line, read);) {
        ADD_DURATION(parse);
        const auto words = SplitIntoWordsView(line);
    }
}
*/



struct TotalDuration {
  string message;
  steady_clock::duration value;

  explicit TotalDuration(const string& msg)
		  : message(msg + ": "), value(0) {
  }
  ~TotalDuration(){
	  ostringstream os;
	  os << message
		 << duration_cast<milliseconds>(value).count()
		 << " ms" << endl;
	  cerr << os.str();
  }
};

class AddDuration {
public:
	explicit AddDuration(steady_clock::duration& dest)
			: add_to(dest), start(steady_clock::now()) {
	}

	explicit AddDuration(TotalDuration& dest)
			: AddDuration(dest.value) {
	}

	~AddDuration(){
		add_to += steady_clock::now() - start;
	}


private:
	steady_clock::duration& add_to;
	steady_clock::time_point start;
};

#define MY_UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define MY_UNIQ_ID(lineno) MY_UNIQ_ID_IMPL(lineno)

#define ADD_DURATION(value) \
  AddDuration MY_UNIQ_ID(__LINE__){value};

