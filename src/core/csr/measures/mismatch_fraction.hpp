// Copyright (c) 2015-2019 Daniel Cooke
// Use of this source code is governed by the MIT license that can be found in the LICENSE file.

#ifndef mismatch_fraction_hpp
#define mismatch_fraction_hpp

#include <string>
#include <vector>

#include "measure.hpp"
#include "mismatch_count.hpp"
#include "depth.hpp"

namespace octopus {

class VcfRecord;

namespace csr {

class MismatchFraction : public Measure
{
    MismatchCount mismatch_count_;
    Depth depth_;
    const static std::string name_;
    std::unique_ptr<Measure> do_clone() const override;
    ResultType do_evaluate(const VcfRecord& call, const FacetMap& facets) const override;
    ResultCardinality do_cardinality() const noexcept override;
    const std::string& do_name() const override;
    std::string do_describe() const override;
    std::vector<std::string> do_requirements() const override;
public:
    MismatchFraction();
};

} // namespace csr
} // namespace octopus

#endif
