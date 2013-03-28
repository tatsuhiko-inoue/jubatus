#ifndef JUBATUS_SERVER_LINEAR_FUNCTION_MIXER_HPP_
#define JUBATUS_SERVER_LINEAR_FUNCTION_MIXER_HPP_

#include "../framework.hpp"
#include "../storage/storage_base.hpp"

#include "diffv.hpp"

namespace jubatus {
namespace server {

class linear_function_mixer
    : public jubatus::framework::mixable<storage::storage_base, diffv> {
 public:
  diffv get_diff_impl() const;
  void mix_impl(const diffv& lhs, const diffv& rhs, diffv& mixed) const;
  void put_diff_impl(const diffv& v);

  std::string get_pull_argument_impl() const;
  diffv pull_impl(const std::string&) const;
  void push_impl(const diffv& v);

  void clear();
};

}
}

#endif  // JUBATUS_SERVER_LINEAR_FUNCTION_MIXER_HPP_
