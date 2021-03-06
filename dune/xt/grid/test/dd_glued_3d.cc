// This file is part of the dune-xt-grid project:
//   https://github.com/dune-community/dune-xt-grid
// Copyright 2009-2018 dune-xt-grid developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2017)
//   Rene Milk       (2018)
//   Tobias Leibner  (2017)

#include <dune/xt/common/test/main.hxx> // <- this one has to come first (includes the config.h)!

#include "dd_glued.hh"

#if HAVE_DUNE_GRID_GLUE

namespace Dune {
namespace XT {
namespace Grid {


template <bool anything>
struct ExpectedResults<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>,
                       YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>,
                       anything>
{
  static int num_coarse_refinements()
  {
    return 0;
  }

  static int num_local_refinements()
  {
    return 2;
  }

  static std::string id()
  {
    return "3d_yaspgrid_yaspgrid";
  }

  static std::set<size_t> num_local_couplings_intersections()
  {
    // we expect 16 rectangles, each containing two triangles
    return {32};
  }

  static bool failure_for_lower_or_equal()
  {
    return true;
  }

  static bool failure_for_equal()
  {
    return true;
  }

  static bool failure_for_higher()
  {
    return true;
  }

  static std::map<std::pair<ssize_t, ssize_t>, size_t> results()
  {
    return {{{0, 0}, 216},
            {{0, 1}, 864},
            {{0, 2}, 3456},
            {{1, 0}, 108},
            {{1, 1}, 864},
            {{1, 2}, 3456},
            {{2, 0}, 108},
            {{2, 1}, 108},
            {{2, 2}, 3456}};
  }
}; // struct ExpectedResults<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>, YaspGrid<3,
// EquidistantOffsetCoordinates<double, 3>>, anything>

#if HAVE_DUNE_ALUGRID

template <class Comm, bool anything>
struct ExpectedResults<ALUGrid<3, 3, cube, nonconforming, Comm>,
                       YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>,
                       anything>
{
  static int num_coarse_refinements()
  {
    return 0;
  }

  static int num_local_refinements()
  {
    return 2;
  }

  static std::string id()
  {
    return "3d_alucubenonconforminggrid_yaspgrid";
  }

  static std::set<size_t> num_local_couplings_intersections()
  {
    return {32};
  }

  static bool failure_for_lower_or_equal()
  {
    return true;
  }

  static bool failure_for_equal()
  {
    return true;
  }

  static bool failure_for_higher()
  {
    return true;
  }

  static std::map<std::pair<ssize_t, ssize_t>, size_t> results()
  {
    return {{{0, 0}, 216},
            {{0, 1}, 864},
            {{0, 2}, 3456},
            {{1, 0}, 108},
            {{1, 1}, 864},
            {{1, 2}, 3456},
            {{2, 0}, 108},
            {{2, 1}, 108},
            {{2, 2}, 3456}};
  }
}; // struct ExpectedResults<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>, ALUGrid<3, 3, simplex, nonconforming,
// Comm>, anything>

template <class Comm, bool anything>
struct ExpectedResults<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>,
                       ALUGrid<3, 3, cube, nonconforming, Comm>,
                       anything>
{
  static int num_coarse_refinements()
  {
    return 0;
  }

  static int num_local_refinements()
  {
    return 2;
  }

  static std::string id()
  {
    return "3d_yaspgrid_alucubenonconforminggrid";
  }

  static std::set<size_t> num_local_couplings_intersections()
  {
    return {32};
  }

  static bool failure_for_lower_or_equal()
  {
    return true;
  }

  static bool failure_for_equal()
  {
    return true;
  }

  static bool failure_for_higher()
  {
    return true;
  }

  static std::map<std::pair<ssize_t, ssize_t>, size_t> results()
  {
    return {{{0, 0}, 216},
            {{0, 1}, 864},
            {{0, 2}, 3456},
            {{1, 0}, 108},
            {{1, 1}, 864},
            {{1, 2}, 3456},
            {{2, 0}, 108},
            {{2, 1}, 108},
            {{2, 2}, 3456}};
  }
}; // ExpectedResults<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>, ALUGrid<3, 3, cube, nonconforming, Comm>,
// anything>

#endif // HAVE_DUNE_ALUGRID
#if HAVE_DUNE_UGGRID || HAVE_UG

template <bool anything>
struct ExpectedResults<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>, UGGrid<3>, anything>
{
  static int num_coarse_refinements()
  {
    return 0;
  }

  static int num_local_refinements()
  {
    return 2;
  }

  static std::string id()
  {
    return "3d_yaspgrid_uggrid";
  }

  static std::set<size_t> num_local_couplings_intersections()
  {
    // we expect 16 rectangles, each containing two triangles
    return {32};
  }

  static bool failure_for_lower_or_equal()
  {
    return true;
  }

  static bool failure_for_equal()
  {
    return true;
  }

  static bool failure_for_higher()
  {
    return true;
  }

  static std::map<std::pair<ssize_t, ssize_t>, size_t> results()
  {
    return {{{0, 0}, 216},
            {{0, 1}, 864},
            {{0, 2}, 3456},
            {{1, 0}, 72},
            {{1, 1}, 864},
            {{1, 2}, 3456},
            {{2, 0}, 72},
            {{2, 1}, 36},
            {{2, 2}, 3456}};
  }
}; // struct ExpectedResults<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>, UGGrid<3>, anything>

#endif // HAVE_DUNE_UGGRID || HAVE_UG
#if HAVE_ALBERTA

template <bool anything>
struct ExpectedResults<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>, AlbertaGrid<3, 3>, anything>
{
  static int num_coarse_refinements()
  {
    return 0;
  }

  static int num_local_refinements()
  {
    return 2;
  }

  static std::string id()
  {
    return "3d_yaspgrid_albertagrid";
  }

  static std::set<size_t> num_local_couplings_intersections()
  {
    return {10};
  }

  static bool failure_for_lower_or_equal()
  {
    return true;
  }

  static bool failure_for_equal()
  {
    return true;
  }

  static bool failure_for_higher()
  {
    return true;
  }

  static std::map<std::pair<ssize_t, ssize_t>, size_t> results()
  {
    return {{{-1, -1}, 702}};
  }
}; // struct ExpectedResults<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>, AlbertaGrid<3>, anything>

#endif // HAVE_ALBERTA

} // namespace Grid
} // namespace XT
} // namespace Dune


using namespace Dune;
using namespace Dune::XT::Grid;


// clang-format off
typedef ::testing::Types< std::tuple<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>,
                                     YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>>
#if HAVE_DUNE_ALUGRID
                        , std::tuple<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>,
                                     Dune::ALUGrid<3, 3, cube, nonconforming>>
//                      , std::tuple<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>,
//                                   ALUGrid<3, 3, simplex, conforming>>                  // <- knwon to fail completely
//                      , std::tuple<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>,
//                                   ALUGrid<3, 3, simplex, nonconforming>>               // <- knwon to fail completely
                        , std::tuple<Dune::ALUGrid<3, 3, cube, nonconforming>,
                                     YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>>
//                      , std::tuple<ALUGrid<3, 3, simplex, conforming>,
//                                   ALUGrid<3, 3, simplex, nonconforming>>               // <- knwon to fail completely
//                      , std::tuple<ALUGrid<3, 3, simplex, nonconforming>,
//                                   ALUGrid<3, 3, simplex, nonconforming>>               // <- knwon to fail completely
//                      , std::tuple<ALUGrid<3, 3, cube, nonconforming>,
//                                   ALUGrid<3, 3, simplex, nonconforming>>               // <- knwon to fail completely
#endif // HAVE_DUNE_ALUGRID
#if !ENABLE_MPI && (HAVE_DUNE_UGGRID || HAVE_UG)
                        , std::tuple<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>, UGGrid<3>>
#endif
#if HAVE_ALBERTA
                        , std::tuple<YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>, AlbertaGrid<3, 3>>
#endif
                        > GridTypes; // clang-format on

TYPED_TEST_CASE(GluedDdGridTest, GridTypes);
TYPED_TEST(GluedDdGridTest, setup_works)
{
  this->setup();
}
TYPED_TEST(GluedDdGridTest, visualize_is_callable)
{
  this->visualize_is_callable();
}
TYPED_TEST(GluedDdGridTest, couplings_are_of_correct_size)
{
  this->couplings_are_of_correct_size();
}
TYPED_TEST(GluedDdGridTest, __STILL_BROKEN__intersections_are_correctly_oriented_for_equal_levels)
{
  this->check_intersection_orientation_for_equal_levels();
}
TYPED_TEST(GluedDdGridTest, __STILL_BROKEN__intersections_are_correctly_oriented_for_higher_neighbor_levels)
{
  this->check_intersection_orientation_for_higher_neighbor_levels();
}
TYPED_TEST(GluedDdGridTest, __STILL_BROKEN__intersection_orientation_is_wrong_for_lower_or_equal_neighbor_levels)
{
  this->check_intersection_orientation_for_lower_or_equal_neighbor_levels();
}


#endif // HAVE_DUNE_GRID_GLUE
