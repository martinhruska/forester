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

#ifndef FIXPOINT_H
#define FIXPOINT_H

// Standard library headers
#include <vector>
#include <memory>

// Forester headers
#include "boxman.hh"
#include "fixpointinstruction.hh"
#include "forestautext.hh"
#include "ufae.hh"

/**
 * @brief  The base class for fixpoint instructions
 */
class FixpointBase : public FixpointInstruction
{
protected:

	/// Fixpoint configuration obtained in the forward run
	TreeAut fwdConf_;

	FAE accs_;

	UFAE fwdConfWrapper_;

	std::vector<std::shared_ptr<const FAE>> fixpoint_;

	TreeAut &ta_;

	BoxMan &boxMan_;


protected:

	size_t fold(
			const std::shared_ptr<FAE>&  fae,
			std::set<size_t>&            forbidden,
			SymState::AbstractionInfo&   ainfo);

public:

	virtual void extendFixpoint(const std::shared_ptr<const FAE>& fae)
	{
		fixpoint_.push_back(fae);
	}

	virtual void clear()
	{
		fixpoint_.clear();
		fwdConf_.clear();
		fwdConfWrapper_.clear();
		accs_.clear();
		accs_.appendRoot(accs_.allocTA());
	}

#if 0
	void recompute()
	{
		fwdConf_.clear();
		fwdConfWrapper_.clear();
		TreeAut ta(*fwdConf_.backend);
		Index<size_t> index;

		for (auto fae : fixpoint_)
		{
			fwdConfWrapper_.fae2ta(ta, index, *fae);
		}

		if (!ta.areTransitionsEmpty())
		{
			fwdConfWrapper_.adjust(index);
			ta.minimized(fwdConf_);
		}
	}
#endif

public:

	FixpointBase(
		const CodeStorage::Insn*           insn,
		TreeAut&                           fixpoint,
		TreeAut&                           ta,
		BoxMan&                            boxMan) :
		FixpointInstruction(insn),
		fwdConf_(fixpoint),
		accs_(fixpoint, boxMan),
		fwdConfWrapper_(fwdConf_, boxMan),
		fixpoint_{},
		ta_(ta),
		boxMan_(boxMan)
	{
		accs_.appendRoot(accs_.allocTA());
	}

	virtual ~FixpointBase()
	{ }

	virtual const TreeAut& getFixPoint() const
	{
		return fwdConf_;
	}

	virtual SymState* reverseAndIsect(
		ExecutionManager&                      execMan,
		const SymState&                        fwdPred,
		const SymState&                        bwdSucc) const;
};

/**
 * @brief  Computes a fixpoint with abstraction
 *
 * Computes a fixpoint, employing abstraction. During computation, new
 * convenient boxes are learnt.
 */
class FI_abs : public FixpointBase
{
private:  // data members

	/// The set of FA used for the predicate abstraction
	std::vector<std::shared_ptr<const TreeAut>> predicates_;

private:  // methods

	/**
	 * @brief  Performs abstraction on the given automaton
	 *
	 * This method performs abstraction on the given forest automaton.
	 *
	 * @param[in,out]  fae  The forest  automaton on which the abstraction is to
	 *                      be performed
	 */
	void abstract(
		FAE&             fae);

	void strongFusion(
		FAE&             fae);

	void weakFusion(
		FAE&             fae);

	void loadFaeToAccs(
		FAE&       fae);

public:   // methods

	FI_abs(
		const CodeStorage::Insn*       insn,
		TreeAut&                       fixpoint,
		TreeAut&                       ta,
		BoxMan&                        boxMan) :
		FixpointBase(insn, fixpoint, ta, boxMan),
		predicates_()
	{ }

	/**
	 * @brief  Adds a new predicate to abstraction
	 *
	 * This method adds a new predicate to the abstrastion performed at this
	 * instruction.
	 *
	 * @param[in]  predicate  The predicate to be added
	 */
	void addPredicate(std::vector<std::shared_ptr<const TreeAut>>& predicate)
	{
		predicates_.insert(predicates_.end(), predicate.begin(), predicate.end());
	}

	/**
	 * @brief  Retrieves the predicates
	 *
	 * This method returns the container with predicate forest automata.
	 *
	 * @returns  Container with predicate forest automata
	 */
	const std::vector<std::shared_ptr<const TreeAut>>& getPredicates() const
	{
		return predicates_;
	}

	/**
	 * @brief Print predicates
	 * 
	 * This methods prints the predicates
	 */
	void printPredicates(std::ostringstream& os) const;

	/**
	 * @brief Print info about predicates in this instruction.
	 *
	 * Print debug info about predicates in this instruction
	 *
	 */
	void printDebugInfoAboutPredicates() const;

	virtual void execute(ExecutionManager& execMan, SymState& state);

	virtual std::ostream& toStream(std::ostream& os) const {
		return os << "abs   \t";
	}

	static std::vector<std::shared_ptr<const TreeAut>> learnPredicates(
			SymState*			                             fwdState,
			SymState*			                             bwdState
	);
};

/**
 * @brief  Computes a fixpoint without abstraction
 *
 * Computes a fixpoint without the use of abstraction.
 */
class FI_fix : public FixpointBase
{
public:

	FI_fix(
		const CodeStorage::Insn*           insn,
		TreeAut&                           fixpoint,
		TreeAut&                           ta,
		BoxMan&                            boxMan) :
		FixpointBase(insn, fixpoint, ta, boxMan)
	{ }

	virtual void execute(ExecutionManager& execMan, SymState& state);

	virtual std::ostream& toStream(std::ostream& os) const {
		return os << "fix   \t";
	}
};

#endif
