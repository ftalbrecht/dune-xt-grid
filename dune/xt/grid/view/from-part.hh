#ifndef DUNE_XT_GRID_VIEW_FROM_PART_HH
#define DUNE_XT_GRID_VIEW_FROM_PART_HH

#include <type_traits>

#include <dune/common/typetraits.hh>

#include <dune/xt/common/type_traits.hh>
#include <dune/xt/grid/type_traits.hh>

namespace Dune {
namespace XT {
namespace Grid {


template <class GridLayerType>
class TemporaryConstView
{
  static_assert(is_layer<GridLayerType>::value, "");

  template <bool view = is_view<GridLayerType>::value, bool part = is_part<GridLayerType>::value, bool anything = true>
  struct const_storage
  {
    static_assert(AlwaysFalse<typename Common::dependent<bool>::_typename<view>::type>::value,
                  "This case is not covered (yet?)!");
  };

  template <bool anything>
  struct const_storage<true, false, anything>
  {
    typedef GridLayerType type;

    const_storage(const GridLayerType& grid_view)
      : value(grid_view)
    {
    }

    const type& value;
  };

  template <bool anything>
  struct const_storage<false, true, anything>
  {
    typedef typename GridLayerType::GridViewType type;

    const_storage(const GridLayerType& grid_part)
      : grid_part_(grid_part)
      , value(grid_part_)
    {
    }

    const GridLayerType& grid_part_;
    const type value;
  };

public:
  using type = typename const_storage<>::type;

  TemporaryConstView(const GridLayerType& grid_layer)
    : const_storage_(grid_layer)
  {
  }

  const type& access() const
  {
    return const_storage_.value;
  }

private:
  const const_storage<> const_storage_;
}; // class TemporaryConstView


template <class GridLayerType>
class TemporaryView : public TemporaryConstView<GridLayerType>
{
  template <bool view = is_view<GridLayerType>::value, bool part = is_part<GridLayerType>::value, bool anything = true>
  struct storage
  {
    static_assert(AlwaysFalse<typename Common::dependent<bool>::_typename<view>::type>::value,
                  "This case is not covered (yet?)!");
  };

  template <bool anything>
  struct storage<true, false, anything>
  {
    typedef GridLayerType type;

    storage(GridLayerType& grid_view)
      : value(grid_view)
    {
    }

    type& value;
  };

  template <bool anything>
  struct storage<false, true, anything>
  {
    typedef typename GridLayerType::GridViewType type;

    storage(GridLayerType& grid_part)
      : value(grid_part)
    {
    }

    type value;
  };

  using BaseType = TemporaryConstView<GridLayerType>;

public:
  using type = typename storage<>::type;

  TemporaryView(GridLayerType& grid_layer)
    : BaseType(grid_layer)
    , storage_(grid_layer)
  {
  }

  using BaseType::access;

  type& access()
  {
    return storage_.value;
  }

private:
  storage<> storage_;
}; // class TemporaryView


template <class GridLayerType>
TemporaryConstView<GridLayerType> make_tmp_view(const GridLayerType& grid_layer)
{
  return TemporaryConstView<GridLayerType>(grid_layer);
}

template <class GridLayerType>
TemporaryView<GridLayerType> make_tmp_view(GridLayerType& grid_layer)
{
  return TemporaryView<GridLayerType>(grid_layer);
}


} // namespace Grid
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_GRID_VIEW_FROM_PART_HH