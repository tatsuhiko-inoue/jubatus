// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef JUBATUS_CLASSIFIER_PERCEPTRON_HPP_
#define JUBATUS_CLASSIFIER_PERCEPTRON_HPP_

#include <string>

#include "classifier_base.hpp"

namespace jubatus {
namespace classifier {

class perceptron : public classifier_base {
 public:
  explicit perceptron(storage::storage_base* storage);
  void train(const sfv_t& sfv, const std::string& label);
  std::string name() const;
};

}  // namespace classifier
}  // namespace jubatus

#endif  // JUBATUS_CLASSIFIER_PERCEPTRON_HPP_
