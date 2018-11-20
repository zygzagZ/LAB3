#include "wallet.h"
#include <bitset>
#include <algorithm>
#include <chrono>

namespace {
	
	/**
	 * @brief      Funkcja zwracająca czas w chwili wywołania
	 *
	 * @return     Czas
	 */
	uint64_t getTime() {
		using namespace std::chrono;
		return duration_cast<milliseconds>(
			system_clock::now().time_since_epoch()
		).count();
	}
	uint64_t moneyAmount = 0;///< Łączna liczba B w obiegu
}
/**
 * @brief      Konstruuje historię jednej operacji
 *
 * @param[in]  b     Stan przed oprecją
 * @param[in]  a     Stan po operacji
 * @param[in]  t     Moment wykonania operacji
 */
Operation::Operation(uint64_t b, uint64_t a, uint64_t t)
	: before(b), after(a), time(t) {
}

/**
 * @brief      Wypisuje na strumień os komunikat
 *
 *             Wypisuje na strumień os komunikat:
 *             "Wallet balance is b B after operation made at day d". 
 *             Liczba b jak przy wypisywaniu portfela. Czas d w formacie
 *             yyyy-mm-dd.
 *
 * @param      os    Strumień
 * @param[in]  op    Operacja
 *
 * @return     Referencja ostream
 */
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

/**
 * @brief      Funkcja zwracająca pusty portfel, który jest niemodyfikowalny
 *
 * @return     Pusty portfel
 */
const Wallet & Empty() {
	static const Wallet w;
	return w;
}

/**
 * @brief      Tworzy pusty portfel. Historia portfela ma jeden wpis.
 */
Wallet::Wallet() {
	money = 0;
	std::cerr << "Wallet()" << std::endl;
	*this = 0;
}

/**
 *@brief       Dekonstuktor. Usuwa portfel i zmniejsz liczbę monet w użyciu
 */
Wallet::~Wallet() {
	moneyAmount -= money;
	std::cerr << "~Wallet()" << std::endl;
}
	
/**
 * @brief      Tworzy portfel z n B, gdzie n jest liczbą naturalną. 
 *             
 *             Historia portfela ma jeden wpis.
 *
 * @param[in]  n     Liczba monet jaka ma się znaleźć w nowym portfelu
 */
Wallet::Wallet(int n) {
	money = 0;
	std::cerr << "Wallet(" << n << ")" << std::endl;
	*this = n * UNITS_IN_B;
}

/**
 * @brief      Tworzy portfel na podstawie napisu str określającego ilość B.
 *
 *             Napis może zawierać część ułamkową (do 8 miejsc po
 *             przecinku). Część ułamkowa jest oddzielona przecinkiem lub
 *             kropką. Białe znaki na początku i końcu napisu powinny być
 *             ignorowane. Historia portfela ma jeden wpis
 *
 * @param[in]  str   Napis określający liczbę B
 */
Wallet::Wallet(const char * _str) {
	std::string str(_str);
	money = 0;
	std::cerr << "Wallet(\"" << str << "\")" << std::endl;
	std::replace(str.begin(), str.end(), ',', '.');
	setUnits(std::stold(str) * UNITS_IN_B);
}

/**
 * @brief      Konstruktor przenoszący. 
 *             
 *             Historia operacji nowego portfela to historia operacji
 *             portfela w i jeden nowy wpis.
 *
 * @param[in]  w     Portfel, którego zawartość przenosimy
 */
Wallet::Wallet(Wallet &&w) {
	money = 0;
	std::cerr << "Wallet(&&w " << w << ")" << std::endl;
	*this = std::move(w);
}

/**
 * @brief      Tworzy portfel.
 *
 *             Tworzy portfel, którego historia operacji to suma historii
 *             operacji w1 i w2 plus jeden wpis, całość uporządkowana wg
 *             czasów wpisów. Po operacji w portfelu jest w1.getUnits() +
 *             w2.getUnits() jednostek, a portfele w1 i w2 są puste.
 *
 * @param[in]  w1  Pierwszy portfel
 * @param[in]  w2  Drugi porfel
 */
Wallet::Wallet(Wallet &&w1, Wallet &&w2) {
	std::cerr << "Wallet(&&" << w1 << ", &&" << w2 << ")" << std::endl;
	history = std::move(w1.history);
	history.reserve(opSize()+w2.opSize());
	auto it = history.end();
	std::copy(w2.history.begin(), w2.history.end(), 
		std::back_inserter(history));

	std::inplace_merge(history.begin(), it, history.end());
	auto sum = w1.getUnits() + w2.getUnits();
	w1.setUnits(0);
	w2.setUnits(0);
	setUnits(sum);
}

/**
 * @brief      Metoda klasowa tworząca portfel na podstawie napisu str.
 *             
 *             Napis jest zapisem ilości B w systemie binarnym. Kolejność
 *             bajtów jest grubokońcówkowa (ang. big endian).
 *
 * @param[in]  str   Zadany napis
 *
 * @return     Nowy porfel o liczbie B, odpowiadającej str.
 */
Wallet Wallet::fromBinary(std::string str) {
	std::cerr << "Wallet::fromBinary(\"" << str << "\")" << std::endl;
	return Wallet(static_cast<int>(std::bitset<20>(str).to_ulong()));
}

/**
 * @brief      Przemnożenie liczby pieniędzy w portfelu
 *
 * @param[in]  w     Stała przez którą mnożymy
 *
 * @return     Zaktualizowany portfel
 */
Wallet& Wallet::operator*=(int w) {
	std::cerr << *this << " *= " << w << std::endl;
	setUnits(money*w);

	return *this;
}

/**
 * @brief      Funkcja ustalająca liczbę B w portfelu
 *
 * @param[in]  newBalance  Nowy stan Bajtkomonet
 */
void Wallet::setUnits(int64_t w) {
	if (w < 0)
		throw std::logic_error("Liczba pieniędzy poniżej 0");

	if (moneyAmount - money + w > 21e6*UNITS_IN_B)
		throw std::logic_error("Liczba B przekroczyła 21 milionów");

	moneyAmount += w - money;

	history.emplace_back(money, w, getTime());
	money = w;
}

/**
 * @brief      Przypisanie przenoszące.
 *
 *             Jeżeli oba obiekty są tym samym obiektem, to nic nie robi,
 *             wpp. historia operacji to historia operacji w i jeden nowy
 *             wpis.
 *
 * @param[in]  w  Portfel
 *
 * @return     Zaktualizowany portfel
 */
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


/**
 * @brief      Operator dodawania
 *
 *             Po operacji w2 ma 0 B i dodatkowy wpis w historii, a w1 ma
 *             w1.getUnits() + w2.getUnits() jednostek i jeden dodatkowy wpis w
 *             historii.
 *
 * @param      w1         Pierwszy portfel
 * @param      w2         Drugi portfel
 *
 * @return     Zaktualizowany portfel
 */
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

/**
 * @brief      Odejmowanie portfeli
 *
 *             Po odejmowaniu w w2 jest dwa razy więcej jednostek, niż było w w2
 *             przed odejmowaniem
 *
 * @param      w2    Drugi portfel
 * @param      w1    Pierwszy portfel
 *
 * @return     Zaktualizowany portfel
 */
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


/**
 * @brief      Operacja +=
 *
 *             Po operacji w ma 0 B i dodatkowy wpis w historii, a l ma
 *             l.getUnits() + w.getUnits() jednostek i jeden dodatkowy wpis w
 *             historii.
 *
 * @param      l     Lewa strona znaku równości
 * @param      w     Prawa strona
 *
 * @return     Zaktualizowany portfel
 */
Wallet& operator+=(Wallet &l, Wallet &w) {
	std::cerr << "&" << l << " += &" << w << std::endl;
	auto sum = w.getUnits() + l.getUnits();
	w.setUnits(0);
	l.setUnits(sum);

	return l;
}

/**
 * @brief      Operacja -=
 *
 *             Po operacji w ma w.getUnits()*2 B i dodatkowy wpis w historii, a
 *             l ma l.getUnits() + w.getUnits() jednostek i jeden dodatkowy wpis
 *             w historii.
 *
 * @param      l     Lewa strona znaku -=
 * @param      w     Prawa strona znaku -=
 *
 * @return     Zaktualizowany portfel
 */
Wallet& operator-=(Wallet &l, Wallet &w) {
	std::cerr << "&" << l << " -= &" << w << std::endl;
	l.setUnits(l.getUnits() - w.getUnits());
	w.setUnits(w.getUnits()*2);

	return l;
}

/**
 * @brief      Wypisuje stan portfela na os
 *
 *             Wypisuje "Wallet[b B]" na strumień os, gdzie b to zawartość
 *             portfela w B. Wypisywana liczba jest bez białych znaków, bez zer
 *             wiodących oraz zer na końcu w rozwinięciu dziesiętnym oraz z
 *             przecinkiem jako separatorem dziesiętnym
 *
 * @param      os    Strumień
 * @param[in]  w     Portfel
 *
 * @return     Strumień
 */
std::ostream & operator<<(std::ostream &os, const Wallet &w) {
	os << "Wallet[" << ((double)w.getUnits()/Wallet::UNITS_IN_B) << " B]";
	return os;
}
