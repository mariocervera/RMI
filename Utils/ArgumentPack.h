#ifndef __INCLUDE_ARGUMENTPACK_H__
#define __INCLUDE_ARGUMENTPACK_H__

#include <cassert>
#include <initializer_list>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>

namespace elint
{

/// Generic argument passing class useful for passing arbitrary values on
/// factories or other creational classes. This class stores internally a pack
/// of keys and values.
class ArgumentPack
{
public:
  using Container = std::unordered_map<std::string, std::string>;
  using const_iterator = Container::const_iterator;

  ArgumentPack(
      std::initializer_list<std::pair<const std::string, std::string>> Init)
      : Args(Init)
  {
  }

  ArgumentPack() = default;

  /// Inserts into the pack the specified key and value. Upon duplicate keys
  /// only the first one will be used.
  void insert(const std::string &Key, const std::string &Value)
  {
    Args.emplace(Key, Value);
  }

  /// Returns a pair associated to the specified key. The first element of the
  /// pair indicates that the operation was successfull or not, whereas the
  /// second element is the Value associated.
  /// NOTE: On error, a default constructed value is returned, alongside with
  /// false.
  template <typename T> std::pair<bool, T> readAs(const std::string &Key) const
  {
    auto I = Args.find(Key);
    if (I == Args.cend())
      return {false, T()};

    T V;
    std::istringstream IS(I->second);

    IS >> V;

    assert(!IS.fail() && "Invalid conversion");
    return {!IS.fail(), IS.fail() ? T() : V};
  }

  /// Returns true if the specified key exists, otherwise returns false.
  bool exists(const std::string &Key) const { return Args.count(Key); }

  /// Returns the number of elements in the ArgumentPack.
  unsigned size() const { return Args.size(); }

  /// Returns true when the ArgumentPack is empty, false otherwise.
  bool empty() const { return Args.empty(); }

  /// Iterators.
  const_iterator begin() const { return Args.cbegin(); }
  const_iterator end() const { return Args.cend(); }

private:
  Container Args;
};

template <>
inline std::pair<bool, std::string>
ArgumentPack::readAs<std::string>(const std::string &Key) const
{
  auto I = Args.find(Key);
  return {I != Args.end(), (I != Args.end()) ? I->second : std::string()};
}

} // namespace elint

#endif // __INCLUDE_ARGUMENTPACK_H__
