#ifndef RANDOM_H
#define RANDOM_H

#include <ctime>

class Random {
public:
	Random() { srand(time(0)); e = 10000; }
	inline int getRandi() { return rand(); }
	inline int getRandi(int n) { return rand() % n; }
	inline int getRandi(int a, int b) { return a + rand() % (b - a) ; }
	inline float getRandf() { return (rand() % e) / float(e); }
	inline float getRandf(float a, float b) { return a + (rand() % e) * (b - a) / float(e); }
	inline float getRandf1() { return getRandf(-1, 1); }
	void setPrecise(int ee) { e = ee; }
	void shuffle() { srand(time(0)); }
	int getPrecise() { return e; }
private:
	int e;
};


#endif
