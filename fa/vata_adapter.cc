#include "vata_adapter.hh"

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <utility>

VATAAdapter::VATAAdapter(TreeAut aut) : vataAut_(aut), orig_()
{}

VATAAdapter::VATAAdapter() : vataAut_(), orig_()
{}

VATAAdapter::VATAAdapter(const VATAAdapter& adapter) : vataAut_(adapter.vataAut_),
    orig_(adapter.orig_)
{}

VATAAdapter::~VATAAdapter()
{}

VATAAdapter VATAAdapter::createTAWithSameTransitions(
    const VATAAdapter&         ta)
{
    FA_DEBUG_AT(1,"Create TA with same transitions\n");
    VATAAdapter t(VATAAdapter(TreeAut(ta.vataAut_, false, false)));
    t.orig_ = TA::createTAWithSameTransitions(ta.orig_);
    assert(equal(t.vataAut_,t.orig_));
    return t;
}

VATAAdapter* VATAAdapter::allocateTAWithSameTransitions(
    const VATAAdapter&         ta)
{
    FA_DEBUG_AT(1,"Allocate TA with same transitions\n");
    VATAAdapter* res = new VATAAdapter(TreeAut(ta.vataAut_, false, false));
    res->orig_ = ta.orig_;
    res->orig_ = TA::createTAWithSameTransitions(ta.orig_);
    assert(equal(res->vataAut_, res->orig_));
    return res;
}

VATAAdapter VATAAdapter::createTAWithSameFinalStates(
    const VATAAdapter&         ta,
    bool                       copyFinalStates)
{
    FA_DEBUG_AT(1,"Create TA with same final states\n");
    VATAAdapter t(TreeAut(ta.vataAut_, true, copyFinalStates));
    t.orig_ = TA::createTAWithSameFinalStates(ta.orig_, copyFinalStates);
    assert(equal(t.vataAut_,t.orig_));
    return t;
}

VATAAdapter* VATAAdapter::allocateTAWithSameFinalStates(
    const VATAAdapter&         ta,
    bool                       copyFinalStates)
{
    FA_DEBUG_AT(1,"Allocate TA with same final states\n");
    VATAAdapter* res = new VATAAdapter(TreeAut(ta.vataAut_, true, copyFinalStates));
    res->orig_ = TA::createTAWithSameFinalStates(ta.orig_, copyFinalStates);
    assert(equal(res->vataAut_, res->orig_));
    return res;
}

VATAAdapter::iterator VATAAdapter::begin() const
{
    //FA_DEBUG_AT(1,"TA begin\n");
    assert(equal(this->vataAut_, this->orig_));
    return vataAut_.begin();
}

VATAAdapter::iterator VATAAdapter::end() const
{
    //FA_DEBUG_AT(1,"TA end\n");
    assert(equal(this->vataAut_, this->orig_));
	return vataAut_.end();
}

// TODO CHECK semantic against original implementation
typename VATAAdapter::DownAccessor::Iterator VATAAdapter::begin(
        size_t parent) const
{
    FA_DEBUG_AT(1,"TA Down begin\n");
    assert(equal(this->vataAut_, this->orig_));
    return vataAut_[parent].begin();
}

// TODO CHECK semantic against original implementation
typename VATAAdapter::DownAccessor::Iterator VATAAdapter::end(
        size_t parent) const
{
    FA_DEBUG_AT(1,"TA Down end\n");
    assert(equal(this->vataAut_, this->orig_));
    return vataAut_[parent].end();
}

// TODO CHECK semantic against original implementation
typename VATAAdapter::DownAccessor::Iterator VATAAdapter::end(
        size_t parent,
        DownAccessor::Iterator i) const
{
    FA_DEBUG_AT(1,"TA Down end 1\n");
    assert(equal(this->vataAut_, this->orig_));
    return vataAut_[parent].end();
}

typename VATAAdapter::AcceptTrans::Iterator VATAAdapter::accBegin() const
{
    FA_DEBUG_AT(1,"TA Acc begin\n");
    assert(equal(this->vataAut_, this->orig_));
   return vataAut_.GetAcceptTrans().begin();
}

typename VATAAdapter::AcceptTrans::Iterator VATAAdapter::accEnd() const
{
    FA_DEBUG_AT(1,"TA Acc end\n");
    assert(equal(this->vataAut_, this->orig_));
   return vataAut_.GetAcceptTrans().end();
}

// TODO CHECK semantic against original implementation
typename VATAAdapter::AcceptTrans::Iterator VATAAdapter::accEnd(
        VATAAdapter::AcceptTrans::Iterator i) const
{
    FA_DEBUG_AT(1,"TA Acc end 1\n");
    assert(equal(this->vataAut_, this->orig_));
    return vataAut_.GetAcceptTrans().end(); 
}

VATAAdapter& VATAAdapter::operator=(const VATAAdapter& rhs)
{
    FA_DEBUG_AT(1,"TA =\n");

	if (this != &rhs)
	{
		this->vataAut_ = rhs.vataAut_;
	}
    this->orig_ = rhs.orig_;

    assert(equal(this->vataAut_, this->orig_));
    return *this;
}

void VATAAdapter::addTransition(
		const std::vector<size_t>&          children,
		const SymbolType&                   symbol,
		size_t                              parent)
{
    //FA_DEBUG_AT(1,"TA add transition\n");

    this->vataAut_.AddTransition(children, symbol, parent);
    this->orig_.addTransition(children, symbol, parent);
    //assert(equal(this->vataAut_, this->orig_));
}

void VATAAdapter::addTransition(const Transition& transition)
{
    //FA_DEBUG_AT(1,"TA add transition 1\n");
    this->vataAut_.AddTransition(transition);
    this->orig_.addTransition(transition.GetChildren(),
            transition.GetSymbol(), transition.GetParent());
    //assert(equal(this->vataAut_, this->orig_));
}

const VATAAdapter::Transition VATAAdapter::getTransition(
		const std::vector<size_t>&          children,
		const SymbolType&                   symbol,
		size_t                              parent)

{
    FA_DEBUG_AT(1,"TA get transition\n");
    if (vataAut_.ContainsTransition(children, symbol, parent))
    {
        assert(equal(this->vataAut_, this->orig_));
        return Transition(parent, symbol, children);
    }

    assert(false);
    return Transition();
}

const label_type VATAAdapter::GetSymbol(const Transition& t)
{
    //FA_DEBUG_AT(1,"TA get symbol\n");
    return label_type(t.GetSymbol());
}

void VATAAdapter::addFinalState(size_t state)
{
    FA_DEBUG_AT(1,"TA add final state\n");
    vataAut_.SetStateFinal(state);
    this->orig_.addFinalState(state);
    assert(equal(this->vataAut_, this->orig_));
}

void VATAAdapter::addFinalStates(const std::set<size_t>& states)
{
    FA_DEBUG_AT(1,"TA add final states\n");
    vataAut_.SetStatesFinal(states);
    assert(equal(this->vataAut_, this->orig_));
}

void VATAAdapter::addFinalStates(const std::unordered_set<size_t>& states)
{
    FA_DEBUG_AT(1,"TA add final states 1\n");
    for (size_t state : states)
    {
        vataAut_.SetStateFinal(state);
    }
    this->orig_.addFinalStates(states);
    assert(equal(this->vataAut_, this->orig_));
}

bool VATAAdapter::isFinalState(size_t state) const
{
    FA_DEBUG_AT(1,"TA is final state\n");
    assert(equal(this->vataAut_, this->orig_));
    return vataAut_.IsStateFinal(state);
}

const std::unordered_set<size_t>& VATAAdapter::getFinalStates() const
{
    FA_DEBUG_AT(1,"TA get final states\n");
    assert(equal(this->vataAut_, this->orig_));
    return vataAut_.GetFinalStates();
}

size_t VATAAdapter::getFinalState() const
{
	FA_DEBUG_AT(1,"TA get final state\n");

	const std::unordered_set<size_t>& finalStates = this->getFinalStates();
	assert(1 == finalStates.size());
    assert(equal(this->vataAut_, this->orig_));
    return *finalStates.begin();
}

const VATAAdapter::Transition VATAAdapter::getAcceptingTransition() const
{
    assert(++(vataAut_.GetAcceptTrans().begin()) == vataAut_.GetAcceptTrans().end());
    //FA_DEBUG_AT(1,"TA get accepting transitions\n");
    assert(equal(this->vataAut_, this->orig_));
    return *(vataAut_.GetAcceptTrans().begin());
}

VATAAdapter& VATAAdapter::unreachableFree(VATAAdapter& dst) const
{
    FA_DEBUG_AT(1,"TA unreachable\n");
    dst.vataAut_ = std::move(vataAut_.RemoveUnreachableStates());
    this->orig_.unreachableFree(dst.orig_);
    assert(equal(this->vataAut_, this->orig_));
    return dst;
}

VATAAdapter& VATAAdapter::uselessAndUnreachableFree(VATAAdapter& dst) const
{
    FA_DEBUG_AT(1,"TA useless\n");
    dst.vataAut_ = std::move(vataAut_.RemoveUselessStates());
    dst.vataAut_ = std::move(dst.vataAut_.RemoveUnreachableStates());
    this->orig_.uselessAndUnreachableFree(dst.orig_);
    assert(equal(this->vataAut_, this->orig_));
    return dst;
}

VATAAdapter& VATAAdapter::disjointUnion(
		VATAAdapter&                      dst,
		const VATAAdapter&                src,
		bool                              addFinalStates)
{
    FA_DEBUG_AT(1,"TA disjoint\n");
    if (addFinalStates)
    {
        dst.vataAut_ = std::move(TreeAut::UnionDisjointStates(
            src.vataAut_, dst.vataAut_));
    }
    else
    { // if it is not needed copy the final states
      // it is sufficient to copy just the transitions
        for (const Transition& t : src.vataAut_)
        {
            dst.addTransition(t);
        }
    }

    TA::disjointUnion(dst.orig_, src.orig_, addFinalStates);
    assert(equal(src.vataAut_, src.orig_));
    assert(equal(dst.vataAut_, dst.orig_));
    return dst;
}

VATAAdapter& VATAAdapter::minimized(VATAAdapter& dst) const
{
    FA_DEBUG_AT(1,"TA minimized\n");
    dst.vataAut_ = std::move(vataAut_.Reduce());
    this->orig_.minimized(dst.orig_);
    assert(equal(this->vataAut_, this->orig_));
    assert(equal(dst.vataAut_, dst.orig_));
    return dst;
}

bool VATAAdapter::areTransitionsEmpty()
{
    FA_DEBUG_AT(1,"TA are transitions empty\n");
    assert(equal(this->vataAut_, this->orig_));
    return vataAut_.AreTransitionsEmpty();
}

// TODO: Rewrite to std::move
VATAAdapter& VATAAdapter::copyTransitions(VATAAdapter& dst) const
{
    FA_DEBUG_AT(1,"TA copy transitions\n");
    CopyAllFunctor copyAllFunctor;
    dst.vataAut_.CopyTransitionsFrom(vataAut_, copyAllFunctor);
    this->orig_.copyTransitions(dst.orig_);
    assert(equal(this->vataAut_, this->orig_));
    assert(equal(dst.vataAut_, dst.orig_));
	return dst;
}

// TODO: Rewrite to std::move
VATAAdapter& VATAAdapter::copyNotAcceptingTransitions(
        VATAAdapter&                       dst,
        const VATAAdapter&                 ta) const
{
    FA_DEBUG_AT(1,"TA copy not accepting transitions\n");
    CopyNonAcceptingFunctor copyFunctor(ta);
    dst.vataAut_.CopyTransitionsFrom(vataAut_, copyFunctor);
    this->orig_.copyNotAcceptingTransitions(dst.orig_, ta.orig_);
    assert(equal(this->vataAut_, this->orig_));
    assert(equal(dst.vataAut_, dst.orig_));
    assert(equal(ta.vataAut_, ta.orig_));
	return dst;
}

void VATAAdapter::clear()
{
    FA_DEBUG_AT(1,"TA clear\n");
    vataAut_.Clear();
    this->orig_.clear();
    assert(equal(this->vataAut_, this->orig_));
}

bool VATAAdapter::subseteq(const VATAAdapter& a, const VATAAdapter& b)
{
    FA_DEBUG_AT(1,"TA subseteq\n");
    assert(equal(a.vataAut_, a.orig_));
    assert(equal(b.vataAut_, b.orig_));
   return TreeAut::CheckInclusion(a.vataAut_, b.vataAut_);
}

// TODO: Rewrite to std::move
VATAAdapter& VATAAdapter::unfoldAtRoot(
    VATAAdapter&                   dst,
	size_t                         newState,
	bool                           registerFinalState) const
{
    FA_DEBUG_AT(1,"TA unfoldAtRoot\n");
    if (registerFinalState)
    {
        dst.addFinalState(newState);
    }

    for (auto trans : vataAut_)
    {
        dst.addTransition(trans);
        if (isFinalState(trans.GetParent()))
        {
            dst.addTransition(trans.GetChildren(), trans.GetSymbol(), newState);
        }
    }

    this->orig_.unfoldAtRoot(dst.orig_, newState, registerFinalState);
    assert(equal(dst.vataAut_, dst.orig_));
    assert(equal(this->vataAut_, this->orig_));
    return dst;
}


// TODO: Rewrite to std::move
VATAAdapter& VATAAdapter::unfoldAtRoot(
    VATAAdapter&                                  dst,
    const std::unordered_map<size_t, size_t>&     statesTranslator,
    const std::unordered_map<size_t, size_t>&     statesHelp,
    bool                                          registerFinalState) const
{
    FA_DEBUG_AT(1,"TA unfoldAtRoot1\n");
    this->copyTransitions(dst);
    for (auto state : this->getFinalStates())
    {
        std::unordered_map<size_t, size_t>::const_iterator j = statesTranslator.find(state);
        assert(j != statesTranslator.end());

        for (auto trans : vataAut_[state])
        {
            dst.addTransition(trans.GetChildren(), trans.GetSymbol(), j->second);
        }

        if (registerFinalState)
        {
            dst.addFinalState(j->second);
        }
    }

    this->orig_.unfoldAtRoot(dst.orig_, statesHelp, registerFinalState);
    assert(equal(dst.vataAut_, dst.orig_));
    assert(equal(this->vataAut_, this->orig_));
    return dst;
}

void VATAAdapter::buildStateIndex(Index<size_t>& index) const
{
    FA_DEBUG_AT(1,"TA buildStateIndex\n");

    for (auto state : vataAut_.GetUsedStates())
    {
        index.add(state);
    }
    assert(equal(this->vataAut_, this->orig_));
}

// TODO: check this if there will be problems (and they will)
VATAAdapter::TreeAut::AcceptTrans VATAAdapter::getEmptyRootTransitions() const
{
    FA_DEBUG_AT(1,"TA get empty root\n");
    assert(vataAut_.IsStateFinal(cEmptyRootTransIndex)
            && vataAut_.GetFinalStates().size() == 1);
    assert(equal(this->vataAut_, this->orig_));
    return vataAut_.GetAcceptTrans();
}

// OL: this should return ALL transitions reachable from rootState, not only
// where rootState is the parent, but also those under them. So this method
// should do reachability analysis of the graph of the automaton (remembering
// already visited states etc.). See treeaut.hh, around the line 360.
void VATAAdapter::copyReachableTransitionsFromRoot(
    const VATAAdapter&        src,
    const size_t&             rootState)
{
    FA_DEBUG_AT(1,"TA copy reachable transitions from root\n");
    std::vector<Transition> stack;
    std::unordered_set<size_t> visited;

    for (const Transition t : src.vataAut_[rootState])
    {
        stack.push_back(t);
    }

    while(!stack.empty())
    {
        const Transition t = stack.back();
        stack.pop_back();
        this->addTransition(t);
        visited.insert(t.GetParent());
        for (size_t child : t.GetChildren())
        {
            if (visited.count(child) == 0)
            {
                for (const Transition k : src.vataAut_[child])
                {
                    stack.push_back(k);
                }
            }
        }
    }

    this->orig_.copyReachableTransitionsFromRoot(src.orig_, rootState);
    assert(equal(this->vataAut_, this->orig_));
    assert(equal(src.vataAut_, src.orig_));
}

// collapses states according to a given relation
// OL: should finish the function in VATA
VATAAdapter& VATAAdapter::collapsed(
    VATAAdapter&                             dst,
    const std::vector<std::vector<bool>>&    rel,
    const Index<size_t>&                     stateIndex) const
{
    // relation compatible with the one in VATA
    std::unordered_map<size_t, size_t> vataRel(rel.size());
    for (size_t state1 : vataAut_.GetUsedStates())
    {
        size_t i = stateIndex.translate(state1);
        for (size_t state2 : vataAut_.GetUsedStates())
        {
            size_t j = stateIndex.translate(state2);
            if (rel[i][j])
            {
                vataRel.insert(std::make_pair(state2,state1));
            }
        }
    }

    assert(vataRel.size() == rel.size()); // all elements are defined

    FA_DEBUG_AT(1,"TA collapsed\n");
    dst.vataAut_ = vataAut_.CollapseStates(vataRel);

	//============================================================
    dst.orig_.clear();
    for (Transition t : dst.vataAut_)
    {
        dst.orig_.addTransition(t.GetChildren(), t.GetSymbol(), t.GetParent());
    }
    for (const size_t s : dst.vataAut_.GetFinalStates())
    {
        dst.orig_.addFinalState(s);
    }

    assert(equal(this->vataAut_, this->orig_));
    assert(equal(dst.vataAut_, dst.orig_));
    return dst;
}

bool VATAAdapter::equal(const TreeAut& vata, const TA& orig)
{
    TreeAut ta;

    for (TA::Transition t : orig)
    {
        ta.AddTransition(t.GetChildren(), t.GetSymbol(), t.GetParent());
    }
    for (auto state : orig.getFinalStates())
    {
        ta.SetStateFinal(state);
    }

    std::cerr << VATAAdapter(vata);
    std::cerr << VATAAdapter(ta);
    std::cerr << "============\n";
    bool res = TreeAut::CheckInclusion(vata, ta) && TreeAut::CheckInclusion(ta, vata);

    if (!res)
    {
        std::cerr << "VATA " << VATAAdapter(vata);
        std::cerr << "TA " << orig;
        std::cerr << "======================\n";
    }

    return res;
}

std::ostream& operator<<(std::ostream& os, const VATAAdapter& ta)
{
    os << "TREE AUT " << std::endl;
    for (auto t : ta.vataAut_)
    {
        if(ta.vataAut_.IsStateFinal(t.GetParent()))
        {
            os << "[" << t.GetParent() << "] " << VATAAdapter::GetSymbol(t) << " ";
        }
        else
        {
            os << t.GetParent() << " " << VATAAdapter::GetSymbol(t) << " ";
        }
        for (auto s : t.GetChildren()) os << s << " " ;
        os << "\n";
    }

    os.flush();
    return os;
}
