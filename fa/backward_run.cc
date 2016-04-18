/*
 * Copyright (C) 2012-2015  Ondrej Lengal, Martin Hruska
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

// Forester headers
#include "backward_run.hh"
#include "memplot.hh"
#include "streams.hh"
#include "fixpoint.hh"

namespace
{
	void printDebugInfo(
			const AbstractInstruction *instr,
			const SymState *fwdState,
			const SymState *bwdState)
	{
		for (size_t i = 0; i < 10; ++i)
		{
			FA_DEBUG_AT(1, "");
		}

		FA_DEBUG_AT(1, "instruction: " << *instr);
		FA_DEBUG_AT(1, "fwd pred state: " << *fwdState);
		FA_DEBUG_AT(1, "bwd succ state: " << *bwdState);
	}
}

bool BackwardRun::isSpuriousCE(
	const SymState::Trace&                           fwdTrace,
	SymState*&                                       failPoint,
	std::vector<std::shared_ptr<const TreeAut>>&     predicate)
{
	// Assertions
	assert(!fwdTrace.empty());
	assert(nullptr == failPoint);
	assert(predicate.empty());

	// iterator from the leaf of the forward run
	auto itFwdTrace = fwdTrace.cbegin();

	// the backward trace
	std::vector<SymState*> bwdTrace;

	// check that the iterator does not point to a nullptr
	assert(nullptr != *itFwdTrace);

	// copy the error state
	SymState* bwdState = execMan_.copyState(**itFwdTrace);

	bwdTrace.push_back(bwdState);

	while (++itFwdTrace != fwdTrace.cend())
	{	// until we hit the initial state of the execution trace
		SymState* fwdState = const_cast<SymState*>(*itFwdTrace);
		assert(nullptr != fwdState);

		const AbstractInstruction* instr = fwdState->GetInstr();
		assert(nullptr != instr);

		printDebugInfo(instr, fwdState, bwdState);
		SymState* resultState = instr->reverseAndIsect(execMan_, *fwdState, *bwdState);
		assert(nullptr != resultState);

		if (resultState->GetFAE()->Empty())
		{	// in case the intersection is empty - spurious counterexample
			failPoint = fwdState;
			predicate.insert(
					predicate.end(),
					resultState->GetPredicates().begin(),
					resultState->GetPredicates().end());
			assert(!predicate.empty());
			//learnPredicates(fwdState, bwdState, predicate);
			return true;
		}

		bwdTrace.push_back(resultState);
		bwdState = resultState;

//		std::string filename = MemPlotter::plotHeap(*resultState);
	}

	for (SymState* st : bwdTrace)
	{	// recycle states on the trace
		assert(nullptr != st);

		execMan_.recycleState(st);
	}

	return false;
}
