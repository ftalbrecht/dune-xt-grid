// This file is part of the dune-xt-grid project:
//   https://github.com/dune-community/dune-xt-grid
// Copyright 2009-2018 dune-xt-grid developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2017)
//   Rene Milk       (2018)
//
// This file is copied here from https://gitlab.dune-project.org/dune-fem/dune-fem/, rev fe283ebc, as a temporary
// workaround until https://gitlab.dune-project.org/dune-fem/dune-fem/issues/59 is fixed.

// clang-format off

#ifndef DUNE_XT_GRID_VIEW_GRIDVIEW2GRIDPART_HH
#define DUNE_XT_GRID_VIEW_GRIDVIEW2GRIDPART_HH

#if HAVE_DUNE_FEM

#include <utility>

#include <dune/common/exceptions.hh>

#include <dune/grid/common/gridenums.hh>

#include <dune/fem/gridpart/common/gridpart.hh>
#include <dune/fem/gridpart/common/nonadaptiveindexset.hh>
#include <dune/fem/quadrature/caching/twistutility.hh>
#include <dune/fem/space/common/dofmanager.hh>

namespace Dune
{
namespace XT
{
namespace Grid
{

    // Internal forward declaration
    // ----------------------------

    template< class GridView >
    class GridView2GridPart;



#ifndef DOXYGEN

    // GridView2GridPartTraits
    // -----------------------

    template< class GridView >
    struct GridView2GridPartTraits
    {
      typedef GridView2GridPart< GridView > GridPartType;

      typedef GridView GridViewType;
      static const bool conforming = GridView::conforming;

      typedef typename GridViewType::Grid GridType;
      typedef typename GridViewType::CollectiveCommunication CollectiveCommunicationType;

      typedef Fem::NonAdaptiveIndexSet< typename GridView::IndexSet > IndexSetType;

      template< int codim >
      struct Codim
      {
        typedef typename GridViewType::template Codim< codim >::Entity EntityType;
        typedef typename GridType::template Codim< codim >::EntitySeed EntitySeedType;

        typedef typename GridViewType::template Codim< codim >::Geometry GeometryType;
        typedef typename GridViewType::template Codim< codim >::LocalGeometry LocalGeometryType;

        template< PartitionIteratorType pitype >
        struct Partition
        {
          typedef typename GridViewType::template Codim< codim >::template Partition< pitype >::Iterator IteratorType;
        };
      };

      typedef typename GridViewType::IntersectionIterator IntersectionIteratorType;

      typedef Fem::TwistUtility< GridType > TwistUtilityType;

      static const PartitionIteratorType indexSetPartitionType = All_Partition;
      static const InterfaceType indexSetInterfaceType = All_All_Interface;
    };

#endif // #ifndef DOXYGEN



    // GridView2GridPart
    // -----------------

    template< class GridView >
    class GridView2GridPart
      : public Fem::GridPartInterface< GridView2GridPartTraits< GridView > >
    {
      typedef GridView2GridPart< GridView > ThisType;
      typedef GridView2GridPartTraits< GridView > TraitsType;
      typedef Fem::GridPartInterface< TraitsType > BaseType;

    public:
      /** \copydoc Dune::Fem::GridPartInterface::GridType */
      typedef typename BaseType::GridType GridType;

      /** \copydoc Dune::Fem::GridPartInterface::GridViewType */
      typedef typename BaseType::GridViewType GridViewType;

      template< int codim >
      struct Codim
        : public BaseType::template Codim< codim >
      {};

      /** \copydoc Dune::Fem::GridPartInterface::IntersectionIteratorType */
      typedef typename BaseType::IntersectionIteratorType IntersectionIteratorType;

      /** \copydoc Dune::Fem::GridPartInterface::IndexSetType */
      typedef typename BaseType::IndexSetType IndexSetType;

      /** \copydoc Dune::Fem::GridPartInterface::CollectiveCommunicationType */
      typedef typename BaseType::CollectiveCommunicationType CollectiveCommunicationType;

    private:
      typedef Fem::DofManager< GridType > DofManagerType;

    public:

      /** \name Construction
       *  \{
       */

      explicit GridView2GridPart ( const GridView &gridView )
        : gridView_( gridView ),
          indexSet_( gridView_.indexSet() ),
          dofManager_( DofManagerType::instance( gridView_.grid() ) )
      {}

      explicit GridView2GridPart ( GridView &&gridView )
        : gridView_( std::move( gridView ) ),
          indexSet_( gridView_.indexSet() ),
          dofManager_( DofManagerType::instance( gridView_.grid() ) )
      {}

      GridView2GridPart ( const ThisType &rhs )
        : gridView_( rhs.gridView_ ),
          indexSet_( gridView_.indexSet() ),
          dofManager_( DofManagerType::instance( rhs.grid() ) )
      {}

      /** \} */

      /** \name Public member methods
       *  \{
       */

      /** \copydoc Dune::Fem::GridPartInterface::grid */
      const GridType &grid () const { return gridView_.grid(); }

      /** \copydoc Dune::Fem::GridPartInterface::indexSet */
      const IndexSetType &indexSet () const { return indexSet_; }

      /** \copydoc Dune::Fem::GridPartInterface::begin */
      template< int codim >
      typename Codim< codim >::IteratorType begin () const
      {
        return begin< codim, InteriorBorder_Partition >();
      }

      /** \copydoc Dune::Fem::GridPartInterface::begin */
      template< int codim, PartitionIteratorType pitype >
      typename Codim< codim >::template Partition< pitype >::IteratorType begin () const
      {
        return gridView_.template begin< codim, pitype >();
      }

      /** \copydoc Dune::Fem::GridPartInterface::end */
      template< int codim >
      typename Codim< codim >::IteratorType end () const
      {
        return end< codim, InteriorBorder_Partition >();
      }

      /** \copydoc Dune::Fem::GridPartInterface::end */
      template< int codim, PartitionIteratorType pitype >
      typename Codim< codim >::template Partition< pitype >::IteratorType end () const
      {
        return gridView_.template end< codim, pitype >();
      }

      /** \copydoc Dune::Fem::GridPartInterface::ibegin */
      IntersectionIteratorType ibegin ( const typename Codim< 0 >::EntityType &entity ) const
      {
        return gridView_.ibegin( entity );
      }

      /** \copydoc Dune::Fem::GridPartInterface::iend */
      IntersectionIteratorType iend ( const typename Codim< 0 >::EntityType &entity ) const
      {
        return gridView_.iend( entity );
      }

      /** \copydoc Dune::Fem::GridPartInterface::boundaryId */
      int boundaryId ( const typename BaseType::IntersectionType &intersection ) const
      {
#if DUNE_GRID_EXPERIMENTAL_GRID_EXTENSIONS
        return intersection.boundaryId();
#else
        DUNE_THROW( NotImplemented, "Method boundaryId() not implemented" );
#endif
      }

      /** \copydoc Dune::Fem::GridPartInterface::comm */
      const CollectiveCommunicationType &comm () const { return gridView_.comm(); }

      /** \copydoc Dune::Fem::GridPartInterface::communicate */
      template< class DataHandle, class DataType >
      void communicate ( CommDataHandleIF< DataHandle, DataType > &dataHandle,
                         InterfaceType interface, CommunicationDirection direction ) const
      {
        gridView_.communicate( dataHandle, interface, direction );
      }

      /** \copydoc Dune::Fem::GridPartInterface::sequence */
      int sequence () const { return dofManager_.sequence(); }

      /** \copydoc Dune::Fem::GridPartInterface::entity */
      template < class EntitySeed >
      typename Codim< EntitySeed::codimension >::EntityType
      entity ( const EntitySeed &seed ) const
      {
        return grid().entity( seed );
      }

      /** \copydoc Dune::Fem::GridPartInterface::sequence */
      template <class Entity>
      const Entity &convert( const Entity& entity ) const
      {
        return convert< Entity::codimension >( entity );
      }

      /** \brief cast to underlying grid view */
      explicit operator GridView () const { return gridView_; }

      /** \} */

    private:
      template< int codim >
      const typename Codim< codim >::EntityType &
      convert( const typename Codim< codim >::EntityType &entity ) const
      {
        return entity;
      }

      GridView gridView_;
      IndexSetType indexSet_;
      DofManagerType &dofManager_;
    };

} // namespace Grid
} // namespace XT
} // namespace Dune


#else // HAVE_DUNE_FEM


#include <type_traits>

#include <dune/common/typetraits.hh>

namespace Dune {
namespace XT {
namespace Grid {


template <class GridView>
class GridView2GridPart
{
  static_assert(AlwaysFalse<GridView>::value, "You are missing dune-fem!");
};


} // namespace Grid
} // namespace XT
} // namespace Dune


#endif // HAVE_DUNE_FEM
#endif // #ifndef DUNE_XT_GRID_VIEW_GRIDVIEW2GRIDPART_HH

// clang-format on
