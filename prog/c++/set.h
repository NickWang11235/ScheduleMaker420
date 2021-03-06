#ifndef _SET_H_
#define _SET_H_

#include <vector>
#include <algorithm>
#include "interval.h"

/**
 * A collection of intervals
 */
template <class T>
class po_set{
private:
  std::vector<interval<T>> _intervals;
public:

  po_set(){
    std::vector<interval<T>> v;
    _intervals = v;
  }

  po_set(const interval<T>& intv){
    _intervals.push_back(intv);
  }

  po_set(const std::vector<interval<T>>& intv){
    _intervals = intv;
  }

  /**
   * Constructs a poset with an array
   *
   * @param arr
   * @param size
   * @return
   */
  po_set(interval<T>* arr, int size){
    std::vector<interval<T>> v;
    for(int i = 0; i < size; i++)
      v.push_back(arr[i]);
    _intervals = v;
    clean();
  }

  bool is_empty() const{
    if(_intervals.empty())
      return true;
    for(int i = 0; i < _intervals.size(); i++)
      if(!_intervals.at(i).is_empty())
        return false;
    return true;
  }

  void clean(){
    order();
    set_family_union();
  }

  bool contains(const T& elem) const{
    for(int i = 0; i < _intervals.size(); i++)
      if(_intervals.at(i).contains(elem))
        return true;
    return false;
  }

  /**
   * Order the elements in the collection
   */
  void order(){
    std::sort(_intervals.begin(), _intervals.end(),
             [](const interval<T> a, const interval<T> b) -> bool{ return a.lower < b.lower; });
  }

  /**
   * Performs a union over the collection of sets
   */
  void set_family_union(){
    int i = 0;
    while(i < _intervals.size()-1){
      if(_intervals.at(i).is_empty()){
        _intervals.erase(_intervals.begin()+i);
        continue;
      }
      std::vector<interval<T>> v = _intervals.at(i).intv_union(_intervals.at(i+1));
      if(v.size() == 2){
        i++;
        continue;
      }
      _intervals.at(i) = v.at(0);
      _intervals.erase(_intervals.begin()+i+1);
    }
  }

  /**
   * Finds the intersect between 2 posets
   *
   * @param s
   * @return
   */
  po_set set_intersect(const po_set<T>& s) const{
    std::vector<interval<T>> new_intervals;
    for(int i = 0; i < _intervals.size(); i++)
      for(int j = 0; j < s._intervals.size(); j++){
          new_intervals.push_back(_intervals.at(i).intv_intersect(s._intervals.at(j)));
        }
    po_set p(new_intervals);
    p.clean();
    return p;
  }

  static void push_back_all(std::vector<interval<T>>& v1, const std::vector<interval<T>>& v2){
    for(int i = 0; i < v2.size(); i++)
      v1.push_back(v2.at(i));
  }
  /**
   * Finds the union between 2 posets
   *
   * @param s
   * @return
   */
  po_set set_union(const po_set<T>& s) const{
    if(is_empty())
      return s;
    std::vector<interval<T>> new_intervals, temp;
    for(int i = 0; i < _intervals.size(); i++)
      for(int j = 0; j < s._intervals.size(); j++)
        push_back_all(new_intervals, _intervals.at(i).intv_union(s._intervals.at(j)));
    po_set p(new_intervals);
    p.clean();
    return p;
  }

  /**
   * Finds the difference of this - s
   *
   * @param s
   * @return
   */
  po_set set_difference(const po_set<T>& s) const{
    po_set<T> u;
    std::vector<interval<T>> temp;
    for(int i = 0; i < _intervals.size(); i++){
      po_set<T> p;
      for(int j = 0; j < s._intervals.size(); j++){
        temp = _intervals.at(i).intv_difference(s._intervals.at(j));
        if(p.is_empty()){
          p = po_set<T>(temp);
          continue;
        }
        p = p.set_intersect(po_set<T>(temp));
      }
      u = u.set_union(p);
    }
    u.clean();
    return u;
  }

  /**
   * Finds the complement of this given universe u
   *
   * @param u
   * @return
   */
  po_set set_complement(const po_set<T>& u) const{
    return u.set_difference(*this);
  }

  /**
   * Returns a string representation of the poset
   *
   * @return
   */
  std::string to_string() const{
    std::string str = "";
    for(int i = 0; i < _intervals.size(); i++){
      str += _intervals.at(i).to_string();
      if(i < _intervals.size() - 1)
        str += "U";
    }
    return str;
  }
};

#endif
