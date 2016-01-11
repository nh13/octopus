//
//  vcf_writer.hpp
//  Octopus
//
//  Created by Daniel Cooke on 29/07/2015.
//  Copyright (c) 2015 Oxford University. All rights reserved.
//

#ifndef __Octopus__vcf_writer__
#define __Octopus__vcf_writer__

#include <boost/filesystem/path.hpp>

#include "htslib_bcf_facade.hpp"

class VcfHeader;
class VcfRecord;
class GenomicRegion;

namespace fs = boost::filesystem;

class VcfWriter
{
public:
    VcfWriter()  = delete;
    explicit VcfWriter(const fs::path& file_path);
    explicit VcfWriter(const fs::path& file_path, const VcfHeader& header);
    ~VcfWriter() = default;
    
    VcfWriter(const VcfWriter&)            = delete;
    VcfWriter& operator=(const VcfWriter&) = delete;
    VcfWriter(VcfWriter&&)                 = default;
    VcfWriter& operator=(VcfWriter&&)      = default;
    
    const fs::path path() const;
    void write(const VcfHeader& header);
    void write(const VcfRecord& record);
    
private:
    fs::path file_path_;
    bool is_header_written_;
    HtslibBcfFacade writer_;
};

bool operator==(const VcfWriter& lhs, const VcfWriter& rhs);

namespace std {
    template <> struct hash<VcfWriter>
    {
        size_t operator()(const VcfWriter& writer) const
        {
            return hash<string>()(writer.path().string());
        }
    };
} // namespace std

#endif /* defined(__Octopus__vcf_writer__) */