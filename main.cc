#include "wallet.h"
#include <unistd.h>
#include <cassert>
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
			throw std::logic_error("Liczba przekroczyła 21 mln B lub spadła "
				"poniżej 0B bez wyjątku");
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

	{
		Wallet w1(1), w2(2);
		Wallet suma2 = Wallet(2) - w2;
		assert(w2 == 4);
		assert(w2.opSize() == 2);
		assert(suma2 == 0);
		Wallet suma4 = Wallet(2) - Wallet(1); 
		assert(suma4.opSize() == 2);
		assert(suma4 == 1);
		
		w1 *= 3;
		2 < w2;
		Wallet zuch = 2 + w2;
	}

	{
		Wallet w1(1), w2(2);
		// Wallet suma1 = w1 + Wallet(1); // błąd kompilacji 1
		// Wallet suma2 = w1 + w2;  // błąd kompilacji 2
		// Wallet suma3 = w1; // błąd kompilacji 3
		// Wallet w3(true); // błąd kompilacji 4
		// Wallet w4('a'); // błąd kompilacji 5
	}

	{

		Wallet m1(5);
		usleep(5000);

		Wallet m2(4);
		assert(m1[0] < m2[0]);
		usleep(5000);

		m1 += Wallet(1);
		assert(m1[0] < m2[0]);
		assert(m1[1] > m2[0]);
		usleep(5000);

		Wallet m3(move(m1), move(m2));
		assert(m3.opSize() == 4);
		assert(m3[0].getUnits() == 5*UNITS_IN_B);
		assert(m3[1].getUnits() == 4*UNITS_IN_B);
		assert(m3[2].getUnits() == 6*UNITS_IN_B);
		assert(m3[3].getUnits() == 10*UNITS_IN_B);
		assert(m3[0] < m3[1] && m3[1] < m3[2] && m3[2] < m3[3]);
		assert(!(m3[1] <= m3[0]));

		cout << m3[0] << endl << m3[1] << endl << m3[2] << endl << m3[3] << endl;
	}

	{
		Wallet w1, w2;
		w1 = Wallet(4) += 2;
		// w2 = w1 += 2; // błąd kompilacji 6
	}
}
