// This file is part of the dune-xt-grid project:
//   https://github.com/dune-community/dune-xt-grid
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Andreas Buhr    (2014)
//   Felix Schindler (2012 - 2016)
//   Kirsten Weber   (2012)
//   Rene Milk       (2012 - 2015)
//   Sven Kaulmann   (2014)
//   Tobias Leibner  (2014, 2016)

#ifndef DUNE_XT_GRID_PROVIDER_INTERFACE_HH
#define DUNE_XT_GRID_PROVIDER_INTERFACE_HH

#include <memory>

#include <dune/common/fvector.hh>

#include <dune/grid/io/file/vtk/vtkwriter.hh>
#include <dune/grid/common/rangegenerators.hh>

#include <dune/xt/common/configuration.hh>
#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/ranges.hh>

#include <dune/xt/grid/boundaryinfo.hh>
#include <dune/xt/grid/layers.hh>

namespace Dune {
namespace XT {
namespace Grid {


template <class GridImp>
class ConstProviderInterface
{
  static_assert(GridImp::dimension > 0, "Dimension should be positive!");

public:
  typedef GridImp GridType;
  static const size_t dimDomain = GridImp::dimension;
  typedef typename GridType::ctype DomainFieldType;
  typedef FieldVector<DomainFieldType, dimDomain> DomainType;
  typedef typename GridType::template Codim<0>::Entity EntityType;

  template <ChooseLayer layer, ChoosePartView type>
  struct Layer
  {
    typedef typename Grid::Layer<GridType, layer, type>::Type Type;
  };

  template <ChoosePartView type>
  struct Level
  {
    typedef typename LevelPartView<GridType, type>::Type Type;
  };

  template <ChoosePartView type>
  struct Leaf
  {
    typedef typename LeafPartView<GridType, type>::Type Type;
  };

  template <ChooseLayer type>
  struct View
  {
    typedef typename LayerView<GridType, type>::Type Type;
  };

  template <ChooseLayer type>
  struct Part
  {
    typedef typename LayerView<GridType, type>::Type Type;
  };

  typedef typename Layer<ChooseLayer::level, ChoosePartView::view>::Type LevelGridViewType;
#if HAVE_DUNE_FEM
  typedef typename Layer<ChooseLayer::level, ChoosePartView::part>::Type LevelGridPartType;
#endif

  typedef typename Layer<ChooseLayer::leaf, ChoosePartView::view>::Type LeafGridViewType;
#if HAVE_DUNE_FEM
  typedef typename Layer<ChooseLayer::leaf, ChoosePartView::part>::Type LeafGridPartType;
#endif

  static const std::string static_id()
  {
    return "grid.provider";
  }

  virtual ~ConstProviderInterface()
  {
  }

  virtual const GridType& grid() const = 0;

  template <ChooseLayer layer_type, ChoosePartView part_view_type>
  typename Layer<layer_type, part_view_type>::Type layer(const int level_in = 0) const
  {
    return Grid::Layer<GridType, layer_type, part_view_type>::create(grid(), level_in);
  }

  template <ChoosePartView type>
  typename Level<type>::Type level(const int level_in) const
  {
    return LevelPartView<GridType, type>::create(grid(), level_in);
  }

  LevelGridViewType level_view(const int level_in) const
  {
    return this->template level<ChoosePartView::view>(level_in);
  }

  template <ChoosePartView type>
  typename Leaf<type>::Type leaf() const
  {
    return LeafPartView<GridType, type>::create(grid());
  }

  LeafGridViewType leaf_view() const
  {
    return this->template leaf<ChoosePartView::view>();
  }

  virtual void visualize(const std::string filename = static_id(),
                         const Common::Configuration& boundary_info_cfg = Common::Configuration()) const
  {
    if (boundary_info_cfg.empty())
      visualize_plain(filename);
    else
      visualize_with_boundary(boundary_info_cfg, filename);
  } // ... visualize(...)

  virtual void visualize(const Common::Configuration& boundary_info_cfg) const
  {
    visualize_with_boundary(boundary_info_cfg, static_id());
  }

  virtual void visualize(const Common::Configuration& boundary_info_cfg, const std::string filename) const
  {
    visualize_with_boundary(boundary_info_cfg, filename);
  }

private:
  virtual void visualize_plain(const std::string filename) const
  {
    if (GridType::dimension > 3) // give us a call if you have any idea!
      DUNE_THROW(NotImplemented, "For grids of dimension > 3!");
    for (auto lvl : Common::value_range(grid().maxLevel() + 1)) {
      auto grid_view = level_view(lvl);
      // vtk writer
      Dune::VTKWriter<LevelGridViewType> vtkwriter(grid_view);
      // codim 0 entity id
      std::vector<double> entityId = generateEntityVisualization(grid_view);
      vtkwriter.addCellData(entityId, "entity_id__level_" + Common::to_string(lvl));
#if DUNE_GRID_EXPERIMENTAL_GRID_EXTENSIONS
      // boundary id
      std::vector<double> boundaryId = generateBoundaryIdVisualization(grid_view);
      vtkwriter.addCellData(boundaryId, "boundary_id__level_" + Common::to_string(lvl));
#endif
      // write
      vtkwriter.write(filename + "__level_" + Common::to_string(lvl), VTK::appendedraw);
    }
  } // ... visualize_plain(...)

  virtual void visualize_with_boundary(const Common::Configuration& boundary_info_cfg, const std::string filename) const
  {
    if (GridType::dimension > 3) // give us a call if you have any idea!
      DUNE_THROW(NotImplemented, "For grids of dimension > 3!");
    // boundary info
    typedef XT::Grid::BoundaryInfoFactory<typename LevelGridViewType::Intersection> BoundaryInfoFactory;
    auto boundary_info_ptr = BoundaryInfoFactory::create(boundary_info_cfg.get<std::string>("type"), boundary_info_cfg);
    for (auto lvl : Common::value_range(grid().maxLevel() + 1)) {
      auto grid_view = level_view(lvl);
      // vtk writer
      Dune::VTKWriter<LevelGridViewType> vtkwriter(grid_view);
      // codim 0 entity id
      std::vector<double> entityId = generateEntityVisualization(grid_view);
      vtkwriter.addCellData(entityId, "entity_id__level_" + Common::to_string(lvl));
#if DUNE_GRID_EXPERIMENTAL_GRID_EXTENSIONS
      // boundary id
      std::vector<double> boundaryId = generateBoundaryIdVisualization(grid_view);
      vtkwriter.addCellData(boundaryId, "boundary_id__level_" + Common::to_string(lvl));
#endif
      // dirichlet values
      std::vector<double> dirichlet = generateBoundaryVisualization(grid_view, *boundary_info_ptr, "dirichlet");
      vtkwriter.addCellData(dirichlet, "isDirichletBoundary__level_" + Common::to_string(lvl));
      // neumann values
      std::vector<double> neumann = generateBoundaryVisualization(grid_view, *boundary_info_ptr, "neumann");
      vtkwriter.addCellData(neumann, "isNeumannBoundary__level_" + Common::to_string(lvl));
      // write
      vtkwriter.write(filename + "__level_" + Common::to_string(lvl), VTK::appendedraw);
    }
  } // ... visualize_with_boundary(...)

#if DUNE_GRID_EXPERIMENTAL_GRID_EXTENSIONS
  std::vector<double> generateBoundaryIdVisualization(const LevelGridViewType& gridView) const
  {
    std::vector<double> data(gridView.indexSet().size(0));
    // walk the grid
    const auto it_end = gridView.template end<0>();
    for (auto it = gridView.template begin<0>(); it != it_end; ++it) {
      const auto& entity              = *it;
      const auto& index               = gridView.indexSet().index(entity);
      data[index]                     = 0.0;
      size_t numberOfBoundarySegments = 0;
      bool isOnBoundary               = false;
      const auto intersectionItEnd = gridView.iend(entity);
      for (auto intersectionIt = gridView.ibegin(entity); intersectionIt != intersectionItEnd; ++intersectionIt) {
        if (!intersectionIt->neighbor() && intersectionIt->boundary()) {
          isOnBoundary = true;
          numberOfBoundarySegments += 1;
          data[index] += double(intersectionIt->boundaryId());
        }
      }
      if (isOnBoundary) {
        data[index] /= double(numberOfBoundarySegments);
      }
    } // walk the grid
    return data;
  }
#else
  std::vector<double> generateBoundaryIdVisualization(const LevelGridViewType& /*gridView*/) const
  {
    DUNE_THROW(InvalidStateException, "Experimental Grid Extensions missing");
  }
#endif

  template <class BoundaryInfoType>
  std::vector<double> generateBoundaryVisualization(const LevelGridViewType& gridView,
                                                    const BoundaryInfoType& boundaryInfo, const std::string type) const
  {
    constexpr DirichletBoundary dirichlet_type{};
    constexpr NeumannBoundary neumann_type{};
    std::vector<double> data(gridView.indexSet().size(0));
    // walk the grid
    for (auto&& entity : elements(gridView)) {
      const auto& index = gridView.indexSet().index(entity);
      data[index] = 0.0;
      for (auto intersectionIt = gridView.ibegin(entity); intersectionIt != gridView.iend(entity); ++intersectionIt) {
        if (type == "dirichlet") {
          if (boundaryInfo.type(*intersectionIt) == dirichlet_type)
            data[index] = 1.0;
        } else if (type == "neumann") {
          if (boundaryInfo.type(*intersectionIt) == neumann_type)
            data[index] = 1.0;
        } else
          DUNE_THROW(Common::Exceptions::internal_error, "Unknown type '" << type << "'!");
      }
    } // walk the grid
    return data;
  } // std::vector< double > generateBoundaryVisualization(...) const

  std::vector<double> generateEntityVisualization(const LevelGridViewType& gridView) const
  {
    std::vector<double> data(gridView.indexSet().size(0));
    for (auto&& entity : elements(gridView)) {
      const auto& index = gridView.indexSet().index(entity);
      data[index]       = double(index);
    } // walk the grid
    return data;
  } // ... generateEntityVisualization(...)
}; // class ConstProviderInterface

template <class GridImp>
class ProviderInterface : public ConstProviderInterface<GridImp>
{
  typedef ConstProviderInterface<GridImp> BaseType;

public:
  using typename BaseType::GridType;
  using typename BaseType::LevelGridViewType;
  using typename BaseType::LeafGridViewType;

#if HAVE_DUNE_FEM
  using typename BaseType::LevelGridPartType;
  using typename BaseType::LeafGridPartType;
#endif

  static const std::string static_id()
  {
    return BaseType::static_id();
  }

  virtual ~ProviderInterface()
  {
  }

  using BaseType::grid;

  virtual GridType& grid() = 0;

  using BaseType::layer;

  template <ChooseLayer layer_type, ChoosePartView part_view_type>
  typename BaseType::template Layer<layer_type, part_view_type>::Type layer(const int level_in = 0)
  {
    return Grid::Layer<GridType, layer_type, part_view_type>::create(grid(), level_in);
  }

  using BaseType::level;

  template <ChoosePartView type>
  typename BaseType::template Level<type>::Type level(const int level_in)
  {
    return LevelPartView<GridType, type>::create(grid(), level_in);
  }

#if HAVE_DUNE_FEM

  LevelGridPartType level_part(const int level_in)
  {
    return this->template level<ChoosePartView::part>(level_in);
  }

#endif // HAVE_DUNE_FEM

  using BaseType::leaf;

  template <ChoosePartView type>
  typename BaseType::template Leaf<type>::Type leaf()
  {
    return LeafPartView<GridType, type>::create(grid());
  }

#if HAVE_DUNE_FEM

  LeafGridPartType leaf_part()
  {
    return this->template leaf<ChoosePartView::part>();
  }

#endif // HAVE_DUNE_FEM
}; // class ProviderInterface


} // namespace Grid
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_GRID_PROVIDER_INTERFACE_HH
