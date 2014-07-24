/*
 * Copyright (C) 2014 Martin Hruska
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

#ifndef VATA_ABSTRACTION_H
#define VATA_ABSTRACTION_H

// std headers
#include <vector>

// Forester header
#include "utils.hh"

class VATAAbstraction
{
public:
    // currently erases '1' from the relation
	template <class A, class F>
	static void heightAbstraction(
        const A&                                   aut,
		std::vector<std::vector<bool>>&            result,
		size_t                                     height,
		F                                          f,
		const Index<size_t>&                       stateIndex)
	{
		std::vector<std::vector<bool>> tmp;

		while (height--)
		{
			tmp = result;

			for (Index<size_t>::iterator i = stateIndex.begin(); i != stateIndex.end(); ++i)
			{
				const size_t& state1 = i->second;
				for (Index<size_t>::iterator k = stateIndex.begin(); k != stateIndex.end(); ++k)
				{
					const size_t& state2 = k->second;
					if (!VATAAbstraction::areStatesEquivalent(
                                aut, state1, state2, f, stateIndex, tmp))
                    {
						result[state1][state2] = false;
                    }
				}
			}
		}

        VATAAbstraction::completeSymmetricIndex(result);
	}

private:
    /**
	 * @brief  Determines whether two transitions_ match
	 *
	 * This function determines whether two transitions_ match (and can therefore
	 * e.g. be merged during abstraction). First, the @p funcMatch functor is used
	 * to determine whether the transitions_ are to be checked at all.
	 */
	template <class T, class F>
	static bool transMatch(
		const T&                                  trans1,
		const T&                                  trans2,
		F                                         funcMatch,
		const std::vector<std::vector<bool>>&     mat,
		const Index<size_t>&                      stateIndex)
	{
		if (!funcMatch(trans1, trans2))
			return false;

		if (trans1.GetChildrenSize() != trans2.GetChildrenSize())
			return false;

		for (size_t m = 0; m < trans1.GetChildrenSize(); ++m)
		{
			if (!mat[stateIndex[trans1.GetNthChildren(m)]][stateIndex[trans2.GetNthChildren(m)]])
			{
				return false;
			}
		}

		return true;
	}

    template <class A, class F>
    static bool areStatesEquivalent(
            const A&                       aut,
            int                            state1,
            int                            state2,
            F                              f,
		    const Index<size_t>&           stateIndex,
            std::vector<std::vector<bool>> tmp)
    {
        if ((state1 == state2) || !tmp[state1][state2])
            return false;

        for (const typename A::Transition& trans1 : aut[state1])
        {
            for (const typename A::Transition& trans2 : aut[state2])
            {
                if (!VATAAbstraction::transMatch(
                            trans1, trans2, f, tmp, stateIndex))
                {
                    return false;
                }
            }
        }

        return true;
    }

    static void completeSymmetricIndex(std::vector<std::vector<bool>>& result);
};

#endif