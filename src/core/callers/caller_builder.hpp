// Copyright (c) 2015-2019 Daniel Cooke
// Use of this source code is governed by the MIT license that can be found in the LICENSE file.

#ifndef caller_builder_hpp
#define caller_builder_hpp

#include <unordered_map>
#include <string>
#include <memory>
#include <functional>

#include <boost/optional.hpp>

#include "config/common.hpp"
#include "basics/ploidy_map.hpp"
#include "basics/trio.hpp"
#include "basics/pedigree.hpp"
#include "readpipe/read_pipe.hpp"
#include "core/tools/coretools.hpp"
#include "core/models/haplotype_likelihood_model.hpp"
#include "utils/memory_footprint.hpp"
#include "caller.hpp"
#include "cancer_caller.hpp"

namespace octopus {

class CallerBuilder
{
public:
    using RefCallType = Caller::RefCallType;
    using NormalContaminationRisk = CancerCaller::Parameters::NormalContaminationRisk;
    
    CallerBuilder() = delete;
    
    CallerBuilder(const ReferenceGenome& reference, const ReadPipe& read_pipe,
                  VariantGeneratorBuilder vgb, HaplotypeGenerator::Builder hgb);
    
    CallerBuilder(const CallerBuilder&);
    CallerBuilder& operator=(const CallerBuilder&);
    CallerBuilder(CallerBuilder&&);
    CallerBuilder& operator=(CallerBuilder&&);
    
    ~CallerBuilder() = default;
    
    // common
    CallerBuilder& set_reference(const ReferenceGenome& reference) noexcept;
    CallerBuilder& set_read_pipe(const ReadPipe& read_pipe) noexcept;
    CallerBuilder& set_variant_generator(const VariantGeneratorBuilder& vb) noexcept;
    CallerBuilder& set_ploidies(PloidyMap ploidies) noexcept;
    CallerBuilder& set_caller(std::string caller);
    CallerBuilder& set_refcall_type(Caller::RefCallType type) noexcept;
    CallerBuilder& set_refcall_merge_block_threshold(Phred<double> threshold) noexcept;
    CallerBuilder& set_sites_only() noexcept;
    CallerBuilder& set_reference_haplotype_protection(bool b) noexcept;
    CallerBuilder& set_target_memory_footprint(MemoryFootprint memory) noexcept;
    CallerBuilder& set_execution_policy(ExecutionPolicy policy) noexcept;
    CallerBuilder& set_use_paired_reads(bool use) noexcept;
    CallerBuilder& set_use_linked_reads(bool use) noexcept;
    CallerBuilder& set_bad_region_detector(BadRegionDetector detector) noexcept;
    
    CallerBuilder& set_min_variant_posterior(Phred<double> posterior) noexcept;
    CallerBuilder& set_min_refcall_posterior(Phred<double> posterior) noexcept;
    CallerBuilder& set_max_haplotypes(unsigned n) noexcept;
    CallerBuilder& set_haplotype_extension_threshold(Phred<double> p) noexcept;
    CallerBuilder& set_model_filtering(bool b) noexcept;
    CallerBuilder& set_min_phase_score(Phred<double> score) noexcept;
    CallerBuilder& set_snp_heterozygosity(double heterozygosity) noexcept;
    CallerBuilder& set_indel_heterozygosity(double heterozygosity) noexcept;
    CallerBuilder& set_max_genotypes(unsigned max) noexcept;
    CallerBuilder& set_max_joint_genotypes(unsigned max) noexcept;
    CallerBuilder& set_likelihood_model(HaplotypeLikelihoodModel model) noexcept;
    CallerBuilder& set_model_based_haplotype_dedup(bool use) noexcept;
    CallerBuilder& set_independent_genotype_prior_flag(bool use_independent) noexcept;
    CallerBuilder& set_max_vb_seeds(unsigned n) noexcept;
    
    // cancer
    CallerBuilder& set_normal_sample(SampleName normal_sample);
    CallerBuilder& set_max_somatic_haplotypes(unsigned n) noexcept;
    CallerBuilder& set_somatic_snv_mutation_rate(double rate) noexcept;
    CallerBuilder& set_somatic_indel_mutation_rate(double rate) noexcept;
    CallerBuilder& set_min_expected_somatic_frequency(double frequency) noexcept;
    CallerBuilder& set_credible_mass(double mass) noexcept;
    CallerBuilder& set_min_credible_somatic_frequency(double frequency) noexcept;
    CallerBuilder& set_tumour_germline_concentration(double concentration) noexcept;
    CallerBuilder& set_min_somatic_posterior(Phred<double> posterior) noexcept;
    CallerBuilder& set_normal_contamination_risk(NormalContaminationRisk risk) noexcept;
    
    // trio
    CallerBuilder& set_trio(Trio trio);
    CallerBuilder& set_min_denovo_posterior(Phred<double> posterior) noexcept;
    CallerBuilder& set_snv_denovo_mutation_rate(double rate) noexcept;
    CallerBuilder& set_indel_denovo_mutation_rate(double rate) noexcept;
    
    // polyclone
    CallerBuilder& set_max_clones(unsigned n) noexcept;
    
    // cell
    CallerBuilder& set_dropout_concentration(double concentration) noexcept;
    
    // pedigree
    CallerBuilder& set_pedigree(Pedigree pedigree);
    
    std::unique_ptr<Caller> build(const ContigName& contig) const;
    
private:
    struct Components
    {
        std::reference_wrapper<const ReferenceGenome> reference;
        std::reference_wrapper<const ReadPipe> read_pipe;
        VariantGeneratorBuilder variant_generator_builder;
        HaplotypeGenerator::Builder haplotype_generator_builder;
        HaplotypeLikelihoodModel likelihood_model;
        Phaser phaser;
        boost::optional<BadRegionDetector> bad_region_detector = boost::none;
    };
    
    struct Parameters
    {
        // common
        Caller::Parameters general;
        PloidyMap ploidies;
        Phred<double> min_variant_posterior, min_refcall_posterior;
        boost::optional<double> snp_heterozygosity, indel_heterozygosity;
        Phred<double> min_phase_score;
        unsigned max_genotypes, max_joint_genotypes;
        bool deduplicate_haplotypes_with_caller_model;
        bool use_independent_genotype_priors;
        boost::optional<unsigned> max_vb_seeds;
        
        // cancer
        boost::optional<SampleName> normal_sample;
        unsigned max_somatic_haplotypes;
        double somatic_snv_mutation_rate, somatic_indel_mutation_rate;
        double min_expected_somatic_frequency;
        double credible_mass;
        double min_credible_somatic_frequency;
        double tumour_germline_concentration;
        Phred<double> min_somatic_posterior;
        NormalContaminationRisk normal_contamination_risk;
        bool call_somatics_only;
        
        // trio
        boost::optional<Trio> trio;
        Phred<double> min_denovo_posterior;
        boost::optional<double> snv_denovo_mutation_rate, indel_denovo_mutation_rate;
        
        // polyclone
        unsigned max_clones;
        
        // cell
        double dropout_concentration;
        
        // pedigree
        boost::optional<Pedigree> pedigree;
    };
    
    using CallerFactoryMap = std::unordered_map<std::string, std::function<std::unique_ptr<Caller>()>>;
    
    std::string caller_;
    Components components_;
    Parameters params_;
    CallerFactoryMap factory_;
    
    mutable boost::optional<ContigName> requested_contig_;
    
    Caller::Components make_components() const;
    CallerFactoryMap generate_factory() const;
};

} // namespace octopus

#endif
