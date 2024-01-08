#ifndef CHIHAYA_UTILS_MISC_HPP
#define CHIHAYA_UTILS_MISC_HPP

#include "H5Cpp.h"
#include "ritsuko/hdf5/hdf5.hpp"
#include "ritsuko/ritsuko.hpp"

#include <string>
#include <stdexcept>

namespace chihaya {

namespace internal_misc {

inline bool is_version_at_or_below(const ritsuko::Version& v, int major, int minor) {
    return v.major < major || (v.major == major && v.minor <= minor);
}

template<class V>
bool are_dimensions_equal(const V& left, const V& right) {
    if (left.size() != right.size()) {
        return false;
    } else {
        for (size_t i = 0; i < left.size(); ++i) {
            if (left[i] != right[i]) {
                return false;
            }
        }
    }
    return true;
}

inline void validate_missing_placeholder(const H5::DataSet& handle, const ritsuko::Version& version) {
    if (version.major == 0) {
        return;
    }

    const char* placeholder = "missing_placeholder";
    if (!handle.attrExists(placeholder)) {
        return;
    }

    auto ahandle = handle.openAttribute(placeholder);
    ritsuko::hdf5::check_missing_placeholder_attribute(handle, ahandle, /* type_class_only = */ (version.major == 1 && version.minor == 0));
}

inline uint64_t load_along(const H5::Group& handle, const ritsuko::Version& version) {
    auto ahandle = ritsuko::hdf5::open_dataset(handle, "along");
    if (!ritsuko::hdf5::is_scalar(ahandle)) {
        throw std::runtime_error("'along' should be a scalar dataset");
    }

    if (is_version_at_or_below(version, 1, 0)) {
        if (ahandle.getTypeClass() != H5T_INTEGER) {
            throw std::runtime_error("'along' should be an integer dataset");
        }
        int along_tmp; 
        ahandle.read(&along_tmp, H5::PredType::NATIVE_INT);
        if (along_tmp < 0) {
            throw std::runtime_error("'along' should be non-negative");
        }
        return along_tmp;

    } else {
        if (ritsuko::hdf5::exceeds_integer_limits(ahandle, 64, false)) {
            throw std::runtime_error("'along' should have a datatype that fits in a 64-bit unsigned integer");
        }
        return ritsuko::hdf5::load_scalar_numeric_dataset<uint64_t>(ahandle);
    }
}

inline ArrayDetails load_seed_details(const H5::Group& handle, const std::string& name, const ritsuko::Version& version) {
    ArrayDetails output;
    auto shandle = ritsuko::hdf5::open_group(handle, name.c_str());
    try {
        seed_details = ::chihaya::validate(shandle, version);
    } catch (std::exception& e) {
        throw std::runtime_error("failed to validate '" + name + "'; " + std::string(e.what()));
    }
    return output;
}

}

}

#endif
