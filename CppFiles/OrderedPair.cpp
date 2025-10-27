//
// Created by nmila on 9/5/2025.
//

#include "../HeaderFiles/OrderedPair.h"
#include <string>
using namespace std;

OrderedPair::OrderedPair(int _x, int _y) {
	x = _x;
	y = _y;
}

OrderedPair::OrderedPair() {
	x = 0;
	y = 0;
}

OrderedPair operator+(const OrderedPair &p1, const OrderedPair &p2) {return OrderedPair(p1.x + p2.x, p1.y + p2.y);}
OrderedPair operator-(const OrderedPair &p1, const OrderedPair &p2) {return OrderedPair(p1.x - p2.x, p1.y - p2.y);}
OrderedPair operator*(const OrderedPair &p1, const OrderedPair &p2) {return OrderedPair(p1.x * p2.x, p1.y * p2.y);}
OrderedPair operator*(const OrderedPair &p1, double k) {return OrderedPair(p1.x * k, p1.y * k);}
OrderedPair operator*(double k, const OrderedPair &p1) {return OrderedPair(p1.x * k, p1.y * k);}
OrderedPair operator/(const OrderedPair &p1, const OrderedPair &p2) {return OrderedPair(p1.x / p2.x, p1.y / p2.y);}
OrderedPair operator/(const OrderedPair &p1, double k) {return OrderedPair(p1.x / k, p1.y / k);}
OrderedPair operator/(double k, const OrderedPair &p1) {return OrderedPair(p1.x / k, p1.y / k);}
bool operator==(const OrderedPair &p1, const OrderedPair &p2) {return p1.x == p2.x && p1.y == p2.y;}
bool operator!=(const OrderedPair &p1, const OrderedPair &p2) {return p1.x != p2.x || p1.y != p2.y;}
bool operator<(const OrderedPair &p1, const OrderedPair &p2) {
	if (p1.x != p2.x) { return p1.x < p2.x; }
	return p1.y < p2.y;
}
bool operator>(const OrderedPair &p1, const OrderedPair &p2) {
	if (p1.x != p2.x) { return p1.x > p2.x; }
	return p1.y > p2.y;
}
