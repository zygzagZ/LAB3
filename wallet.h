#ifndef __WALLET_H__
#define __WALLET_H__
#include <iostream>
#include <vector>

/**
 * @brief      Klasa opisująca historie jednej operacji
 */
class Operation {
public:
	
	/**
	 * @brief      Konstruuje historię jednej operacji
	 *
	 * @param[in]  b     Stan przed oprecją
	 * @param[in]  a     Stan po operacji
	 * @param[in]  t     Moment wykonania operacji
	 */
	Operation(uint64_t b, uint64_t a, uint64_t t);
	
	/**
	 * @brief      Zwraca liczbę jednostek w portfelu po operacji
	 *
	 * @return     Liczba jednostek
	 */
	uint64_t getUnits() const { return after; }
	
	/**
	 * @brief      Implementacja operacji o1 op o2 z treści zadania
	 */
	operator uint64_t() const { return time; }
private:
	uint64_t before;///< Stan przed wykonaniem operacji
	uint64_t after;///< Stan po wykonaniu operacji
	uint64_t time;///< Moment wykonania operacji

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
	friend std::ostream & operator<<(std::ostream &os, const Operation &op);
};

/**
 * @brief      Klasa portfel, przechowująca Bajtkomonety i historię operacji
 */
class Wallet {
public:
	static const int64_t UNITS_IN_B = 1e8;///< Liczba jednostek w 1B

	/**
	 * @brief      Tworzy pusty portfel. Historia portfela ma jeden wpis.
	 */
	Wallet();
	
	/**
	 *@brief       Dekonstuktor
	 */
	~Wallet();


template<
    typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>   Wallet(T&&) = delete;

	/**
	 * @brief      Konstruktor przenoszący. 
	 *             
	 *             Historia operacji nowego portfela to historia operacji
	 *             portfela w i jeden nowy wpis.
	 *
	 * @param[in]  w     Portfel, którego zawartość przenosimy
	 */
	Wallet(Wallet &&w);
	
	/**
	 * @brief      Tworzy portfel z n B, gdzie n jest liczbą naturalną. 
	 *             
	 *             Historia portfela ma jeden wpis.
	 *
	 * @param[in]  n     Liczba monet jaka ma się znaleźć w nowym portfelu
	 */
	Wallet(int n);
	
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
	Wallet(const char *str);
	
	/**
	 * @brief      Tworzy portfel.
	 *
	 *             Tworzy portfel, którego historia operacji to suma historii
	 *             operacji w1 i w2 plus jeden wpis, całość uporządkowana wg
	 *             czasów wpisów. Po operacji w w0 jest w1.getUnits() +
	 *             w2.getUnits() jednostek, a portfele w1 i w2 są puste.
	 *
	 * @param[in]  w1  Pierwszy portfel
	 * @param[in]  w2  Drugi porfel
	 */
	Wallet(Wallet &&w1, Wallet &&w2);

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
	static Wallet fromBinary(std::string str);


	Wallet& operator=(int64_t w);
	
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
	Wallet& operator=(Wallet &&w);
	
	/**
	 * @brief      Przemnożenie liczby pieniędzy w portfelu
	 *
	 * @param[in]  w     Stała przez którą mnożymy
	 *
	 * @return     Zaktualizowany portfel
	 */
	Wallet& operator*=(int w);

	/**
	 * @brief      Funkcja zwracająca operacją znajdującą się pod indeksem
	 *             index
	 *
	 * @param[in]  index  Indeks
	 *
	 * @return     Operacja znajdująca się pod indeksem index
	 */
	const Operation & operator[] (int index) const {
		return history[index];
	}

	/**
	 * @brief      Funkcja zwracająca liczbę operacji wykonanych na portfelu
	 *
	 * @return     Liczba operacji
	 */
	size_t opSize() const { return history.size(); }
	
	/**
	 * @brief      Funkcja zwracająca liczbę jednostek
	 *
	 * @return     Liczba jednostek
	 */
	uint64_t getUnits() const { return money; }

private:
	uint64_t money; ///<Liczba jednostek w portfelu
	std::vector<Operation> history;///< Historia operacji wykonanych na Wallecie
	
	/**
	 * @brief      Funkcja ustalająca liczbę B w portfelu
	 *
	 * @param[in]  newBalance  Nowy stan Bajtkomonet
	 */
	void setUnits(int64_t newBalance);

	friend Wallet operator+(Wallet &&w1, Wallet &w2);
	friend Wallet operator+(Wallet &&w1, Wallet &&w2);
	friend Wallet operator-(Wallet &&w1, Wallet &w2);
	friend Wallet operator-(Wallet &&w1, Wallet &&w2);
	friend bool operator==(const Wallet &l, const Wallet &r);
	friend bool operator!=(const Wallet &l, const Wallet &r);
	friend bool operator<(const Wallet &l, const Wallet &r);
	friend bool operator<=(const Wallet &l, const Wallet &r);
	friend bool operator>(const Wallet &l, const Wallet &r);
	friend bool operator>=(const Wallet &l, const Wallet &r);

	friend Wallet& operator+=(Wallet &lhs, Wallet &w);
	friend Wallet& operator+=(Wallet &lhs, Wallet &&w);
	friend Wallet&& operator+=(Wallet &&lhs, Wallet &w);
	friend Wallet&& operator+=(Wallet &&lhs, Wallet &&w);
	friend Wallet& operator-=(Wallet &lhs, Wallet &w);
	friend Wallet& operator-=(Wallet &lhs, Wallet &&w);
	friend Wallet&& operator-=(Wallet &&lhs, Wallet &w);
	friend Wallet&& operator-=(Wallet &&lhs, Wallet &&w);

};

/**
 * @brief      Funkcja zwracająca pusty portfel, który jest niemodyfikowalny
 *
 * @return     Pusty portfel
 */
const Wallet & Empty();

Wallet operator+(Wallet &&w1, Wallet &w2);
Wallet operator+(Wallet &&w1, Wallet &&w2);

/// Implementacja operatorów porównania
inline bool operator==(const Wallet &l, const Wallet &r) { return l.money == r.money; }
inline bool operator!=(const Wallet &l, const Wallet &r) { return l.money != r.money; }
inline bool operator<(const Wallet &l, const Wallet &r) { return l.money < r.money; }
inline bool operator<=(const Wallet &l, const Wallet &r) { return l.money <= r.money; }
inline bool operator>(const Wallet &l, const Wallet &r) { return l.money > r.money; }
inline bool operator>=(const Wallet &l, const Wallet &r) { return l.money >= r.money; }


Wallet& operator+=(Wallet &l, Wallet &w);
Wallet& operator-=(Wallet &l, Wallet &w);

inline Wallet&& operator+=(Wallet &&l, Wallet &w) { return std::move(l += w); }
inline Wallet&& operator+=(Wallet &&l, Wallet &&w) { return std::move(l += w); }
inline Wallet& operator+=(Wallet &l, Wallet &&w) { return l += w; }
inline Wallet& operator-=(Wallet &l, Wallet &&w) { return l -= w; }
inline Wallet&& operator-=(Wallet &&l, Wallet &w) { return std::move(l -= w); }
inline Wallet&& operator-=(Wallet &&l, Wallet &&w) { return std::move(l += w); }


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
std::ostream & operator<<(std::ostream &os, const Wallet &w);

#endif