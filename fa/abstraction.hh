/*
 * Copyright (C) 2011 Jiri Simacek
 *
 * This file is part of forester.
 *
 * forester is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * forester is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with forester.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ABSTRACTION_H
#define ABSTRACTION_H

// Forester headers
#include "forestautext.hh"
#include "streams.hh"

/**
 * @brief  The class that performs abstraction
 *
 * This class peforms abstraction over tree automata.
 */
class Abstraction
{
private:  // data members

	FAE& fae_;

public:   // methods

	/**
	 * @brief  Performs finite height abstraction
	 *
	 * This method performs the finite height abtraction over a tree automaton at
	 * index @p root of the forest automaton. The abstraction is for @p height.
	 *
	 * @param[in]  root    The root on which the abstraction is to be applied
	 * @param[in]  height  The height of the abstraction
	 * @param[in]  f       Functor for matching transitions
	 */
	template <class F>
	void heightAbstraction(
		size_t                root,
		size_t                height,
		F                     f)
	{
		FA_DEBUG_AT(1,"Height abstraction of height " << height);
		// Preconditions
		assert(root < fae_.getRootCount());
		assert(nullptr != fae_.getRoot(root));
        
		const TreeAut& rootTA = *fae_.getRoot(root);
		std::unordered_map<size_t, size_t> rel;

		// compute the abstraction (i.e. which states are to be merged)
		ConnectionGraph::StateToCutpointSignatureMap stateMap;
		ConnectionGraph::computeSignatures(stateMap, rootTA);

		auto cutpointCmp = [&stateMap](
			const size_t state1,
			const size_t state2) -> bool {
			return stateMap[state1] % stateMap[state2];
		};

		rootTA.heightAbstraction(rel, height, f, cutpointCmp);


		TreeAut ta = fae_.createTAWithSameBackend();
		rootTA.collapsed(ta, rel);
		assert(areFinalStatesPreserved(rootTA, ta));

		fae_.setRoot(root, std::shared_ptr<TreeAut>(fae_.allocTA()));
		ta.uselessAndUnreachableFree(*fae_.getRoot(root));
	}

	template <class F>
	static std::shared_ptr<TreeAut> heightAbstractionTA(
		TreeAut&              inTA,
		size_t                height,
		F                     f)
	{
		FA_DEBUG_AT(1,"Height abstraction of height " << height);
		// Preconditions

		const TreeAut& rootTA = inTA;
		std::unordered_map<size_t, size_t> rel;

		// compute the abstraction (i.e. which states are to be merged)
		ConnectionGraph::StateToCutpointSignatureMap stateMap;
		ConnectionGraph::computeSignatures(stateMap, rootTA);

		auto cutpointCmp = [&stateMap](
			const size_t state1,
			const size_t state2) -> bool {
			return stateMap[state1] % stateMap[state2];
		};

		rootTA.heightAbstraction(rel, height, f, cutpointCmp);


		std::shared_ptr<TreeAut> ta = std::shared_ptr<TreeAut>(new TreeAut());
		rootTA.collapsed(*ta, rel);

		std::shared_ptr<TreeAut> result = std::shared_ptr<TreeAut>(new TreeAut());
		ta->uselessAndUnreachableFree(*result);

		return result;
	}

    bool areFinalStatesPreserved(const TreeAut& old, const TreeAut& reduced)
    {
        bool res = true;
        Index<size_t> reducedAutIndex;
        reduced.buildStateIndex(reducedAutIndex);
        for (size_t state : old.getFinalStates())
        { // does it really hold?
            res &= !reducedAutIndex.find(state).second || reduced.isFinalState(state);
        }

        return res;
    }


	/**
	 * @brief  Performs predicate abstraction
	 *
	 * This method performs predicate abstraction of a forest automaton using
	 * a set of predicates (currently only one predicate).
	 *
	 * @param[in]  predicates  The predicates used for the abstraction
	 */
	void predicateAbstraction(
		const size_t                                          abstrRoot,
		const std::vector<std::shared_ptr<const TreeAut>>&    predicates,
		std::ostringstream&                                   oss);


public:

	Abstraction(FAE& fae) :
		fae_(fae)
	{ }
};

#endif
