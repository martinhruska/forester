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

/**
 * @file microcode_rev.cc
 * Reverse operations for microcode instructions.
 */

// Forester headers
#include "executionmanager.hh"
#include "microcode.hh"
#include "streams.hh"
#include "virtualmachine.hh"

SymState* FI_acc_sel::reverseAndIsect(
	ExecutionManager&                      execMan,
	const SymState&                        fwdPred,
	const SymState&                        bwdSucc) const
{
	(void)fwdPred;

	FA_WARN("Skipping reverse operation FI_acc_sel");
	return execMan.copyState(bwdSucc);
}

SymState* FI_acc_set::reverseAndIsect(
	ExecutionManager&                      execMan,
	const SymState&                        fwdPred,
	const SymState&                        bwdSucc) const
{
	(void)fwdPred;

	FA_WARN("Skipping reverse operation FI_acc_set");
	return execMan.copyState(bwdSucc);
}

SymState* FI_acc_all::reverseAndIsect(
	ExecutionManager&                      execMan,
	const SymState&                        fwdPred,
	const SymState&                        bwdSucc) const
{
	(void)fwdPred;

	FA_WARN("Skipping reverse operation FI_acc_all");
	return execMan.copyState(bwdSucc);
}

SymState* FI_pop_greg::reverseAndIsect(
	ExecutionManager&                      execMan,
	const SymState&                        fwdPred,
	const SymState&                        bwdSucc) const
{
	(void)fwdPred;

	FA_WARN("Skipping reverse operation FI_pop_greg");
	return execMan.copyState(bwdSucc);
}

SymState* FI_push_greg::reverseAndIsect(
	ExecutionManager&                      execMan,
	const SymState&                        fwdPred,
	const SymState&                        bwdSucc) const
{
	(void)fwdPred;

	FA_WARN("Skipping reverse operation FI_push_greg");
	return execMan.copyState(bwdSucc);
}

SymState* FI_abort::reverseAndIsect(
	ExecutionManager&                      execMan,
	const SymState&                        fwdPred,
	const SymState&                        bwdSucc) const
{
	(void)fwdPred;

	FA_WARN("Skipping reverse operation for FI_abort");
	return execMan.copyState(bwdSucc);
}

SymState* FI_node_free::reverseAndIsect(
	ExecutionManager&                      execMan,
	const SymState&                        fwdPred,
	const SymState&                        bwdSucc) const
{
	(void)fwdPred;

	FA_WARN("Skipping reverse operation FI_node_free");
	return execMan.copyState(bwdSucc);
}

SymState* FI_node_create::reverseAndIsect(
	ExecutionManager&                      execMan,
	const SymState&                        fwdPred,
	const SymState&                        bwdSucc) const
{
	(void)fwdPred;

	FA_WARN("Skipping reverse operation FI_node_create");
	return execMan.copyState(bwdSucc);
}

SymState* FI_store::reverseAndIsect(
	ExecutionManager&                      execMan,
	const SymState&                        fwdPred,
	const SymState&                        bwdSucc) const
{
	// Assertions
	assert(fwdPred.GetReg(dst_) == bwdSucc.GetReg(dst_));
	assert(fwdPred.GetReg(src_) == bwdSucc.GetReg(src_));

	const Data& treeRef = bwdSucc.GetReg(dst_);

	// load the old value of the memory node
	Data oldVal;
	VirtualMachine(*(fwdPred.GetFAE())).nodeLookup(
		treeRef.d_ref.root, treeRef.d_ref.displ + offset_, oldVal
	);

	// modify the FA 
	SymState* tmpState = execMan.copyState(bwdSucc);
	std::shared_ptr<FAE> fae = std::shared_ptr<FAE>(new FAE(*(tmpState->GetFAE())));

	Data tmp;            // necessary to call the nodeModify() method
	VirtualMachine(*fae).nodeModify(
		treeRef.d_ref.root, treeRef.d_ref.displ + offset_, oldVal, tmp
	);

	tmpState->SetFAE(fae);

	return tmpState;
}

SymState* FI_stores::reverseAndIsect(
	ExecutionManager&                      execMan,
	const SymState&                        fwdPred,
	const SymState&                        bwdSucc) const
{
	(void)fwdPred;

	FA_WARN("Skipping reverse operation for FI_stores");
	return execMan.copyState(bwdSucc);
}

SymState* FI_set_greg::reverseAndIsect(
	ExecutionManager&                      execMan,
	const SymState&                        fwdPred,
	const SymState&                        bwdSucc) const
{
	// Assertions
	assert(fwdPred.GetReg(src_) == bwdSucc.GetReg(src_));

	// load the old value of the desired global register
	Data oldVal = VirtualMachine(*(fwdPred.GetFAE())).varGet(dst_);

	// modify the FA 
	SymState* tmpState = execMan.copyState(bwdSucc);
	std::shared_ptr<FAE> fae = std::shared_ptr<FAE>(new FAE(*(tmpState->GetFAE())));
	VirtualMachine(*fae).varSet(dst_, oldVal);
	tmpState->SetFAE(fae);

	return execMan.copyState(bwdSucc);
}
