//
// Created by nmila on 9/6/2025.
//

#ifndef EndpointPAIR_H
#define EndpointPAIR_H

#include <memory>
#include "Plot.h"

/*
 * The EndpointPair class contains two points to plots and it meant to provide
 *  a path with the two plots it is bounded by
 *
 */

class EndpointPair {
    public:
		// pointers to both plots making up the pair
        std::shared_ptr<Plot> plot_a;
		std::shared_ptr<Plot> plot_b;

		/**
		 * Requires: 2 pointers to plots
		 * Modifies: plot_a, plot_b
		 * Effects: Constructor for Construct, sets plots to provided ones
		 */
		EndpointPair(std::shared_ptr<Plot> _plot_a, std::shared_ptr<Plot> _plot_b);

		/**
		* Requires: nothing
		* Modifies: plot_a, plot_b
		* Effects: Constructor for Construct, sets plots to nullptr
		*/
		EndpointPair();

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: returns false if either of the plots are nullptr
		 */
		bool is_valid() const;

		/**
		 * Requires: plot
		 * Modifies: nothing
		 * Effects: returns if one of the plots is the provided one
		 */
		bool has(const std::shared_ptr<Plot> &plot) const;

		/**
		 * Requires: two enpoint pairs
		 * Modifies: nothing
		 * Effects: returns if the first pair is ==, and != to the second respectively based on the
		 * internal orderedpairs stored by plot_a and plot_b
		 */
		friend bool operator==(const EndpointPair &p1, const EndpointPair &p2);
		friend bool operator!=(const EndpointPair &p1, const EndpointPair &p2);

};
#endif //EndpointPAIR_H
