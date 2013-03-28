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

#ifndef JUBATUS_SERVER_MIXABLE_LINEAR_FUNCTION_HPP_
#define JUBATUS_SERVER_MIXABLE_LINEAR_FUNCTION_HPP_

#include "../common/type.hpp"
#include "../framework/mixable.hpp"
#include "../table/column/ss_column_table.hpp"

namespace jubatus {
namespace server {

struct weight_diff {
  explicit weight_diff(int mix_count = 1)
      : mix_count(mix_count) {}

  MSGPACK_DEFINE(mix_count, weight);

  template<class Archive>
  void serialize(Archive& ar) {
    ar & MEMBER(mix_count) & MEMBER(weight);
  }

  int mix_count;
  sfv_t weight;
};

class mixable_linear_function
    : public framework::mixable<table::ss_column_table, weight_diff> {
public:
  weight_diff get_diff_impl() const;
  void mix_impl(const weight_diff& lhs,
                const weight_diff& rhs,
                weight_diff& mixed) const;
  void put_diff_impl(const weight_diff& v);

  std::string get_pull_argument_impl() const;
  weight_diff pull_impl(const std::string&) const;
  void push_impl(const weight_diff& v);

  void clear();
};

}
}

#endif  // JUBATUS_SERVER_MIXABLE_LINEAR_FUNCTION_HPP_
