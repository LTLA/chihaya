#ifndef CHIHAYA_DIMNAMES_HPP
#define CHIHAYA_DIMNAMES_HPP

#include "H5Cpp.h"
#include "ritsuko/ritsuko.hpp"
#include "ritsuko/hdf5/hdf5.hpp"
#include <stdexcept>
#include "utils_list.hpp"
#include "utils_public.hpp"

/**
 * @file dimnames.hpp
 * @brief Validation for delayed dimnames assignment.
 */

namespace chihaya {

namespace dimnames {

/**
 * @param handle An open handle on a HDF5 group representing a dimnames assignment operation.
 * @param version Version of the **chihaya** specification.
 * @param callbacks Callbacks, passed to `validate()`.
 *
 * @return Details of the object after assigning dimnames.
 * Otherwise, if the validation failed, an error is raised.
 */
inline ArrayDetails validate(const H5::Group& handle, const ritsuko::Version& version, Callbacks& callbacks) {
    ArrayDetails seed_details = internal_misc::load_seed_details(handle, "seed", version, callbacks);
    if (!handle.exists("dimnames")) {
        throw std::runtime_error("expected a 'dimnames' group");
    }
    internal_dimnames::validate(handle, seed_details.dimensions, version);
    return seed_details;
}

}

}

#endif
