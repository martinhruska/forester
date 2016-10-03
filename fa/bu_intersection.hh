#ifndef BU_INTERSECTION_H
#define BU_INTERSECTION_H

#include <vector>
#include <memory>

#include "forestaut.hh"
#include "forestautext.hh"
#include "streams.hh"
#include "vata_adapter.hh"

class BUIntersection
{
private:
    using TreeAut = VATAAdapter;

public:
    using TreeAutVec = std::vector<std::shared_ptr<const TreeAut>>;

    struct BUProductResult
    {
        TreeAutVec tas_;
        VATA::AutBase::ProductTranslMap productMap_;

        BUProductResult(
                TreeAutVec& tav,
                VATA::AutBase::ProductTranslMap productMap) :
            tas_(tav), productMap_(productMap)
        { }

        bool isEmpty()
        {
            for (auto ta : tas_)
            {
                if (ta == nullptr)
                {
                    return true;
                }

				TreeAut tmp(*ta);
                if (tmp.areTransitionsEmpty())
                {
                    return true;
                }
            }

            return false;
        }
    };


    static BUProductResult bottomUpIntersection(
            const FAE&            fwdFAE,
            const FAE&            bwdFAE);

    static bool isResultNonEmpty(
            const BUProductResult& result
    );
};
#endif
