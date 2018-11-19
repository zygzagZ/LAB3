#include "wallet.h"
#include <bitset>
#include <algorithm>
#include <chrono>

namespace {
	uint64_t getTime() {
		using namespace std::chrono;
		return duration_cast<milliseconds>(
			system_clock::now().time_since_epoch()
		).count();
	}
	uint64_t moneyAmount = 0;
}

Operation::Operation(uint64_t b, uint64_t a, uint64_t t)
	: before(b), after(a), time(t) {
}

std::ostream & operator<<(std::ostream &os, const Operation &op) {
	using namespace std::chrono;
	std::time_t tt = system_clock::to_time_t(
		time_point<high_resolution_clock>() + milliseconds(op.time));
	std::tm ttm = *localtime(&tt);

	return os << "Wallet balance is " << ((long double)op.after/Wallet::UNITS_IN_B) << 
		" B after operation made at day " << 
		ttm.tm_year + 1900 << '-' <<
	    ttm.tm_mon + 1 << '-' <<
	    ttm.tm_mday;
}

const Wallet & Empty() {
	static const Wallet w;
	return w;
}

Wallet::Wallet() {
	money = 0;
	std::cerr << "Wallet()" << std::endl;
	*this = 0;
}

Wallet::~Wallet() {
	moneyAmount -= money;
	std::cerr << "~Wallet()" << std::endl;
}

Wallet::Wallet(int n) {
	money = 0;
	std::cerr << "Wallet(" << n << ")" << std::endl;
	*this = n * UNITS_IN_B;
}

Wallet::Wallet(const char * _str) {
	std::string str(_str);
	money = 0;
	std::cerr << "Wallet(\"" << str << "\")" << std::endl;
	std::replace(str.begin(), str.end(), ',', '.');
	setUnits(std::stold(str) * UNITS_IN_B);
}

Wallet::Wallet(Wallet &&w) {
	money = 0;
	std::cerr << "Wallet(&&w " << w << ")" << std::endl;
	*this = std::move(w);
}

Wallet::Wallet(Wallet &&w1, Wallet &&w2) {
	std::cerr << "Wallet(&&" << w1 << ", &&" << w2 << ")" << std::endl;
	history = std::move(w1.history);
	history.reserve(opSize()+w2.opSize());
	auto it = history.end();
	std::copy(w2.history.begin(), w2.history.end(), 
		std::back_inserter(history));

	std::inplace_merge(history.begin(), it, history.end());
	setUnits(w1.getUnits() + w2.getUnits());
}

Wallet Wallet::fromBinary(std::string str) {
	std::cerr << "Wallet::fromBinary(\"" << str << "\")" << std::endl;
	return Wallet(static_cast<int>(std::bitset<20>(str).to_ulong()));
}

Wallet& Wallet::operator*=(int w) {
	std::cerr << *this << " *= " << w << std::endl;
	setUnits(money*w);

	return *this;
}

void Wallet::setUnits(int64_t w) {
	if (w < 0)
		throw std::logic_error("Liczba pieniędzy poniżej 0");

	if (moneyAmount - money + w > 21e6*UNITS_IN_B)
		throw std::logic_error("Liczba B przekroczyła 21 milionów");

	moneyAmount += w - money;

	history.emplace_back(money, w, getTime());
	money = w;
}

Wallet& Wallet::operator=(Wallet &&w) {
	if (this != &w) { 
		std::cerr << *this << " = &&w " << w << std::endl;
		history = std::move(w.history);
		setUnits(w.money);
	}
	return *this;
}

Wallet& Wallet::operator=(int64_t w) {
	std::cerr << *this << " = (int)" << w << std::endl;
	setUnits(w);

	return *this;
}

std::ostream & operator<<(std::ostream &os, const Wallet &w) {
	os << "Wallet[" << ((double)w.getUnits()/Wallet::UNITS_IN_B) << " B]";
	return os;
}

Wallet operator+(Wallet &&w1, Wallet &w2) {
	std::cerr << " operator+ &&" << w1 << " &" << w2 << std::endl;
	w1.history.clear();
	std::copy(w2.history.begin(), w2.history.end(), std::back_inserter(w1.history));
	w1.setUnits(w1.getUnits() + w2.getUnits());
	w2 = 0;
	return std::move(w1);
}

Wallet operator+(Wallet &&w1, Wallet &&w2) {
	std::cerr << " operator+ &&" << w1 << " &&" << w2 << std::endl;
	int64_t newUnits = w1.getUnits() + w2.getUnits();
	w2.setUnits(0);
	w1.setUnits(newUnits);
	w1.history.pop_back();
	return std::move(w1);
}

Wallet operator-(Wallet &&w1, Wallet &w2) {
	std::cerr << " operator- &&" << w1 << " &" << w2 << std::endl;
	w1.history.clear();
	std::copy(w2.history.begin(), w2.history.end(), w1.history.begin());
	w1.setUnits(w1.getUnits() - w2.getUnits());
	w2 *= 2;
	return std::move(w1);
}

Wallet operator-(Wallet &&w1, Wallet &&w2) {
	std::cerr << " operator- &&" << w1 << " &&" << w2 << std::endl;
	int64_t newUnits = w1.getUnits() - w2.getUnits();
	w1.setUnits(newUnits);
	w1.history.pop_back();
	return std::move(w1);
}

Wallet& operator+=(Wallet &l, Wallet &w) {
	std::cerr << "&" << l << " += &" << w << std::endl;
	auto sum = w.getUnits() + l.getUnits();
	w.setUnits(0);
	l.setUnits(sum);

	return l;
}

Wallet& operator-=(Wallet &l, Wallet &w) {
	std::cerr << "&" << l << " -= &" << w << std::endl;
	l.setUnits(l.getUnits() - w.getUnits());
	w.setUnits(w.getUnits()*2);

	return l;
}

Wallet&& operator+=(Wallet &&l, Wallet &w) { return std::move(l += w); }
Wallet&& operator+=(Wallet &&l, Wallet &&w) { return std::move(l += w); }
Wallet& operator+=(Wallet &l, Wallet &&w) { return l += w; }
Wallet& operator-=(Wallet &l, Wallet &&w) { return l -= w; }
Wallet&& operator-=(Wallet &&l, Wallet &w) { return std::move(l -= w); }
Wallet&& operator-=(Wallet &&l, Wallet &&w) { return std::move(l += w); }