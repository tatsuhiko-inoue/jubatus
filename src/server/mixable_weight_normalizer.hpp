// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef JUBATUS_SERVER_MIXABLE_WEIGHT_NORMALIZER_HPP_
#define JUBATUS_SERVER_MIXABLE_WEIGHT_NORMALIZER_HPP_

#include "../framework/mixable.hpp"
#include "../fv_converter/weight_normalizer.hpp"
#include "../table/column/column_table.hpp"

namespace jubatus {
namespace server {

class mixable_weight_normalizer
    : public framework::mixable<fv_converter::weight_normalizer,
                                fv_converter::weight_normalizer_diff> {
public:
  fv_converter::weight_normalizer_diff get_diff_impl() const;
  void mix_impl(const fv_converter::weight_normalizer_diff& lhs,
                const fv_converter::weight_normalizer_diff& rhs,
                fv_converter::weight_normalizer_diff& mixed) const;
  void put_diff_impl(const fv_converter::weight_normalizer_diff& v);

  std::string get_pull_argument_impl() const;
  fv_converter::weight_normalizer_diff pull_impl(const std::string&) const;
  void push_impl(const fv_converter::weight_normalizer_diff& v);

  void clear();
};

}
}

#endif  // JUBATUS_SERVER_MIXABLE_WEIGHT_NORMALIZER_HPP_
