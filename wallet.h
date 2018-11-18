#ifndef __WALLET_H__
#define __WALLET_H__
#include <iostream>
#include <vector>

class Operation {
public:
	Operation(uint64_t b, uint64_t a, uint64_t t);
	uint64_t getUnits() const { return after; }
	operator uint64_t() const { return time; }
private:
	uint64_t before, after, time;

	friend std::ostream & operator<<(std::ostream &os, const Operation &op);
};

class Wallet {
public:
	static const int64_t UNITS_IN_B = 1e8;

	Wallet();
	~Wallet();

template<
    typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>   Wallet(T&&) = delete;

	Wallet(Wallet &&w);
	Wallet(int n);


	Wallet(const char *str);
	Wallet(Wallet &&w1, Wallet &&w2);

	static Wallet fromBinary(std::string str);

	Wallet& operator=(int64_t w);
	Wallet& operator=(Wallet &&w);
	Wallet& operator+=(Wallet &w);
	Wallet& operator+=(Wallet &&w);
	Wallet& operator-=(Wallet &w);
	Wallet& operator-=(Wallet &&w);
	Wallet& operator*=(double w);

	const Operation & operator[] (int index) const {
		return history[index];
	}

	size_t opSize() const { return history.size(); }
	uint64_t getUnits() const { return money; }

private:
	void setUnits(int64_t newBalance);
	uint64_t money;
	std::vector<Operation> history;

	friend Wallet operator+(Wallet &&w1, Wallet &w2);
	friend Wallet operator+(Wallet &&w1, Wallet &&w2);
	friend Wallet operator-(Wallet &&w1, Wallet &w2);
	friend Wallet operator-(Wallet &&w1, Wallet &&w2);
	friend bool operator==(const Wallet &l, const Wallet &r);
	friend bool operator<(const Wallet &l, const Wallet &r);
	friend bool operator<=(const Wallet &l, const Wallet &r);
	friend bool operator>(const Wallet &l, const Wallet &r);
	friend bool operator>=(const Wallet &l, const Wallet &r);

};

const Wallet & Empty();
Wallet operator+(Wallet &&w1, Wallet &w2);
Wallet operator+(Wallet &&w1, Wallet &&w2);
Wallet operator-(Wallet &&w1, Wallet &w2);
Wallet operator-(Wallet &&w1, Wallet &&w2);


inline bool operator==(const Wallet &l, const Wallet &r) { return l.money == r.money; }
inline bool operator<(const Wallet &l, const Wallet &r) { return l.money < r.money; }
inline bool operator<=(const Wallet &l, const Wallet &r) { return l.money <= r.money; }
inline bool operator>(const Wallet &l, const Wallet &r) { return l.money > r.money; }
inline bool operator>=(const Wallet &l, const Wallet &r) { return l.money >= r.money; }

std::ostream & operator<<(std::ostream &os, const Wallet &w);

#endif