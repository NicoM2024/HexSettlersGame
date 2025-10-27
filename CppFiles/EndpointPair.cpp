//
// Created by nmila on 9/6/2025.
//

#include "../HeaderFiles/EndpointPair.h"

using std::shared_ptr;

// Constructor required pointers to two different plots
EndpointPair::EndpointPair(shared_ptr<Plot> _plot_a, shared_ptr<Plot> _plot_b) {
    plot_a = _plot_a;
    plot_b = _plot_b;
}

// Default constructor constructs an invalid endpoint pair
EndpointPair::EndpointPair(){
    plot_a = nullptr;
    plot_b = nullptr;
}

// Is only valid if both plots are not nullptr
bool EndpointPair::is_valid() const {
    return plot_a == nullptr || plot_b == nullptr;
}

// Returns true if either of the plots are equal to the provided plot
bool EndpointPair::has(const shared_ptr<Plot> &plot) const {
    return plot_a == plot || plot_b == plot;
}

// Overloaded operators for comparing and modifying ordered pairs
// for ==, it doesn't matter what order the plots are int
bool operator==(const EndpointPair &p1, const EndpointPair &p2) {
    return (p1.plot_a == p2.plot_a && p1.plot_b == p2.plot_b) || (p1.plot_a == p2.plot_b && p1.plot_b == p2.plot_a);
}
bool operator!=(const EndpointPair &p1, const EndpointPair &p2) {
    return !(p1 == p2);
}