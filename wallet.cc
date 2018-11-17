#include <sys/timeb.h>
#include <bits/stdc++.h>
#include "wallet.h"

using namespace std;

namespace {
	uint64_t getTime() {
		static uint64_t t = 0;
		return t++;
		// timeb tb;
		// ftime(&tb);
		// // cerr << tb.millitm << " / " << tb.time << endl;
		// return tb.millitm + tb.time * 1000ULL;
	}
	uint64_t moneyAmount = 0;
}

Operation::Operation(uint64_t b, uint64_t a, uint64_t t) : before(b), after(a), time(t) {
}

std::ostream & operator<<(std::ostream &os, const Operation &op) {
	os << "Wallet balance is " << ((double)op.after/Wallet::UNITS_IN_B) << " B after operation made at day ";
	// TODO: format daty
	return os;
}

const Wallet & Empty() {
	static const Wallet w;
	return w;
}

Wallet::Wallet() {
	money = 0;
	cerr << "Wallet()" << endl;
	*this = 0;
}

Wallet::~Wallet() {
	moneyAmount -= money;
	cerr << "~Wallet()" << endl;
}

Wallet::Wallet(int64_t n) {
	money = 0;
	cerr << "Wallet(" << n << ")" << endl;
	*this = n * UNITS_IN_B;
}

Wallet::Wallet(std::string str) {
	money = 0;
	cerr << "Wallet(\"" << str << "\")" << endl;
	std::replace(str.begin(), str.end(), ',', '.');
	setMoney(std::stold(str) * UNITS_IN_B);
}

Wallet::Wallet(Wallet &&w) {
	money = 0;
	cerr << "Wallet(&&w " << w << ")" << endl;
	*this = move(w);
}

Wallet::Wallet(Wallet &&w1, Wallet &&w2) {
	cerr << "Wallet(&&" << w1 << ", &&" << w2 << ")" << endl;
	history = move(w1.history);
	history.reserve(opSize()+w2.opSize());
	auto it = history.end();
	for (auto &i : w2.history)
		history.emplace_back(i);

	std::inplace_merge(history.begin(), it, history.end());
	setMoney(w1.getUnits() + w2.getUnits());
}

Wallet Wallet::fromBinary(std::string str) {
	cerr << "Wallet::fromBinary(\"" << str << "\")" << endl;
	return Wallet(bitset<20>(str).to_ullong());
}

Wallet& Wallet::operator+=(Wallet &w) {
	cerr << *this << " += " << w << endl;
	setMoney(w.getUnits() + getUnits());
	w.setMoney(0);

	return *this;
}

Wallet& Wallet::operator+=(double w) {
	cerr << *this << " += " << w << endl;
	setMoney(money+w*UNITS_IN_B);

	return *this;
}

Wallet& Wallet::operator*=(double w) {
	cerr << *this << " *= " << w << endl;
	setMoney(money*w);

	return *this;
}

Wallet& Wallet::operator=(Wallet &&w) {
	if (this != &w) { 
		cerr << *this << " = &&w " << w << endl;
		history = std::move(w.history);
		setMoney(w.money);
	}
	return *this;
}

void Wallet::setMoney(int64_t w) {
	if (w < 0)
		throw std::logic_error("Liczba pieniędzy poniżej 0");

	if (moneyAmount - money + w > 21e6*UNITS_IN_B)
		throw std::logic_error("Liczba B przekroczyła 21 milionów");

	moneyAmount += w - money;

	history.emplace_back(money, w, getTime());
	money = w;
}

Wallet& Wallet::operator=(int64_t w) {
	cerr << *this << " = (int)" << w << endl;
	setMoney(w);

	return *this;
}

std::ostream & operator<<(std::ostream &os, const Wallet &w) {
	os << "Wallet[" << ((double)w.getUnits()/Wallet::UNITS_IN_B) << " B]";
	return os;
}

Wallet operator+(Wallet w1, Wallet &w2) {
	cerr << " operator+ " << w1 << " &" << w2 << endl;
	std::copy(w2.history.begin(), w2.history.end(), w1.history.begin());
	w1 = w1.getUnits() + w2.getUnits();
	w2 = 0;
	return w1;
}

Wallet operator-(Wallet w1, Wallet &w2) {
	cerr << " operator- " << w1 << " " << w2 << endl;
	w1.history.reserve(w1.opSize()+w2.opSize());
	auto it = w1.history.end();
	for (auto &i : w2.history)
		w1.history.emplace_back(i);

	std::inplace_merge(w1.history.begin(), it, w1.history.end());
	w1 = w1.getUnits() - w2.getUnits();
	w2 = w2.getUnits()*2;
	return std::move(w1);
}

using namespace std;

static const uint UNITS_IN_B = 100000000;

int main() {
	{
		assert(Empty() == 0);
		assert(Empty() < Wallet(1));
		assert(Empty().getUnits() == 0);
		assert(Empty().opSize() == 1);
		assert(Empty()[0].getUnits() == 0);

		Wallet w1;
		assert(w1 == Empty());
		assert(w1.opSize() == 1);

		Wallet w2(1), w3(1);
		assert(w2.opSize() == 1);
		assert(w2.getUnits() == UNITS_IN_B);

		assert(Wallet::fromBinary("100") == Wallet(4));
		assert(Wallet(" 1.2000 ") == Wallet("1,2"));
		assert((Wallet(2) + w2) == Wallet(3));
		assert(Wallet(1) + Wallet(2) == Wallet(3));
		assert(1+2 == Wallet(3));

		Wallet w4;
		assert(w4.opSize() == 1);

		w4 += 2;
		assert(w4.opSize() == 2);
		assert(w4[0] <= w4[1]);
		assert(w4[1].getUnits() == w4.getUnits());

		w4 *= 3;
		assert(w4.opSize() == 3);
		assert(w4[2].getUnits() == 6 * UNITS_IN_B);

		assert(Wallet(move(w4)).opSize() == 4);

		cerr << "ello\n" << endl;
		Wallet w5{2};
		Wallet w6 = Wallet(10) - w5;
		assert(w5 == Wallet(4));
		assert(w6 == Wallet(8));
		w5 += w6;
		assert(w5 == Wallet(12));
		assert(w6 == Wallet(0));
		cerr << "ello\n" << endl;
	}


	// moje testy
	{
		Wallet m0;
		assert(m0 == 0);
		assert(m0.opSize() == 1);

		Wallet m1(5);
		assert(m1.getUnits() == 5*UNITS_IN_B);
		assert(m1.opSize() == 1);

		Wallet m2("1,5\t"), m3("    1.5");
		assert(m2 == m3);
		assert(m2.getUnits() == 3*UNITS_IN_B/2);
		assert(m2.opSize() == 1 && m3.opSize() == 1);
		
		Wallet m4(std::move(m1));
		assert(m4.getUnits() == 5*UNITS_IN_B);
		assert(m4.opSize() == 2);

		Wallet m5(move(m4), move(m3));
		assert(m5.getUnits() == 13*UNITS_IN_B/2);
		assert(m5.opSize() == 4);

		m0 = move(m2);
		assert(m0.getUnits() == 3*UNITS_IN_B/2);
		assert(m0.opSize() == 2);
		assert(m0[0].getUnits() == m0.getUnits());
	}

	{
		Wallet w1(20000000), w2(1000000);
		bool ok = true;
		try {
			Wallet w3(1);
			ok = false;
		} catch(const std::exception &e) {}

		try {
			Wallet w3(-1);
			ok = false;
		} catch(const std::exception &e) {}
		if (!ok) 
			throw std::logic_error("Liczba przekroczyła 21 mln B lub spadła poniżej 0B bez wyjątku");
	}

	{
		Wallet w1(1), w2(2);

		Wallet suma2 = Wallet(2) + w2; // OK, w w2 jest 0 B po operacji
									   // i jeden nowy wpis w historii,
									   // a w suma2 jest w2.getUnits() + 2 B.
									   // Historia operacji powstałego obiektu
									   // zależy od implementacji.
		assert(w2 == 0);
		assert(w2.opSize() == 2);
		assert(suma2 == 4);
		Wallet suma4 = Wallet(1) + Wallet(2);  // OK, suma4 ma dwa wpisy
											   // w historii i 3 B
		assert(suma4.opSize() == 2);
		assert(suma4 == 3);
	}



	/*{

		Wallet w1(1), w2(2);
		Wallet suma1 = w1 + Wallet(1); // błąd kompilacji 1
		Wallet suma3 = suma1 + w2;  // błąd kompilacji 2
		suma1 = suma3; // błąd kompilacji 3
	}
*/
}
