// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "mixable_weight_normalizer.hpp"

#include "../common/exception.hpp"

using namespace std;
using namespace jubatus::fv_converter;

namespace jubatus {
namespace server {

weight_normalizer_diff mixable_weight_normalizer::get_diff_impl() const {
  throw JUBATUS_EXCEPTION(
      unsupported_method("mixable_weight_normalizer::get_diff_impl"));
}

void mixable_weight_normalizer::mix_impl(const weight_normalizer_diff&,
                                         const weight_normalizer_diff&,
                                         weight_normalizer_diff&) const {
  throw JUBATUS_EXCEPTION(unsupported_method("mixable_weight_normalizer::mix_impl"));
}

void mixable_weight_normalizer::put_diff_impl(const weight_normalizer_diff&) {
  throw JUBATUS_EXCEPTION(
      unsupported_method("mixable_weight_normalizer::put_diff_impl"));
}

string mixable_weight_normalizer::get_pull_argument_impl() const {
  return string();
}

weight_normalizer_diff mixable_weight_normalizer::pull_impl(const string&) const {
  return get_model()->pull();
}

void mixable_weight_normalizer::push_impl(const weight_normalizer_diff& v) {
  get_model()->push(v);
}

void mixable_weight_normalizer::clear() {}

}
}
