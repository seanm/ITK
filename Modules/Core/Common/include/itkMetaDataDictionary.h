/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkMetaDataDictionary_h
#define itkMetaDataDictionary_h

#include "itkMetaDataObjectBase.h"
#include <algorithm> // For std::equal
#include <vector>
#include <map>
#include <string>
#include <memory>

namespace itk
{
/** \class MetaDataDictionary
 *  \brief Provides a mechanism for storing a collection of arbitrary
 *         data types
 *
 * \author Hans J. Johnson
 *
 * The MetaDataDictionary, along with the MetaDataObject derived template
 * classes, is designed to provide a mechanism for storing a collection of
 * arbitrary data types. The main motivation for such a collection is to
 * associate arbitrary data elements with itk DataObjects.
 *
 * The MetaDataDictionary implements shallow copying with copy on
 * write behavior. When a copy of this class is created, the new copy
 * will be shared with the old copy via C++11 shared pointers. When a
 * non-constant operation is done, if the dictionary is not unique to
 * this object, then a deep copy is performed. This make is very cheap
 * to create multiple copies of the same dictionary if they are never
 * modified.
 *
 * \ingroup ITKCommon
 * \sphinx
 * \sphinxexample{Core/Common/StoreNonPixelDataInImage,Store Non-Pixel Data In Image}
 * \endsphinx
 */
class ITKCommon_EXPORT MetaDataDictionary
{
public:
  using Self = MetaDataDictionary;
  /**
   * Defines the default behavior for printing out this element
   * \param os An output stream
   */
  virtual void
  Print(std::ostream & os) const;

  // Declare the datastructure that will be used to hold the
  // dictionary.
  using MetaDataDictionaryMapType = std::map<std::string, MetaDataObjectBase::Pointer>;
  using Iterator = MetaDataDictionaryMapType::iterator;
  using ConstIterator = MetaDataDictionaryMapType::const_iterator;

  // Constructor
  MetaDataDictionary();
  // Copy Constructor
  MetaDataDictionary(const MetaDataDictionary &);
  MetaDataDictionary(MetaDataDictionary &&) = default;
  // operator =
  MetaDataDictionary &
  operator=(const MetaDataDictionary &);
  MetaDataDictionary &
  operator=(MetaDataDictionary &&) = default;

  // Destructor
  virtual ~MetaDataDictionary();

  /** Returns (metaDataDictionary1 == metaDataDictionary2). */
  friend bool
  operator==(const Self & lhs, const Self & rhs)
  {
    using KeyValuePair = MetaDataDictionaryMapType::value_type;

    return (lhs.m_Dictionary == rhs.m_Dictionary) ||
           ((lhs.m_Dictionary != nullptr) && (rhs.m_Dictionary != nullptr) &&
            (lhs.m_Dictionary->size() == rhs.m_Dictionary->size()) &&
            std::equal(lhs.m_Dictionary->cbegin(),
                       lhs.m_Dictionary->cend(),
                       rhs.m_Dictionary->cbegin(),
                       [](const KeyValuePair & keyValuePair1, const KeyValuePair & keyValuePair2) {
                         const auto & value1 = keyValuePair1.second;
                         const auto & value2 = keyValuePair2.second;
                         return (keyValuePair1.first == keyValuePair2.first) &&
                                ((value1 == value2) ||
                                 ((value1 != nullptr) && (value2 != nullptr) && (*value1 == *value2)));
                       }));
  }

  /** Returns (metaDataDictionary1 != metaDataDictionary2). */
  friend bool
  operator!=(const Self & lhs, const Self & rhs)
  {
    return !(lhs == rhs);
  }


  /** Returns a vector of keys to the key/value entries in the
   * dictionary.  Iterate through the dictionary using these keys.
   */
  [[nodiscard]] std::vector<std::string>
  GetKeys() const;

  // Implement map's api. On some Microsoft compilers, stl containers
  // cannot be exported. This causes problems when building DLL's.
  // Here we inherit privately from std::map and provide a simple
  // API. The implementation will be in the DLL.
  MetaDataObjectBase::Pointer &
  operator[](const std::string &);

  // \brief Get a constant point to a DataObject
  //
  // If the key does not exist then nullptr is returned.
  const MetaDataObjectBase *
  operator[](const std::string &) const;

  [[nodiscard]] const MetaDataObjectBase *
  Get(const std::string &) const;
  void
  Set(const std::string &, MetaDataObjectBase *);
  [[nodiscard]] bool
  HasKey(const std::string &) const;

  bool
  Erase(const std::string &);

  /** \warning the following functions SHOULD NOT be used with
   * the visual studio 6 compiler since iterator outside of the dll
   * context cannot be dereferenced safely */

  /** Returns an iterator to the beginning of the map */
  /** @ITKStartGrouping */
  // Blacklisted by igenerator.py
  Iterator
  Begin();
  // Blacklisted by igenerator.py
  [[nodiscard]] ConstIterator
  Begin() const;
  /** @ITKEndGrouping */
  /** Returns an iterator to the end of the map */
  /** @ITKStartGrouping */
  // Blacklisted by igenerator.py
  Iterator
  End();
  // Blacklisted by igenerator.py
  [[nodiscard]] ConstIterator
  End() const;
  /** @ITKEndGrouping */
  /** Returns an iterator matching the string key */
  Iterator
  Find(const std::string & key);

  [[nodiscard]] ConstIterator
  Find(const std::string & key) const;

  /** remove all MetaObjects from dictionary */
  void
  Clear();

  void
  Swap(MetaDataDictionary & other);

private:
  bool
  MakeUnique();

  std::shared_ptr<MetaDataDictionaryMapType> m_Dictionary{};
};

inline void
swap(MetaDataDictionary & a, MetaDataDictionary & b) noexcept
{
  a.Swap(b);
}

} // namespace itk
#endif // itkMetaDataDictionary_h
