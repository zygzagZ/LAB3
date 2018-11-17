#ifndef __WALLET_H__
#define __WALLET_H__
#include <bits/stdc++.h>
using namespace std;

class Operation {
public:
	Operation(uint64_t b, uint64_t a, uint64_t t);
	// bool operator<(const Operation &op2) const;
	uint64_t getUnits() const { return after; }
	operator uint64_t() const { return time; }
private:
	uint64_t before, after, time;

	friend std::ostream & operator<<(std::ostream &os, const Operation &op);
};

class Wallet {
public:
	static const uint UNITS_IN_B = 1e8;

	Wallet();
	~Wallet();
	Wallet(const Wallet &w) = delete;
	Wallet(Wallet &&w);
	Wallet(int64_t n);
	Wallet(std::string str);
	Wallet(Wallet &&w1, Wallet &&w2);

	static Wallet fromBinary(std::string str);

	Wallet& operator=(int64_t w);
	Wallet& operator=(Wallet &&w);
	Wallet& operator+=(double w);
	Wallet& operator+=(Wallet &w);
	Wallet& operator*=(double w);

	bool operator==(const Wallet & w) const { return money == w.money; }
	bool operator<(const Wallet & w) const { return money < w.money; }
	bool operator<=(const Wallet & w) const { return money <= w.money; }
	bool operator>(const Wallet & w) const { return money > w.money; }
	bool operator>=(const Wallet & w) const { return money >= w.money; }
	// explicit operator uint64_t() const { cerr << "(int)" << money << endl; return money; }
	// operator double() const { cerr << "(double)" << money/(double)UNITS_IN_B << endl; return money/(double)UNITS_IN_B; }
	const Operation & operator[] (int index) const {
		return history[index];
	}

	size_t opSize() const { return history.size(); }
	uint64_t getUnits() const { return money; }

private:
	void setMoney(int64_t newBalance);
	uint64_t money;
	std::vector<Operation> history;

	friend Wallet operator+(Wallet w1, Wallet &w2);
	// friend Wallet operator+(Wallet w1, Wallet w2);
	friend Wallet operator-(Wallet w1, Wallet &w2);

};

const Wallet & Empty();
Wallet operator+(Wallet w1, Wallet &w2);
// Wallet operator+(Wallet w1, Wallet w2);
Wallet operator-(Wallet w1, Wallet &w2);

// inline Wallet operator+(Wallet left, Wallet const& right);
// inline Wallet operator+(Wallet const& left, Wallet right) { right += left; return right; } // commutative
// inline Wallet operator+(Wallet left, Wallet&& right) { left += right; return left; } // disambiguation

std::ostream & operator<<(std::ostream &os, const Wallet &w);
#endif