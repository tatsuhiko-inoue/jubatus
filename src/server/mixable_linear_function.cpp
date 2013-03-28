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

#include "mixable_linear_function.hpp"

#include <algorithm>

using namespace std;
using namespace jubatus::table;

namespace jubatus {
namespace server {

weight_diff mixable_linear_function::get_diff_impl() const {
  model_ptr table = get_model();
  const size_t table_size = table->size();
  const_float_column weights = table->get_float_column(0);

  weight_diff diff(1);
  diff.weight.reserve(table_size);
  for (size_t i = 0; i < table_size; ++i) {
    diff.weight.push_back(make_pair(table->get_key(i), weights[i]));
  }

  return diff;
}

void mixable_linear_function::mix_impl(const weight_diff& lhs,
                                       const weight_diff& rhs,
                                       weight_diff& mixed) const {
  mixed.mix_count = lhs.mix_count + rhs.mix_count;
  const float l_rate = float(lhs.mix_count) / mixed.mix_count;
  const float r_rate = float(rhs.mix_count) / mixed.mix_count;

  sfv_t l(lhs.weight), r(rhs.weight);
  sort(l.begin(), l.end());
  sort(r.begin(), r.end());

  for (size_t li = 0, ri = 0; li < l.size() && ri < r.size(); ) {
    const int sign = l[li].first.compare(r[ri].first);
    if (sign == 0) {
      const float w = l_rate * l[li].second + r_rate * r[ri].second;
      mixed.weight.push_back(make_pair(l[li].first, w));
      ++li;
      ++ri;
    } else if (sign < 0) {
      mixed.weight.push_back(make_pair(l[li].first, l_rate * l[li].second));
      ++li;
    } else {
      mixed.weight.push_back(make_pair(r[ri].first, r_rate * r[ri].second));
      ++ri;
    }
  }
}

void mixable_linear_function::put_diff_impl(const weight_diff& v) {
  model_ptr table = get_model();
  for (size_t i = 0; i < v.weight.size(); ++i) {
    table->add(v.weight[i].first, v.weight[i].second);
  }
}

string mixable_linear_function::get_pull_argument_impl() const {
  return string();
}

weight_diff mixable_linear_function::pull_impl(const string&) const {
  model_ptr table = get_model();
  const_float_column weights = table->get_float_column(0);

  weight_diff diff(0);
  for (size_t i = 0; i < weights.size(); ++i) {
    diff.weight.push_back(make_pair(table->get_key(i), weights[i]));
  }

  return diff;
}

void mixable_linear_function::push_impl(const weight_diff& v) {
  model_ptr table = get_model();
  float_column weights = table->get_float_column(0);

  for (size_t i = 0; i < weights.size(); ++i) {
    weights[i] /= 2;
  }
  for (size_t i = 0; i < v.weight.size(); ++i) {
    const pair<bool, uint64_t> hit = table->exact_match(v.weight[i].first);
    if (hit.first) {
      weights[hit.second] += v.weight[i].second / 2;
    } else {
      table->add(v.weight[i].first, v.weight[i].second / 2);
    }
  }
}

void mixable_linear_function::clear() {
}

}
}
