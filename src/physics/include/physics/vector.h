/**
 * @file vector.h
 * Declares 2D Vector class
 */

#ifndef PHYSICS_VECTOR_H
#define PHYSICS_VECTOR_H

#include <iostream>
#include "physics/physics_export.h"

namespace physics {

/**
 * Class for 2D vectors
 */
class PHYSICS_EXPORT Vector {
public:
	Vector();
	Vector(double x, double y);

	/**
	 * Equal to operator for vectors
	 *
	 * @param[in]  rhs   The vector to be compared against
	 *
	 * @return     true if the vectors are equal, else false
	 */
	bool operator==(const Vector& rhs) const;

	/**
	 * Addition operator for vectors
	 *
	 * @param[in]  rhs   The vector to be added with
	 *
	 * @return     Sum of the vectors
	 */
	Vector operator+(const Vector& rhs) const;

	/**
	 * Minus operator for vectors
	 * Subtracts the paramater passed from the vector
	 *
	 * @param[in]  rhs   The vector that subtracts from
	 *
	 * @return     Difference of the vectors
	 */
	Vector operator-(const Vector& rhs) const;

	/**
	 * Scalar Addition operator
	 * vec.x + scalar, vec.y + scalar
	 *
	 * @param[in]  scalar  The scalar to add
	 *
	 * @return     The new vector
	 */
	Vector operator+(const double& scalar) const;

	/**
	 * Scalar Subtraction operator
	 * vec.x - scalar, vec.y - scalar
	 *
	 * @param[in]  scalar  The scalar to subtract
	 *
	 * @return     The new vector
	 */
	Vector operator-(const double& scalar) const;

	/**
	 * Scalar multiplication operator
	 *
	 * @param[in]  scalar  The factor to multiply by
	 *
	 * @return     The scaled vector
	 */
	Vector operator*(const double& scalar) const;

	/**
	 * Scalar division operator
	 *
	 * @param[in]  scalar  The factor to divide by
	 *
	 * @return     The scaled vector
	 */
	Vector operator/(const double& scalar) const;

	friend std::ostream& operator<<(
		std::ostream& ostream,
		const Vector& vector
	);

	/**
	 * Dot product of vectors
	 *
	 * @param[in]  rhs   The vector with which the operation is performed
	 *
	 * @return     The result of the dot product
	 */
	double dot(const Vector& rhs) const;

	/**
	 * The magnitude of the vector
	 *
	 * @return     The magnitude of the vector
	 */

	double magnitude() const;
	/**
	 * Distance between this and another vector
	 *
	 * @param[in]  other  The other vector
	 *
	 * @return     The distance between the two vectors
	 */
	double distance(const Vector& other) const;

	double x;
	double y;
};

}

#endif
