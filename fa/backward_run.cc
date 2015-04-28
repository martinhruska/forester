/*
 * Copyright (C) 2012  Ondrej Lengal
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

namespace
{
	std::vector<std::shared_ptr<const TreeAut>> getEmptyTrees(
			const FAE&            fwdFAE,
			const FAE&            bwdFAE)
	{
		assert(fwdFAE.getRootCount() == bwdFAE.getRootCount());
		FA_DEBUG_AT(1, "empty input fwd " << fwdFAE);
		FA_DEBUG_AT(1, "empty input bwd " << bwdFAE);

		std::vector<std::shared_ptr<const TreeAut>> res;

		for (size_t i = 0; i < fwdFAE.getRootCount(); ++i)
		{
			TreeAut isectTA = TreeAut::intersectionBU(
					*(fwdFAE.getRoot(i)),*(bwdFAE.getRoot(i)));
			TreeAut finalIsectTA;
			isectTA.uselessAndUnreachableFree(finalIsectTA);
			FA_DEBUG_AT(1, "empty " << isectTA);

			if (finalIsectTA.areTransitionsEmpty())
			{
				res.push_back(bwdFAE.getRoot(i));
			}
		}


		return res;
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

		for (size_t i = 0; i < 10; ++i)
		{
			FA_DEBUG_AT(1, "");
		}

		FA_DEBUG_AT(1, "instruction: " << *instr);
		FA_DEBUG_AT(1, "fwd pred state: " << *fwdState);
		FA_DEBUG_AT(1, "bwd succ state: " << *bwdState);

		SymState* resultState = instr->reverseAndIsect(execMan_, *fwdState, *bwdState);
		assert(nullptr != resultState);

		if (resultState->GetFAE()->Empty())
		{	// in case the intersection is empty - spurious counterexample
			failPoint = fwdState;
			
			SymState* tmpState = execMan_.copyStateWithNewRegs(*bwdState, fwdState->GetInstr());
			// perform intersection
			tmpState->Intersect(*fwdState);

			FA_NOTE(*(bwdState->GetFAE()));
			std::shared_ptr<FAE> normFAEBwd = bwdState->newNormalizedFAE();
			std::shared_ptr<FAE> normFAEFwd = fwdState->newNormalizedFAE();
			assert(normFAEFwd->getRootCount() == normFAEBwd->getRootCount());
			
			predicate = getEmptyTrees(*normFAEFwd, *normFAEBwd);
			if (predicate.empty())
			{
				predicate.insert(predicate.end(),
						normFAEBwd->getRoots().begin(), normFAEBwd->getRoots().end());
			}

			assert(!predicate.empty()); // TODO this could be condition of spuriousness

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
