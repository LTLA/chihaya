#ifndef CHIHAYA_UNARY_COMPARISON_HPP
#define CHIHAYA_UNARY_COMPARISON_HPP

#include "H5Cpp.h"
#include "ritsuko/ritsuko.hpp"
#include "ritsuko/hdf5/hdf5.hpp"

#include <stdexcept>

#include "utils_public.hpp"
#include "utils_comparison.hpp"
#include "utils_unary.hpp"
#include "utils_misc.hpp"
#include "utils_type.hpp"

/**
 * @file unary_comparison.hpp
 * @brief Validation for delayed unary comparisons.
 */

namespace chihaya {

/**
 * @namespace chihaya::unary_comparison
 * @brief Namespace for delayed unary comparisons.
 */
namespace unary_comparison {

/**
 * @param handle An open handle on a HDF5 group representing an unary comparison operation.
 * @param version Version of the **chihaya** specification.
 *
 * @return Details of the object after applying the comparison operation.
 * Otherwise, if the validation failed, an error is raised.
 */
inline ArrayDetails validate(const H5::Group& handle, const ritsuko::Version& version) {
    auto seed_details = internal_misc::load_seed_details(handle, "seed", version);

    auto method = internal_unary::load_method(handle);
    if (!internal_comparison::is_valid_operation(method)) {
        throw std::runtime_error("unrecognized 'method' (" + method + ")");
    }

    auto side = internal_unary::load_side(handle);
    if (side != "left" && side != "right") {
        throw std::runtime_error("unrecognized side '" + side + "'");
    }

    // Checking the value.
    auto vhandle = ritsuko::hdf5::open_dataset(handle, "value");
    try {
        if (internal_misc::is_version_at_or_below(version, 1, 0)) {
            if ((seed_details.type == STRING) != (vhandle.getTypeClass() == H5T_STRING)) {
                throw std::runtime_error("both or neither of 'seed' and 'value' should contain strings");
            }
        } else {
            auto type = internal_type::fetch_data_type(vhandle);
            auto tt = internal_type::translate_type_1_1(type);
            if ((tt == STRING) != (seed_details.type == STRING)) {
                throw std::runtime_error("both or neither of 'seed' and 'value' should contain strings");
            }
            internal_type::check_type_1_1(vhandle, tt);
        }

        internal_misc::validate_missing_placeholder(vhandle, version);
    } catch (std::exception& e) {
        throw std::runtime_error("failed to validate 'value'; " + std::string(e.what()));
    }

    size_t ndims = vhandle.getSpace().getSimpleExtentNdims();
    if (ndims == 0) {
        // scalar operation.
    } else if (ndims == 1) {
        hsize_t extent;
        vhandle.getSpace().getSimpleExtentDims(&extent);
        internal_unary::check_along(handle, version, seed_details.dimensions, extent);
    } else { 
        throw std::runtime_error("'value' dataset should be scalar or 1-dimensional for an unary comparison operation");
    }

    seed_details.type = BOOLEAN;
    return seed_details;
}

}

}

#endif
