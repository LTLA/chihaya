#ifndef CHIHAYA_UNARY_SPECIAL_CHECK_HPP
#define CHIHAYA_UNARY_SPECIAL_CHECK_HPP

#include "H5Cpp.h"
#include <stdexcept>
#include <vector>
#include <algorithm>
#include "utils.hpp"

namespace chihaya {

/**
 * @cond
 */
inline ArrayDetails validate(const H5::Group& handle, const std::string&);
/**
 * @endcond
 */

/**
 * Validate delayed unary checks for special values in a HDF5 file.
 * This is "unary" in the sense that only one delayed object is involved.
 *
 * @param handle An open handle on a HDF5 group representing an unary special check operation.
 * @param name Name of the group inside the file.
 *
 * @return Details of the object after applying the special check.
 * Otherwise, if the validation failed, an error is raised.
 * 
 * A delayed special check is represented as a HDF5 group with the following attributes:
 *
 * - `delayed_type` should be a scalar string `"operation"`.
 * - `delayed_operation` should be a scalar string `"unary special check"`.
 *
 * Inside the group, we expect:
 *
 * - A `seed` group, containing a delayed object on which the special check is to be applied.
 *   The `seed` group handle is passed to `validate()` to check its contents recursively and to retrieve the dimensions.
 *   This is expected to be float but boolean and integers will be implicitly promoted as necessary.
 * - A `method` string scalar dataset, specifying the mathematical operation to perform.
 *   This can be any one of:
 *   - `is_nan`, is each value of the `seed` object `NaN`?
 *   - `is_finite`, is each value of the `seed` object finite?
 *   - `is_infinite`, is each value of the `seed` object infinite?
 *   - `is_missing`, is each value of the `seed` object "missing"?
 *     The exact implementation of "missingness" is implementation-defined but should at least include all `NaN`s.
 *
 * The type of the output object is always boolean.
 */
inline ArrayDetails validate_unary_special_check(const H5::Group& handle, const std::string& name) {
    if (!handle.exists("seed") || handle.childObjType("seed") != H5O_TYPE_GROUP) {
        throw std::runtime_error("expected 'seed' group for an unary special check");
    }

    auto seed_details = validate(handle.openGroup("seed"), name + "/seed");
    if (seed_details.type == STRING) {
        throw std::runtime_error("'seed' should contain numeric or boolean values for an unary special check");
    }

    // Checking the method.
    if (!handle.exists("method") || handle.childObjType("method") != H5O_TYPE_DATASET) {
        throw std::runtime_error("expected 'method' dataset for an unary special check");
    }

    auto mhandle = handle.openDataSet("method");
    if (mhandle.getSpace().getSimpleExtentNdims() != 0 || mhandle.getTypeClass() != H5T_STRING) {
        throw std::runtime_error("'method' should be a scalar string for an unary special check");
    }

    H5::StrType stype(0, H5T_VARIABLE);
    std::string method;
    mhandle.read(method, stype);

    if (method != "is_missing" &&
        method != "is_nan" &&
        method != "is_finite" &&
        method != "is_infinite")
    {
        throw std::runtime_error(std::string("unrecognized 'method' (") + method + ") for an unary special check");
    }

    seed_details.type = BOOLEAN;
    return seed_details;
}

}

#endif
