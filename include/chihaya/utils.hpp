#ifndef CHIHAYA_UTILS_HPP
#define CHIHAYA_UTILS_HPP

#include <vector>
#include <string>

#include "H5Cpp.h"

/**
 * @file utils.hpp
 *
 * @brief Various utilities.
 */

namespace chihaya {

/**
 * Type of the array.
 * Operations involving mixed types will result in promotion to the later types,
 * e.g., an `INTEGER` and `FLOAT` addition will result in promotion to `FLOAT`.
 * Note that operations involving the same types are not guaranteed to preserve type,
 * e.g., `INTEGER` division is assumed to produce a `FLOAT`.
 */
enum ArrayType { BOOLEAN, INTEGER, FLOAT, STRING };

/**
 * @brief Details about an array.
 *
 * This contains the type and dimensionality of the array,
 * which pretty much describes all that we need for validation purposes.
 */
struct ArrayDetails {
    /**
     * @cond
     */
    ArrayDetails() {}

    ArrayDetails(ArrayType t, std::vector<size_t> d) : type(t), dimensions(std::move(d)) {}
    /**
     * @endcond
     */

    /**
     * Type of the array.
     */
    ArrayType type;

    /** 
     * Dimensions of the array.
     * Values should be non-negative.
     */
    std::vector<size_t> dimensions;
};

/**
 * @cond
 */
inline std::string load_string_attribute(const H5::Attribute& attr, const std::string field) {
    if (attr.getTypeClass() != H5T_STRING || attr.getSpace().getSimpleExtentNdims() != 0) {
        throw std::runtime_error(std::string("'") + field + "' attribute should be a scalar string");
    }

    H5::StrType stype(0, H5T_VARIABLE);
    std::string output;
    attr.read(stype, output);

    return output;
}
/**
 * @endcond
 */

/**
 * @cond
 */
inline std::string load_string_attribute(const H5::Group& handle, const std::string& field, const std::string& extra) {
    if (!handle.attrExists(field)) {
        throw std::runtime_error(std::string("expected a '") + field + "' attribute" + extra);
    }
    return load_string_attribute(handle.openAttribute(field), field);
}
/**
 * @endcond
 */

}

#endif
