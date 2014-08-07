#include "vata_adapter.hh"
VATAAdapter::VATAAdapter(TreeAut aut) : vataAut_(aut)
{}

VATAAdapter::VATAAdapter() : vataAut_()
{}

VATAAdapter::VATAAdapter(const VATAAdapter& adapter) : vataAut_(adapter.vataAut_)
{}

VATAAdapter::~VATAAdapter()
{}

VATAAdapter VATAAdapter::createTAWithSameTransitions(
    const VATAAdapter&         ta)
{
    return VATAAdapter(TreeAut(ta.vataAut_, true, false));
}

VATAAdapter* VATAAdapter::allocateTAWithSameTransitions(
    const VATAAdapter&         ta)
{
    return new VATAAdapter(TreeAut(ta.vataAut_, true, false));
}
    
VATAAdapter VATAAdapter::createTAWithSameFinalStates(
    const VATAAdapter&         ta,
    bool                 copyFinalStates)
{
    return VATAAdapter(TreeAut(ta.vataAut_, true, copyFinalStates));
}

VATAAdapter* VATAAdapter::allocateTAWithSameFinalStates(
    const VATAAdapter&         ta,
    bool                 copyFinalStates)
{
    return new VATAAdapter(TreeAut(ta.vataAut_, true, copyFinalStates));
}

VATAAdapter::iterator VATAAdapter::begin() const
{
    return vataAut_.begin();
}

VATAAdapter::iterator VATAAdapter::end() const
{
	return vataAut_.end();
}

// TODO CHECK semantic against original implementation
typename VATAAdapter::DownAccessor::Iterator VATAAdapter::begin(
        size_t rhs) const
{
    return vataAut_[rhs].begin();
}

// TODO CHECK semantic against original implementation
typename VATAAdapter::DownAccessor::Iterator VATAAdapter::end(
        size_t rhs) const
{
    return vataAut_[rhs].end();
}

// TODO CHECK semantic against original implementation
typename VATAAdapter::DownAccessor::Iterator VATAAdapter::end(
        size_t rhs,
        DownAccessor::Iterator i) const
{
    return vataAut_[rhs].end();
}

typename VATAAdapter::AcceptTrans::Iterator VATAAdapter::accBegin() const
{
   return vataAut_.GetAcceptTrans().begin();
}

typename VATAAdapter::AcceptTrans::Iterator VATAAdapter::accEnd() const
{
   return vataAut_.GetAcceptTrans().end();
}

// TODO CHECK semantic against original implementation
typename VATAAdapter::AcceptTrans::Iterator VATAAdapter::accEnd(
        VATAAdapter::AcceptTrans::Iterator i) const
{
   return vataAut_.GetAcceptTrans().end();
}

VATAAdapter& VATAAdapter::operator=(const VATAAdapter& rhs)
{
    this->vataAut_ = rhs.vataAut_;
    return *this;
}

void VATAAdapter::addTransition(
		const std::vector<size_t>&          children,
		const SymbolType&                   symbol,
		size_t                              parent)
{
    this->vataAut_.AddTransition(children, symbol, parent);
}

void VATAAdapter::addTransition(const Transition& transition)
{
    this->vataAut_.AddTransition(transition);
}

const VATAAdapter::Transition VATAAdapter::getTransition(
        const std::vector<size_t>&          children,
		const SymbolType&                   symbol,
		size_t                              parent)

{
    if (vataAut_.ContainsTransition(children, symbol, parent))
    {
        return Transition(symbol, parent, children);
    }

    assert(false);
    return Transition();
}
	
const label_type VATAAdapter::GetSymbol(const Transition& t)
{
    return label_type(t.GetSymbol()); 
}

void VATAAdapter::addFinalState(size_t state)
{
    vataAut_.SetStateFinal(state);
}

void VATAAdapter::addFinalStates(const std::set<size_t>& states)
{
    vataAut_.SetStatesFinal(states);
}

void VATAAdapter::addFinalStates(const std::unordered_set<size_t>& states)
{
    for (size_t state : states)
    {
        vataAut_.SetStateFinal(state);
    }
}

bool VATAAdapter::isFinalState(size_t state) const
{
    return vataAut_.IsStateFinal(state);
}

const std::unordered_set<size_t>& VATAAdapter::getFinalStates() const
{
    return vataAut_.GetFinalStates();
}

size_t VATAAdapter::getFinalState() const
{
    return vataAut_.GetFinalState();
}
    
const VATAAdapter::Transition& VATAAdapter::getAcceptingTransition() const
{
    return *(vataAut_.GetAcceptTrans().begin());
}

// TODO: Is this correct?
VATAAdapter& VATAAdapter::unreachableFree(VATAAdapter& dst) const
{
    dst.vataAut_ = vataAut_.RemoveUnreachableStates();
    return dst;
}

// TODO: Is this correct?
VATAAdapter& VATAAdapter::uselessAndUnreachableFree(VATAAdapter& dst) const
{
    dst.vataAut_ = vataAut_.RemoveUselessStates();
    dst.vataAut_ = dst.vataAut_.RemoveUnreachableStates();
    return dst;
}

VATAAdapter& VATAAdapter::disjointUnion(
		VATAAdapter&                      dst,
		const VATAAdapter&                src,
		bool                              addFinalStates)
{
   dst.vataAut_ = TreeAut::UnionDisjointStates(
           dst.vataAut_, src.vataAut_, addFinalStates);

   return dst;
}

// TODO: Is this correct?
VATAAdapter& VATAAdapter::minimized(VATAAdapter& dst) const
{
    dst.vataAut_ = vataAut_.Reduce();
    return dst;
}

// TODO is this correct
bool VATAAdapter::areTransitionsEmpty()
{
    return vataAut_.AreTransitionsEmpty();
}

VATAAdapter& VATAAdapter::copyTransitions(VATAAdapter& dst) const
{
    CopyAllFunctor copyAllFunctor;
    dst.vataAut_.CopyTransitionsFrom(vataAut_, copyAllFunctor);
	return dst;
}
	
VATAAdapter& VATAAdapter::copyNotAcceptingTransitions(
        VATAAdapter&                       dst,
        const VATAAdapter&                 ta) const
{
    CopyNonAcceptingFunctor copyFunctor(ta);
    dst.vataAut_.CopyTransitionsFrom(vataAut_, copyFunctor);
	return dst;
}

void VATAAdapter::clear()
{
    vataAut_.Clear();
}

bool VATAAdapter::subseteq(const VATAAdapter& a, const VATAAdapter& b)
{
   return TreeAut::CheckInclusion(a.vataAut_, b.vataAut_); 
}

VATAAdapter& VATAAdapter::unfoldAtRoot(
    VATAAdapter&                   dst,
	size_t                         newState,
	bool                           registerFinalState) const
{
    if (registerFinalState)
    {
        dst.addFinalState(newState);
    }

    for (auto trans : vataAut_)
    {
        dst.addTransition(trans);
        if (isFinalState(trans.GetParent()))
            dst.addTransition(trans.GetChildren(), trans.GetSymbol(), newState);
    }

    return dst;
}


VATAAdapter& VATAAdapter::unfoldAtRoot(
    VATAAdapter&                                  dst,
    const std::unordered_map<size_t, size_t>&     states,
    bool                                          registerFinalState) const
{
    copyTransitions(dst);
    for (auto state : getFinalStates())
    {
        std::unordered_map<size_t, size_t>::const_iterator j = states.find(state);
        assert(j != states.end());

        for (auto trans : vataAut_[state])
        { // TODO: Check: is this semantic same as the original?
            dst.addTransition(trans.GetChildren(), trans.GetSymbol(), j->second);
        }

        if (registerFinalState)
        {
            dst.addFinalState(j->second);
        }
    }

    return dst;
}

void VATAAdapter::buildStateIndex(Index<size_t>& index) const
{
    for (auto trans : vataAut_)
    {
        for (auto state : trans.GetChildren())
        {
            index.add(state);
        }
        index.add(trans.GetParent());
    }

    for (size_t state : getFinalStates())
    {
        index.add(state);
    }
}

// TODO: check this if there will be problems (and they will)
VATAAdapter::TreeAut::AcceptTrans VATAAdapter::getEmptyRootTransitions() const
{
    return vataAut_.GetAcceptTrans();
}

void VATAAdapter::copyReachableTransitionsFromRoot(
    const VATAAdapter&        src,
    const size_t&             rootState)
{
    for (const Transition& k : src.vataAut_[rootState])
    {
        addTransition(k);
    }
}

// collapses states according to a given relation
VATAAdapter& VATAAdapter::collapsed(
    VATAAdapter&                             dst,
    const std::vector<std::vector<bool>>&    rel,
    const Index<size_t>&                     stateIndex) const
{
    return dst;

    /*
    std::vector<size_t> headIndex;
    utils::relBuildClasses(rel, headIndex);

    std::ostringstream os;
    utils::printCont(os, headIndex);

    // TODO: perhaps improve indexing
    std::vector<size_t> invStateIndex(stateIndex.size());
    for (Index<size_t>::iterator i = stateIndex.begin(); i != stateIndex.end(); ++i)
    {
        invStateIndex[i->second] = i->first;
    }

    for (std::vector<size_t>::iterator i = headIndex.begin(); i != headIndex.end(); ++i)
    {
        *i = invStateIndex[*i];
    }

    for (const size_t& state : finalStates_)
    {
        dst.addFinalState(headIndex[stateIndex[state]]);
    }

    for (const TransIDPair* trans : this->transitions_)
    {
        std::vector<size_t> lhs;
        stateIndex.translate(lhs, trans->first.lhs());
        for (size_t j = 0; j < lhs.size(); ++j)
            lhs[j] = headIndex[lhs[j]];
        dst.addTransition(lhs, trans->first.label(), headIndex[stateIndex[trans->first.rhs()]]);
        std::ostringstream os;
        utils::printCont(os, lhs);
    }
    return dst;
    */
}
