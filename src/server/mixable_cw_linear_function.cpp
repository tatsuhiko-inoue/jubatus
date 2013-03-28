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

#include "mixable_cw_linear_function.hpp"

using namespace std;
using namespace jubatus::table;

namespace jubatus {
namespace server {

weight_variance_diff mixable_cw_linear_function::get_diff_impl() const {
  model_ptr table = get_model();
  const size_t table_size = table->size();
  const_float_column weights = table->get_float_column(0);
  const_float_column variances = table->get_float_column(1);

  weight_variance_diff diff(1);
  diff.wv.reserve(table_size);
  for (size_t i = 0; i < table_size; ++i) {
    diff.wv.push_back(make_pair(table->get_key(i), make_pair(weights[i], variances[i])));
  }

  return diff;
}

void mixable_cw_linear_function::mix_impl(
    const weight_variance_diff& lhs,
    const weight_variance_diff& rhs,
    weight_variance_diff& mixed) const {
  mixed.mix_count = lhs.mix_count + rhs.mix_count;
  const float l_rate = float(lhs.mix_count) / mixed.mix_count;
  const float r_rate = float(rhs.mix_count) / mixed.mix_count;

  vector<pair<string, pair<float, float> > > l(lhs.wv), r(rhs.wv);
  sort(l.begin(), l.end());
  sort(r.begin(), r.end());

  for (size_t li = 0, ri = 0; li < l.size() && ri < r.size(); ) {
    const int sign = l[li].first.compare(r[ri].first);
    if (sign == 0) {
      const float w = l_rate * l[li].second.first + r_rate * r[ri].second.first;
      const float v = min(l[li].second.second, r[ri].second.second);
      mixed.wv.push_back(make_pair(l[li].first, make_pair(w, v)));
      ++li;
      ++ri;
    } else if (sign < 0) {
      const float w = l_rate * l[li].second.first;
      const float v = l[li].second.second;
      mixed.wv.push_back(make_pair(l[li].first, make_pair(w, v)));
      ++li;
    } else {
      const float w = r_rate * r[ri].second.first;
      const float v = r[ri].second.second;
      mixed.wv.push_back(make_pair(l[li].first, make_pair(w, v)));
      ++ri;
    }
  }
}

void mixable_cw_linear_function::put_diff_impl(
    const weight_variance_diff& v) {
  model_ptr table = get_model();
  for (size_t i = 0; i < v.wv.size(); ++i) {
    table->add(v.wv[i].first, v.wv[i].second.first, v.wv[i].second.second);
  }
}

string mixable_cw_linear_function::get_pull_argument_impl() const {
  return string();
}

weight_variance_diff mixable_cw_linear_function::pull_impl(
    const string&) const {
  model_ptr table = get_model();
  const size_t table_size = table->size();
  const_float_column weights = table->get_float_column(0);
  const_float_column variances = table->get_float_column(1);

  weight_variance_diff diff(0);
  for (size_t i = 0; i < table_size; ++i) {
    diff.wv.push_back(make_pair(table->get_key(i), make_pair(weights[i], variances[i])));
  }

  return diff;
}

void mixable_cw_linear_function::push_impl(
    const weight_variance_diff& v) {
  model_ptr table = get_model();
  float_column weights = table->get_float_column(0);
  float_column variances = table->get_float_column(1);

  for (size_t i = 0; i < weights.size(); ++i) {
    weights[i] /= 2;
  }
  for (size_t i = 0; i < v.wv.size(); ++i) {
    const pair<bool, uint64_t> hit = table->exact_match(v.wv[i].first);
    if (hit.first) {
      weights[hit.second] += v.wv[i].second.first / 2;
      if (variances[hit.second] > v.wv[i].second.second) {
        variances[hit.second] = v.wv[i].second.second;
      }
    } else {
      table->add(v.wv[i].first, v.wv[i].second.first / 2, v.wv[i].second.second);
    }
  }
}

}
}
