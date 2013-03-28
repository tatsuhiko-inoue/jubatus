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

#pragma once

#include <iosfwd>
#include <msgpack.hpp>
#include <pficommon/data/serialization.h>
#include "../common/type.hpp"
#include "../table/column/column_table.hpp"

namespace jubatus {
namespace fv_converter {

struct weight_normalizer_diff {
  weight_normalizer_diff()
      : document_count(0) {}

  MSGPACK_DEFINE(document_count, dfs, weights);

  template<class Archiver>
  void serialize(Archiver& ar) {
    ar & MEMBER(document_count) & MEMBER(dfs) & MEMBER(weights);
  }

  float document_count;
  std::vector<std::pair<std::string, float> > dfs;
  std::vector<std::pair<std::string, float> > weights;
};

class weight_normalizer {
public:
  weight_normalizer(table::column_table* df_table,
                    table::column_table* weight_table);

  void update(const sfv_t& fv);
  void normalize(sfv_t& fv) const;
  void add_weight(const std::string& key, float weight);

  weight_normalizer_diff pull() const;
  void push(const weight_normalizer_diff& diff);

  void save(std::ostream& os);
  void load(std::istream& is);

private:
  double get_global_weight(const std::string& key) const;
  double get_document_frequency(const std::string& key) const;
  double get_user_weight(const std::string& key) const;

  table::column_table* df_table_;
  table::column_table* weight_table_;
  double document_count_;
};

}
}
