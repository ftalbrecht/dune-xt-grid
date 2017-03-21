// This file is part of the dune-xt-grid project:
//   https://github.com/dune-community/dune-xt-grid
// Copyright 2009-2017 dune-xt-grid developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2016 - 2017)

#ifndef DUNE_XT_GRID_BOUNDARYINFO_BINDINGS_HH
#define DUNE_XT_GRID_BOUNDARYINFO_BINDINGS_HH
#if HAVE_DUNE_PYBINDXI

#include <sstream>
#include <type_traits>

#include <dune/pybindxi/pybind11.h>
#include <dune/pybindxi/stl.h>

#include <dune/xt/common/configuration.pbh>
#include <dune/xt/grid/gridprovider/provider.hh>
#include <dune/xt/grid/type_traits.hh>
#include <dune/xt/grid/layers.hh>
#include <dune/xt/grid/dd/subdomains/grid.hh>
#include <dune/xt/grid/grids.bindings.hh>

#include "boundaryinfo.hh"

namespace Dune {
namespace XT {
namespace Grid {
namespace bindings {
namespace internal {


template <class I>
class BoundaryInfoFactory
{
public:
  template <class GP>
  static void bind(pybind11::module& m)
  {
    using namespace pybind11::literals;

    try { // guard since we might not be the first to do so for this grid/intersection
      m.def("available_boundary_infos",
            [](const GP& /*grid_provider*/) { return XT::Grid::BoundaryInfoFactory<I>::available(); },
            "grid_provider"_a);
      m.def("default_boundary_info_config",
            [](const GP& /*grid_provider*/, const std::string& type) {
              return XT::Grid::BoundaryInfoFactory<I>::default_config(type);
            },
            "grid_provider"_a,
            "type"_a);
      m.def("make_boundary_info",
            [](const GP& /*grid_provider*/, const std::string& type, const Common::Configuration& cfg) {
              return XT::Grid::BoundaryInfoFactory<I>::create(type, cfg).release();
            },
            "grid_provider"_a,
            "type"_a,
            "cfg"_a = Common::Configuration());
      m.def("make_boundary_info",
            [](const GP& /*grid_provider*/, const Common::Configuration& cfg) {
              return XT::Grid::BoundaryInfoFactory<I>::create(cfg).release();
            },
            "grid_provider"_a,
            "cfg"_a);
    } catch (std::runtime_error&) {
    }
  } // ... bind(...)
}; // class BoundaryInfoFactory


} // namespace internal


template <class Imp, class G>
class BoundaryInfo
{
  typedef typename Imp::IntersectionType I;
  typedef XT::Grid::BoundaryInfo<I> InterfaceType;

public:
  typedef Imp type;
  typedef pybind11::class_<type, InterfaceType> bound_type;


  static bound_type bind(pybind11::module& m, const std::string& class_name, const std::string& layer_name)
  {
    namespace py = pybind11;

    const auto grid_name = bindings::grid_name<G>::value();
    const auto InterfaceName = Common::to_camel_case("BoundaryInfo_" + layer_name + "_" + grid_name);

    // bind interface, guard since we might not be the first to do so for this intersection
    try {
      py::class_<InterfaceType>(m, InterfaceName.c_str(), InterfaceName.c_str(), py::metaclass());
    } catch (std::runtime_error&) {
    }

    // bind factory
    internal::BoundaryInfoFactory<I>::template bind<GridProvider<G>>(m);
    internal::BoundaryInfoFactory<I>::template bind<GridProvider<G, DD::SubdomainGrid<G>>>(m);

    // bind class
    const auto ClassName = Common::to_camel_case(class_name + "_" + layer_name + "_" + grid_name);
    bound_type c(m, ClassName.c_str(), ClassName.c_str(), py::metaclass());
    c.def_property_readonly_static("static_id", [](const type& /*self*/) { return type::static_id(); });
    c.def("__repr__", [ClassName](const type& /*self*/) { return ClassName; });

    return c;
  } // ... bind(...)
}; // class BoundaryInfo


} // namespace bindings
} // namespace Grid
} // namespace XT
} // namespace Dune


// begin: this is what we need for the lib

#define _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB(prefix, _B, _G, _layer, _backend)                                          \
  prefix class Dune::XT::Grid::bindings::                                                                              \
      BoundaryInfo<_B<Dune::XT::Grid::extract_intersection_t<                                                          \
                       typename Dune::XT::Grid::Layer<_G,                                                              \
                                                      Dune::XT::Grid::Layers::_layer,                                  \
                                                      Dune::XT::Grid::Backends::_backend,                              \
                                                      Dune::XT::Grid::DD::SubdomainGrid<_G>>::type>>,                  \
                   _G>

#define _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB_YASP(prefix, _B)                                                           \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB(prefix, _B, YASP_2D_EQUIDISTANT_OFFSET, leaf, view);                             \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB(prefix, _B, YASP_2D_EQUIDISTANT_OFFSET, dd_subdomain, part)

#if HAVE_DUNE_ALUGRID
#define _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB_ALU(prefix, _B)                                                            \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB(prefix, _B, ALU_2D_SIMPLEX_CONFORMING, leaf, view);                              \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB(prefix, _B, ALU_2D_SIMPLEX_CONFORMING, level, view);                             \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB(prefix, _B, ALU_2D_SIMPLEX_CONFORMING, dd_subdomain, part)
#else
#define _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB_ALU(prefix, _B)
#endif

#define _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB_ALL(prefix, _B)                                                            \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB_YASP(prefix, _B);                                                                \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB_ALU(prefix, _B)

#define DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB(prefix)                                                                     \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB_ALL(prefix, Dune::XT::Grid::AllDirichletBoundaryInfo);                           \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB_ALL(prefix, Dune::XT::Grid::AllNeumannBoundaryInfo);                             \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB_ALL(prefix, Dune::XT::Grid::BoundarySegmentIndexBasedBoundaryInfo);              \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB_ALL(prefix, Dune::XT::Grid::NormalBasedBoundaryInfo)

DUNE_XT_GRID_BOUNDARYINFO_BIND_LIB(extern template);

// end: this is what we need for the lib


// begin: this is what we need for the .so

#define _DUNE_XT_GRID_BOUNDARYINFO_BIND(_m, _B, _G, _layer, _backend, _class_name, _layer_name)                        \
  Dune::XT::Grid::bindings::                                                                                           \
      BoundaryInfo<_B<Dune::XT::Grid::extract_intersection_t<                                                          \
                       typename Dune::XT::Grid::Layer<_G,                                                              \
                                                      Dune::XT::Grid::Layers::_layer,                                  \
                                                      Dune::XT::Grid::Backends::_backend,                              \
                                                      Dune::XT::Grid::DD::SubdomainGrid<_G>>::type>>,                  \
                   _G>::bind(_m, _class_name, _layer_name)

#define _DUNE_XT_GRID_BOUNDARYINFO_BIND_YASP(_m, _B, _class_name)                                                      \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND(_m, _B, YASP_2D_EQUIDISTANT_OFFSET, leaf, view, _class_name, "");                    \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND(_m, _B, YASP_2D_EQUIDISTANT_OFFSET, dd_subdomain, part, _class_name, "dd_subdomain")

#if HAVE_DUNE_ALUGRID
#define _DUNE_XT_GRID_BOUNDARYINFO_BIND_ALU(_m, _B, _class_name)                                                       \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND(_m, _B, ALU_2D_SIMPLEX_CONFORMING, leaf, view, _class_name, "leaf");                 \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND(_m, _B, ALU_2D_SIMPLEX_CONFORMING, level, view, _class_name, "level");               \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND(_m, _B, ALU_2D_SIMPLEX_CONFORMING, dd_subdomain, part, _class_name, "dd_subdomain")
#else
#define _DUNE_XT_GRID_BOUNDARYINFO_BIND_ALU(_m, _B, _class_name)
#endif

#define _DUNE_XT_GRID_BOUNDARYINFO_BIND_ALL(_m, _B, _class_name)                                                       \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_YASP(_m, Dune::XT::Grid::_B, _class_name);                                           \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_ALU(_m, Dune::XT::Grid::_B, _class_name)

#define DUNE_XT_GRID_BOUNDARYINFO_BIND(_m)                                                                             \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_ALL(_m, AllDirichletBoundaryInfo, "all_dirichlet_boundary_info");                    \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_ALL(_m, AllNeumannBoundaryInfo, "all_neumann_boundary_info");                        \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_ALL(                                                                                 \
      _m, BoundarySegmentIndexBasedBoundaryInfo, "boundary_segment_index_based_boundary_info");                        \
  _DUNE_XT_GRID_BOUNDARYINFO_BIND_ALL(_m, NormalBasedBoundaryInfo, "normal_based_boundary_info")

// end: this is what we need for the .so


#endif // HAVE_DUNE_PYBINDXI
#endif // DUNE_XT_GRID_BOUNDARYINFO_BINDINGS_HH