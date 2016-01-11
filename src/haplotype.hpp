//
//  haplotype.hpp
//  Octopus
//
//  Created by Daniel Cooke on 22/03/2015.
//  Copyright (c) 2015 Oxford University. All rights reserved.
//

#ifndef __Octopus__haplotype__
#define __Octopus__haplotype__

#include <queue>
#include <cstddef>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include <iostream>

#include "contig_region.hpp"
#include "mappable.hpp"
#include "contig_allele.hpp"
#include "allele.hpp"
#include "comparable.hpp"
#include "mappable.hpp"

class ReferenceGenome;
class GenomicRegion;
class Variant;

class Haplotype;

namespace detail {
    Haplotype do_splice(const Haplotype& haplotype, const GenomicRegion& region, std::true_type);
    Allele do_splice(const Haplotype& haplotype, const GenomicRegion& region, std::false_type);
} // namespace detail

class Haplotype : public Comparable<Haplotype>, public Mappable<Haplotype>
{
public:
    using SequenceType = Allele::SequenceType;
    using SizeType     = Allele::SizeType;
    
    Haplotype() = default;
    explicit Haplotype(ReferenceGenome& reference, const GenomicRegion& region);
    ~Haplotype() = default;
    
    Haplotype(const Haplotype&)            = default;
    Haplotype& operator=(const Haplotype&) = default;
    Haplotype(Haplotype&&)                 = default;
    Haplotype& operator=(Haplotype&&)      = default;
    
    template <typename T> void push_back(T&& allele);
    template <typename T> void push_front(T&& allele);
    
    const GenomicRegion& get_region() const;
    
    bool contains(const Allele& allele) const;
    bool contains_exact(const Allele& allele) const;
    
    SequenceType get_sequence() const;
    SequenceType get_sequence(const GenomicRegion& region) const;
    
    std::vector<Variant> difference(const Haplotype& other) const;
    
    size_t get_hash() const;
    
    friend struct IsLessComplex;
    friend struct HaveSameAlleles;
    
    friend bool contains(const Haplotype& lhs, const Haplotype& rhs);
    friend Haplotype detail::do_splice(const Haplotype& haplotype, const GenomicRegion& region, std::true_type);
    
    friend void print_alleles(const Haplotype& haplotype);
    friend void print_variant_alleles(const Haplotype& haplotype);
    
private:
    GenomicRegion region_;
    
    std::deque<Allele> explicit_alleles_;
    
    mutable SequenceType cached_sequence_;
    mutable size_t cached_hash_ = 0;
    
    std::reference_wrapper<ReferenceGenome> reference_;
    
    using AlleleIterator = decltype(explicit_alleles_)::const_iterator;
    
    SequenceType get_reference_sequence(const GenomicRegion& region) const;
    Allele get_intervening_reference_allele(const Allele& lhs, const Allele& rhs) const;
    GenomicRegion get_region_bounded_by_explicit_alleles() const;
    SequenceType get_sequence_bounded_by_explicit_alleles(AlleleIterator first, AlleleIterator last) const;
    SequenceType get_sequence_bounded_by_explicit_alleles() const;
    
    bool is_cached_sequence_good() const noexcept;
    void reset_cached_sequence();
};

template <typename T>
void Haplotype::push_back(T&& allele)
{
    if (!explicit_alleles_.empty()) {
        if (!is_after(allele, explicit_alleles_.back())) {
            throw std::runtime_error {"Haplotype::push_back called with out-of-order Allele"};
        }
        
        if (!are_adjacent(explicit_alleles_.back(), allele)) {
            explicit_alleles_.emplace_back(get_intervening_reference_allele(explicit_alleles_.back(), allele));
        }
    }
    
    explicit_alleles_.emplace_back(std::forward<T>(allele));
    
    region_ = get_encompassing(region_, allele);
    
    reset_cached_sequence();
}

template <typename T>
void Haplotype::push_front(T&& allele)
{
    if (!explicit_alleles_.empty()) {
        if (!is_after(explicit_alleles_.front(), allele)) {
            throw std::runtime_error {"Haplotype::push_front called with out-of-order Allele"};
        }
        
        if (!are_adjacent(allele, explicit_alleles_.front())) {
            explicit_alleles_.emplace_front(get_intervening_reference_allele(allele, explicit_alleles_.front()));
        }
    }
    
    explicit_alleles_.emplace_front(std::forward<T>(allele));
    
    region_ = get_encompassing(region_, allele);
    
    reset_cached_sequence();
}

// non-members

bool contains(const Haplotype& lhs, const Allele& rhs);
bool contains(const Haplotype& lhs, const Haplotype& rhs);

template <typename MappableType>
MappableType splice(const Haplotype& haplotype, const GenomicRegion& region)
{
    return detail::do_splice(haplotype, region, std::is_same<Haplotype, std::decay_t<MappableType>> {});
}

bool is_reference(const Haplotype& haplotype, ReferenceGenome& reference);

struct IsLessComplex
{
    bool operator()(const Haplotype& lhs, const Haplotype& rhs) noexcept;
};

void unique_least_complex(std::vector<Haplotype>& haplotypes);

bool operator==(const Haplotype& lhs, const Haplotype& rhs);
bool operator<(const Haplotype& lhs, const Haplotype& rhs);

struct HaveSameAlleles
{
    bool operator()(const Haplotype& lhs, const Haplotype& rhs) const;
};

bool have_same_alleles(const Haplotype& lhs, const Haplotype& rhs);

bool are_equal_in_region(const Haplotype& lhs, const Haplotype& rhs, const GenomicRegion& region);

namespace std
{
    template <> struct hash<Haplotype>
    {
        size_t operator()(const Haplotype& haplotype) const
        {
            return haplotype.get_hash();
        }
    };
    
    template <> struct hash<reference_wrapper<const Haplotype>>
    {
        size_t operator()(const reference_wrapper<const Haplotype> haplotype) const
        {
            return hash<Haplotype>()(haplotype);
        }
    };
} // namespace std

namespace boost
{
    template <> struct hash<Haplotype> : std::unary_function<Haplotype, size_t>
    {
        size_t operator()(const Haplotype& h) const
        {
            return std::hash<Haplotype>()(h);
        }
    };
} // namespace boost

std::ostream& operator<<(std::ostream& os, const Haplotype& haplotype);

void print_alleles(const Haplotype& haplotype);
void print_variant_alleles(const Haplotype& haplotype);

#endif /* defined(__Octopus__haplotype__) */