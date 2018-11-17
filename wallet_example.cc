#include "wallet.h"

#include <cassert>
#include <iostream>

static const uint UNITS_IN_B = 100000000;

using std::move;

int main() {
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

    assert(Wallet(2) + w2 == Wallet(3));
    assert(Wallet(1) + Wallet(2) == Wallet(3));
    assert(1 + 2 == Wallet(3));

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

    Wallet w5{2};
    Wallet w6 = Wallet(10) - w5;
    assert(w5 == Wallet(4));
    assert(w6 == Wallet(8));
}
