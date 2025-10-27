//
// Created by nmila on 9/5/2025.
//

#ifndef ORDEREDPAIR_H
#define ORDEREDPAIR_H

class OrderedPair {

public:
	// X and Y values of ordered pair
	int x;
	int y;

	/**
	* Requires: x and y
	* Modifies: x and y
	* Effects: Constructor for OrderedPair. Sets x and y
	*/
	OrderedPair(int _x, int _y);

	/**
	* Requires: nothing
	* Modifies: x and y
	* Effects: Constructor for OrderedPair. Sets x and y both to 0
	*/
	OrderedPair();

	/**
	* Requires: 2 ordered pairs or and ordered pair and a constant
	* Modifies: nothing
	* Effects: arithmetic operators +, -, *, / orderedpairs as youd expect. boolean operators ==, !=, <, and > as youd expect.
	*/
	friend OrderedPair operator+(const OrderedPair &p1, const OrderedPair &p2);
	friend OrderedPair operator-(const OrderedPair &p1, const OrderedPair &p2);
	friend OrderedPair operator*(const OrderedPair &p1, const OrderedPair &p2);
	friend OrderedPair operator*(const OrderedPair &p1, double k);
	friend OrderedPair operator*(double k, const OrderedPair &p1);
	friend OrderedPair operator/(const OrderedPair &p1, const OrderedPair &p2);
	friend OrderedPair operator/(const OrderedPair &p1, double k);
	friend OrderedPair operator/(double k, const OrderedPair &p1);
	friend bool operator==(const OrderedPair &p1, const OrderedPair &p2);
	friend bool operator!=(const OrderedPair &p1, const OrderedPair &p2);
	friend bool operator<(const OrderedPair &p1, const OrderedPair &p2);
	friend bool operator>(const OrderedPair &p1, const OrderedPair &p2);
};

#endif //ORDEREDPAIR_H
