/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011-2014, Willow Garage, Inc.
 *  Copyright (c) 2014-2015, Open Source Robotics Foundation
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Open Source Robotics Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/** @author Jia Pan */

#ifndef HPP_FCL_DISTANCE_H
#define HPP_FCL_DISTANCE_H

#include <hpp/fcl/collision_object.h>
#include <hpp/fcl/collision_data.h>
#include <hpp/fcl/distance_func_matrix.h>
#include <hpp/fcl/timings.h>

namespace hpp {
namespace fcl {

/// @brief Main distance interface: given two collision objects, and the
/// requirements for contacts, including whether return the nearest points, this
/// function performs the distance between them. Return value is the minimum
/// distance generated between the two objects.
HPP_FCL_DLLAPI FCL_REAL distance(const CollisionObject* o1,
                                 const CollisionObject* o2,
                                 const DistanceRequest& request,
                                 DistanceResult& result);

/// @copydoc distance(const CollisionObject*, const CollisionObject*, const
/// DistanceRequest&, DistanceResult&)
HPP_FCL_DLLAPI FCL_REAL distance(const CollisionGeometry* o1,
                                 const Transform3f& tf1,
                                 const CollisionGeometry* o2,
                                 const Transform3f& tf2,
                                 const DistanceRequest& request,
                                 DistanceResult& result);

/// @copydoc distance(const CollisionObject*, const CollisionObject*, const
/// DistanceRequest&, DistanceResult&) \note this function update the initial
/// guess of \c request if requested.
///       See QueryRequest::updateGuess
inline FCL_REAL distance(const CollisionObject* o1, const CollisionObject* o2,
                         DistanceRequest& request, DistanceResult& result) {
  FCL_REAL res = distance(o1, o2, (const DistanceRequest&)request, result);
  request.updateGuess(result);
  return res;
}

/// @copydoc distance(const CollisionObject*, const CollisionObject*, const
/// DistanceRequest&, DistanceResult&) \note this function update the initial
/// guess of \c request if requested.
///       See QueryRequest::updateGuess
inline FCL_REAL distance(const CollisionGeometry* o1, const Transform3f& tf1,
                         const CollisionGeometry* o2, const Transform3f& tf2,
                         DistanceRequest& request, DistanceResult& result) {
  FCL_REAL res =
      distance(o1, tf1, o2, tf2, (const DistanceRequest&)request, result);
  request.updateGuess(result);
  return res;
}

/// This class reduces the cost of identifying the geometry pair.
/// This is mostly useful for repeated shape-shape queries.
///
/// \code
///   ComputeDistance calc_distance (o1, o2);
///   FCL_REAL distance = calc_distance(tf1, tf2, request, result);
/// \endcode
class HPP_FCL_DLLAPI ComputeDistance {
 public:
  ComputeDistance(const CollisionGeometry* o1, const CollisionGeometry* o2);

  FCL_REAL operator()(const Transform3f& tf1, const Transform3f& tf2,
                      const DistanceRequest& request,
                      DistanceResult& result) const;

  inline FCL_REAL operator()(const Transform3f& tf1, const Transform3f& tf2,
                             DistanceRequest& request,
                             DistanceResult& result) const {
    FCL_REAL res = operator()(tf1, tf2, (const DistanceRequest&)request,
                              result);
    request.updateGuess(result);
    return res;
  }

  bool operator==(const ComputeDistance& other) const {
    return o1 == other.o1 && o2 == other.o2 && swap_geoms == other.swap_geoms &&
           solver == other.solver && func == other.func;
  }

  bool operator!=(const ComputeDistance& other) const {
    return !(*this == other);
  }

  virtual ~ComputeDistance(){};

 protected:
  // These pointers are made mutable to let the derived classes to update
  // their values when updating the collision geometry (e.g. creating a new
  // one). This feature should be used carefully to avoid any mis usage (e.g,
  // changing the type of the collision geometry should be avoided).
  mutable const CollisionGeometry* o1;
  mutable const CollisionGeometry* o2;

  GJKSolver solver;

  DistanceFunctionMatrix::DistanceFunc func;
  bool swap_geoms;

  virtual FCL_REAL run(const Transform3f& tf1, const Transform3f& tf2,
                       const DistanceRequest& request,
                       DistanceResult& result) const;

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

}  // namespace fcl
}  // namespace hpp

#endif
