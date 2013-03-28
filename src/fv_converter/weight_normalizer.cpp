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

#include "weight_normalizer.hpp"

#include <cmath>
#include <iostream>
#include <pficommon/data/serialization.h>
#include "../common/exception.hpp"
#include "../table/column/column_table.hpp"

using namespace std;
using namespace jubatus::table;

namespace jubatus {
namespace fv_converter {

namespace {
owner get_owner() {
  return owner();  // FIXME!!!!!!
}

void extract_float_column(const column_table* table,
                          uint64_t col_id,
                          vector<pair<string, float> >& diff) {
  const_float_column col = table->get_float_column(col_id);

  diff.clear();
  diff.reserve(table->size());
  for (size_t i = 0; i < table->size(); ++i) {
    diff.push_back(make_pair(table->get_key(i), col[i]));
  }
}

void merge_by_average(const vector<pair<string, float> >& diff,
                      column_table* table) {
  float_column col = table->get_float_column(0);

  for (size_t i = 0; i < col.size(); ++i) {
    col[i] /= 2;
  }
  for (size_t i = 0; i < diff.size(); ++i) {
    const pair<bool, uint64_t> hit = table->exact_match(diff[i].first);
    if (hit.first) {
      col[hit.second] += diff[i].second / 2;
    } else {
      table->add(diff[i].first, get_owner(), diff[i].second / 2);
    }
  }
}

void merge_by_overwrite(const vector<pair<string, float> >& diff,
                        column_table* table) {
  for (size_t i = 0; i < diff.size(); ++i) {
    table->add(diff[i].first, get_owner(), diff[i].second);
  }
}
}

weight_normalizer::weight_normalizer(table::column_table* df_table,
                                     table::column_table* weight_table)
    : df_table_(df_table),
      weight_table_(weight_table),
      document_count_(0) {
  if (!df_table_) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("df_table is NULL"));
  }
  if (!weight_table_) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("weight_table is NULL"));
  }

  vector<column_type> schema;
  schema.push_back(column_type(column_type::float_type));
  df_table_->init(schema);
  weight_table_->init(schema);
}

void weight_normalizer::update(const sfv_t& fv) {
  float_column df = df_table_->get_float_column(0);

  document_count_ += 1;
  for (size_t i = 0; i < fv.size(); ++i) {
    const pair<bool, uint64_t> hit = df_table_->exact_match(fv[i].first);
    if (!hit.first) {
      df_table_->add(fv[i].first, get_owner(), 1.f);
    } else {
      // TODO: this modification does not count up its version
      ++df[hit.second];
    }
  }
}

void weight_normalizer::normalize(sfv_t& fv) const {
  for (size_t i = 0; i < fv.size(); ++i) {
    fv[i].second *= get_global_weight(fv[i].first);
  }
}

void weight_normalizer::add_weight(const string& key, float weight) {
  weight_table_->add(key, get_owner(), weight);
}

weight_normalizer_diff weight_normalizer::pull() const {
  weight_normalizer_diff diff;
  diff.document_count = document_count_;

  extract_float_column(df_table_, 0, diff.dfs);
  extract_float_column(weight_table_, 0, diff.weights);

  return diff;
}

void weight_normalizer::push(const weight_normalizer_diff& diff) {
  document_count_ = (document_count_ + diff.document_count) / 2;
  merge_by_average(diff.dfs, df_table_);
  merge_by_overwrite(diff.weights, weight_table_);
}

void weight_normalizer::save(ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << document_count_;
  oa << *df_table_;
  oa << *weight_table_;
}

void weight_normalizer::load(istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> document_count_;
  ia >> *df_table_;
  ia >> *weight_table_;
}

double weight_normalizer::get_global_weight(const string& key) const {
  const size_t type_p = key.find_last_of('/');
  if (type_p != string::npos) {
    const string type = key.substr(type_p + 1);
    if (type == "idf") {
      return log(document_count_ / get_document_frequency(key));
    }
    if (type == "weight") {
      const size_t weight_p = key.find_last_of('#');
      return weight_p == string::npos ? 0 : get_user_weight(key.substr(0, weight_p));
    }
  }

  return 1;
}

double weight_normalizer::get_document_frequency(const string& key) const {
  const pair<bool, uint64_t> hit = df_table_->exact_match(key);
  if (!hit.first) {
    return 0;
  }
  const_float_column df = df_table_->get_float_column(0);
  return df[hit.second];
}

double weight_normalizer::get_user_weight(const string& key) const {
  const pair<bool, uint64_t> hit = weight_table_->exact_match(key);
  if (!hit.first) {
    return 0;
  }
  const_float_column user_weight = weight_table_->get_float_column(0);
  return user_weight[hit.second];
}

}
}
