#include <gtest/gtest.h>

#include "mixable.hpp"

#include <sstream>

using namespace std;

namespace jubatus {
namespace framework {

struct int_model {
  int_model() : value(0) {}

  int value;

  void save(std::ostream & ofs) {
    ofs << value;
  }

  void load(std::istream & ifs) {
    ifs >> value;
  }
};

class mixable_int : public mixable<int_model, int> {
 public:
  mixable_int() : diff_() {}

  void clear() {}

  int get_diff_impl() const {
    return diff_;
  }

  void put_diff_impl(const int& n) {
    get_model()->value += n;
    diff_ = 0;
  }

  void mix_impl(const int& lhs, const int& rhs, int& mixed) const {
    mixed = lhs + rhs;
  }

  string get_pull_argument_impl() const {
    return string();
  }

  int pull_impl(const string&) const {
    return get_diff_impl();
  }

  void push_impl(const int& d) {
    put_diff_impl(d + diff_);
  }

  void add(int n) {
    diff_ += n;
  }

 private:
  int diff_;
};

TEST(mixable, config_not_set) {
  mixable_int m;
  EXPECT_THROW(m.get_diff(), config_not_set);
  EXPECT_THROW(m.put_diff(""), config_not_set);
}

TEST(mixable, save_load) {
  mixable_int m;
  m.set_model(mixable_int::model_ptr(new int_model));
  m.get_model()->value = 10;

  stringstream ss;
  m.save(ss);
  m.get_model()->value = 5;
  m.load(ss);
  EXPECT_EQ(10, m.get_model()->value);
}

TEST(mixable, trivial) {
  mixable_int m;
  m.set_model(mixable_int::model_ptr(new int_model));

  m.add(10);

  string diff1 = m.get_diff();
  string diff2 = m.get_diff();

  std::string mixed;
  m.mix(diff1, diff2, mixed);

  m.put_diff(mixed);

  EXPECT_EQ(20, m.get_model()->value);
}

}
}
