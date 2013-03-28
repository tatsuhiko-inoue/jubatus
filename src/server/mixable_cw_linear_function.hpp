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

#ifndef JUBATUS_SERVER_MIXABLE_CW_LINEAR_FUNCTION_HPP_
#define JUBATUS_SERVER_MIXABLE_CW_LINEAR_FUNCTION_HPP_

#include <string>
#include <utility>
#include <vector>
#include "../framework/mixable.hpp"
#include "../table/column/ss_column_table.hpp"

namespace jubatus {
namespace server {

struct weight_variance_diff {
  explicit weight_variance_diff(int mix_count)
      : mix_count(mix_count) {}

  MSGPACK_DEFINE(mix_count, wv);

  template<class Archive>
  void serialize(Archive& ar) {
    ar & MEMBER(mix_count) & MEMBER(wv);
  }

  int mix_count;
  std::vector<std::pair<std::string, std::pair<float, float> > > wv;
};

class mixable_cw_linear_function
    : public framework::mixable<table::ss_column_table, weight_variance_diff> {
public:
  weight_variance_diff get_diff_impl() const;
  void mix_impl(const weight_variance_diff& lhs,
                const weight_variance_diff& rhs,
                weight_variance_diff& mixed) const;
  void put_diff_impl(const weight_variance_diff& v);

  std::string get_pull_argument_impl() const;
  weight_variance_diff pull_impl(const std::string&) const;
  void push_impl(const weight_variance_diff& v);

  void clear();
};

}
}

#endif  // JUBATUS_SERVER_MIXABLE_CW_LINEAR_FUNCTION_HPP_
