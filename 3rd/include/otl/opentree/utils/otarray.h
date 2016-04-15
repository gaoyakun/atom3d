/*
  Copyright (c) 2004 Pascal Kirchdorfer, Seth Yastrov

  This software is provided 'as-is', without any express or implied warranty. In
  no event will the authors be held liable for any damages arising from the use
  of this software.

  Permission is granted to anyone to use this software for any purpose, including
  commercial applications, and to alter it and redistribute it freely, subject to
  the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim
       that you wrote the original software. If you use this software in a product,
       an acknowledgment in the product documentation would be appreciated but is
       not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _OT_OTARRAY_H_
#define _OT_OTARRAY_H_

#include "otutil.h"

#ifdef USE_STL
#include <vector>
#endif

namespace opentree
{
  /// \addtogroup Utilities
  /// @{

#ifdef USE_STL
  /**
   * otArray
   * Simple array, please note that the order of elements is not given.
   * If you delete an entry, the last will replace it.
   */
  template<typename typ>
  class otArray
  {
  private:
    /// array containing the object.
    std::vector<typ> typarray;

  public:
    /** Creates a new, empty array
     *  \param capacity Initial length of the array.
     *  \param growrate Number of new items added when #capacity limit is reached.
     */
    otArray(unsigned capacity = 0, unsigned growrate = 10)
    {
      if (capacity) typarray.reserve(capacity);
    }

    otArray(otArray& other)
    : typarray(other.typarray)
    {
    }

    /** Deletes the array
     *  If using pointers as #typ then those will not be deleted.
     */
    ~otArray()
    {
    }

    /** Returns an Object
     *  If using pointers as #typ then those will not be deleted.
     *  \param index Position of the object inside the array.
     *  \return Object set by #add at the specified position.
     */
    const typ& get(unsigned int index) const
    {
      otCondAssert(index >= count, "otArray::get : out of bound access")
      return typarray[index];
    }

    /** Returns an Object
     *  If using pointers as #typ then those will not be deleted.
     *  \param index Position of the object inside the array.
     *  \return Object set by #add at the specified position.
     */
    typ& get(unsigned int index)
    {
      return typarray[index];
    }

    ///Same as #get.
    const typ& operator[](unsigned i) { return get(i); } const

    ///Same as #get.
    typ& operator[](unsigned i) { return get(i); }

    ///Returns the number of objects inside the array.
    unsigned getCount() const
    {
       return typarray.size();
    }

    /** Adds a new item to the end of the array
     *  If the end of the array has been reached, it will increase it automatically
     *  \param object item which shall be added.
     */
    void add(const typ& object)
    {
      typarray.push_back(object);
    }

    /** Adds all items to the end of the array
     *  \param objects array of items which shall be added.
     */
    void add(typ* objects, unsigned int objects_count)
    {
      // Copy the objects.
      for (unsigned int i=0; i<objects_count; i++)
      {
        typarray.push(objects[i]);
      }
    }

    /** Finds an object inside the array
     * Please not that the object needs to implement the operator==
     * Returns #count if object wasn't found.
     * \return position of the object inside the array or #count if not found
     */
    unsigned int find(const typ& object) const
    {
      for(int i=0; i<count; i++)
      {
        if (typarray[i] == object)
        {
          return i;
        }
      }
      return count;
    }

    /// Sets a new #growrate.
    void setGrowRate(unsigned int grow)
    {
      typarray.reserve(grow);
    }

    /// Returns the current #growrate
    //unsigned int getGrowRate() const
    //{
    //  return growrate;
    //}

    /** Removes an item
     *  The removed item will be replaced it with the last item in the array.
     *  Therefore the item order inside the array is not given!
     *  \param i index of the object which shall be removed.
     */
    void remove(unsigned int i)
    {
      typarray.erase(i);
    }

    /** Removes an item and deletes it
     *  The removed item will be replaced it with the last item in the array.
     *  Only use that if you have deletable types in the array such as pointers!
     *  \param i index of the object which shall be removed.
     */
    void del(unsigned int i)
    {
      delete typarray[i];
      typarray.erase(i);
    }


    /** Removes all items
     *  \param i index of the object which shall be removed.
     */
    void removeAll()
    {
      typarray.clear();
    }

    /** Removes all items and deletes it
     *  Only use that if you have deletable types in the array such as pointers!
     *  \param i index of the object which shall be removed.
     */
    void delAll()
    {
      for (unsigned int i = 0; i<typarray.size(); i++)
      {
        delete typarray[i];
      }
      typarray.clear();
    }
/*
    const typ* getRawArray() const
    {
      return typarray;
    }
*/
  };

#else

  /**
   * otArray
   * Simple array, please note that the order of elements is not given.
   * If you delete an entry, the last will replace it.
   */
  template<typename typ>
  class otArray
  {
  private:
    /// Pointer to the array containing the object.
    typ* typarray;

    /// Maximum object the array can store.
    unsigned capacity;

    /// Number of currently stored object.
    unsigned count;

    /// Number of new objects which will be added by #increase().
    unsigned growrate;

    /** Increase size of array
     *  The Array will be extended by #growrate. Should the allocation of more memory fail,
     *  so it will return the small array. Error tolerance.
     */
    void increase()
    {
      increase(growrate);
    }

    /** Increase size of array
     *  The Array will be extended by #grow. Should the allocation of more memory fail,
     *  so it will return the small array. Error tolerance.
     */
    void increase(unsigned int grow)
    {
      typ* newtyparray = new (std::nothrow) typ[capacity+grow];
      if (!typarray)
      {
        typarray = newtyparray;
        capacity += grow;
      }
      else if(newtyparray)
      {
        for(unsigned i=0; i != capacity; i++)
        {
          newtyparray[i] = typarray[i];
        }
        delete [] typarray;
        typarray = newtyparray;
        capacity += grow;
      }
      else
      {
        std::cerr << "otArray expansion from: " << capacity << " to " 
          << capacity+grow << "failed\n";
      }
    }

  public:
    /** Creates a new, empty array
     *  \param capacity Initial length of the array.
     *  \param growrate Number of new items added when #capacity limit is reached.
     */
    otArray(unsigned capacity = 0, unsigned growrate = 10)
    : typarray(0), capacity(capacity), count(0), growrate(growrate)
    {
      if (capacity)
      {
        typarray = new typ[capacity];
      }
    }

    /** Deletes the array
     *  If using pointers as #typ then those will not be deleted.
     */
    ~otArray()
    {
      delete [] typarray;
    }

    /** Returns an Object
     *  If using pointers as #typ then those will not be deleted.
     *  \param index Position of the object inside the array.
     *  \return Object set by #add at the specified position.
     */
    inline const typ& get(unsigned int index) const
    {
      otCondAssert(index >= count, "otArray::get : out of bound access")
      return typarray[index];
    }

    /** Returns an Object
     *  If using pointers as #typ then those will not be deleted.
     *  \param index Position of the object inside the array.
     *  \return Object set by #add at the specified position.
     */
    inline typ& get(unsigned int index)
    {
      otCondAssert(index >= count, "otArray::get : out of bound access")
      return typarray[index];
    }

    ///Same as #get.
    inline const typ& operator[](unsigned i) const
    { return get(i); }

    ///Same as #get.
    inline typ& operator[](unsigned i)
    { return get(i); }

    ///Returns the number of objects inside the array.
    unsigned getCount() const
    {
       return count;
    }

    /** Adds a new item to the end of the array
     *  If the end of the array has been reached, it will increase it automatically
     *  \param object item which shall be added.
     */
    void add(const typ& object)
    {
      if (count >= capacity)
      {
        increase();
        if (count >= capacity) //in case increase() failed... error tolerance.
        {
          otCondAssert(count >= capacity, "otArray::add : failed to add object")
          return;
        }
      }
      typarray[count] = object;
      count++;
    }

    /** Adds all items to the end of the array
     *  \param object array of items which shall be added.
     */
    void add(otArray<typ>& object)
    {
      // Increase the array if needed
      if (capacity-count < object.count)
      {
        increase(object.count - (capacity-count));
      }
      // Copy the objects from the other array
      for (unsigned int i=0; i<object.count; i++)
      {
        typarray[count+i] = object.typarray[i];
      }
      // Adjust count variable
      count += object.count;
    }

    /** Adds all items to the end of the array
     *  \param objects array of items which shall be added.
     *  \param objects_count number of objects in #objects
     */
    void add(typ* objects, unsigned int objects_count)
    {
      // Increase the array if needed
      if (capacity-count < objects_count)
      {
        increase(objects_count - (capacity-count));
      }
      // Copy the objects.
      for (unsigned int i=0; i<objects_count; i++)
      {
        typarray[count+i] = objects[i];
      }
      // Adjust count variable
      count += objects_count;
    }

    /** Finds an object inside the array
     * Please not that the object needs to implement the operator==
     * Returns #count if object wasn't found.
     * \return position of the object inside the array or #count if not found
     */
    unsigned int find(const typ& object) const
    {
      for(int i=0; i<count; i++)
      {
        if (typarray[i] == object)
        {
          return i;
        }
      }
      return count;
    }

    /// Sets a new #growrate.
    void setGrowRate(unsigned int grow)
    {
      growrate = grow;
    }

    /// Returns the current #growrate
    unsigned int getGrowRate() const
    {
      return growrate;
    }

    /** Removes an item
     *  The removed item will be replaced it with the last item in the array.
     *  Therefore the item order inside the array is not given!
     *  \param i index of the object which shall be removed.
     */
    void remove(unsigned int i)
    {
      otCondAssert(i >= count, "otArray::get : out of bound access")
      count--;
      typarray[i] = typarray[count];
    }

    /** Removes an item and deletes it
     *  The removed item will be replaced it with the last item in the array.
     *  Only use that if you have deletable types in the array such as pointers!
     *  \param i index of the object which shall be removed.
     */
    void del(unsigned int i)
    {
      otCondAssert(i >= count, "otArray::get : out of bound access")
      count--;
      delete typarray[i];
      typarray[i] = typarray[count];
    }


    /** Removes all items
     */
    void removeAll()
    {
      count=0;
    }

    /** Removes all items and deletes it
     *  Only use that if you have deletable types in the array such as pointers!
     */
    void delAll()
    {
      for (unsigned int i = 0; i< count; i++)
      {
        delete typarray[i];
      }
      count = 0;
    }

    const typ* getRawArray() const
    {
      return typarray;
    }

    typ* getRawArray()
    {
      return typarray;
    }

  };
#endif
  /// @}
}

#endif //_OT_OTARRAY_H_
