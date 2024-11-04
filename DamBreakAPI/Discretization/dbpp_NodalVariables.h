
#pragma once

// C++ include
#include <iostream>
// STL includes
#include <list>
#include <tuple>
#include <vector>
// boost includes
#include <boost/assert.hpp>
#include <boost/operators.hpp>
// Library include
#include "SfxTypes/dbpp_DefineTypes.h"

/** @brief Operator to print Nodal_Value (tuple representation)*/
template <typename... Ts>
std::ostream &operator<<(std::ostream &os, std::tuple<Ts...> const &theTuple) {
  std::apply(
      [&os](Ts const &...tupleArgs) {
        os << '[';
        std::size_t n{0};
        ((os << tupleArgs << (++n != sizeof...(Ts) ? ", " : "")), ...);
        os << ']';
      },
      theTuple);
  return os;
}

namespace dbpp {
/** Brief Nodal type that hold variable value.
 *
 * Design Note
 *  Unit test!! it's a must, just do some basic one to check
 *  basic functionality.
 *
 * Concept
 * -------
 *   Nodal_Value type is a model of node variable, in a simulation
 *   there a set of variables associated with mesh node, and Nodal_Value
 *   represent those node variables.
 */
class Nodal_Value {
public:
  /**
   *  Aliases to make code cleaner
   */
  using nodeIdX =
      std::pair<int32, float64>; /**< represent node Id and coordinate*/
  using tuplevar = std::tuple<float64 /*A*/, float64 /*Q*/,
                              float64 /*H*/>; /**< represent (A,Q,H)*/
public:
  /** @brief default ctor*/
  Nodal_Value();
  /** ctor
   *  @param pair of x-coord and node index
   *  @param degree of liberty of the node
   *  @param flag to node status (boundary condition)
   */
  Nodal_Value(nodeIdX aNodeXId, int aNumOfAttr, int aTieNode = -1);
  /** default ctor (i am not sure about this one!!).
   *  @param pair of x-coordinate  and node index
   *  @param degree of liberty of the node
   *  @param struct that hold nodal values
   *  @param flag to node status (boundary condition)
   */
  Nodal_Value(nodeIdX aNodeXId, int aNumAttr, tuplevar aTuple,
              int aTiedNode = -1);

  //		Nodal_Value( unsigned aId, int aNumAttr, int aTiedNode=-1);
  // Design Note: really need it??

  /** copy initialization*/
  // Nodal_Value(const Nodal_Value &aOther);

  /** assignment ctor*/
  // Nodal_Value &operator=(const Nodal_Value &aOther);

  /** force destructor to be virtual*/
  // virtual ~Nodal_Value() = default;

  /** node parameters
   * @result node number
   */
  nodeIdX &getIdX() { return m_nodeIdX; }
  nodeIdX getIdX() const { return m_nodeIdX; }
  /**
   * @result node number
   */
  int32 &node_no() { return m_nodeIdX.first; }
  int32 node_no() const { return m_nodeIdX.first; }
  /**
   *  @result tie node number
   */
  int32 &tie_node_no() { return m_TieNode_No; }
  int32 tie_node_no() const { return m_TieNode_No; }
  /**
   * @result number of degree of freedom
   */
  int32 &no_of_dim() { return m_NumOfAttr; }
  int32 no_of_dim() const { return m_NumOfAttr; }

  /** check if we have a tied node
   * @result
   */
  bool isTiedNode() const { return m_TieNode_No == -1; }

  /** check if we have a ghost node
   * @result
   */
  bool isGhostNode() const { return m_ghostNode; }

  /** @Brief Set ghost node.
   *   Ghost node is not part of the computational domain
   *   it is added to the global discretization for algorithm
   *   computation purpose.
   * @result
   */
  void setAsGhostNode() {
    if (m_ghostNode == true) {
      return;
    }
    m_ghostNode = true;
  }

  /**
   *  @result struct that contains nodal values
   */
  tuplevar Values() const { return m_Node3Data; }

  /** element accessor (write access)
   * @result nodal value from given index
   */
  float64 &operator[](int i) {
    //			BOOST_ASSERT(i>=0&&i<3);

    if (m_NumOfAttr == 3) {
      switch (i) {
      case 0: // A
        return std::get<0>(m_Node3Data);
        break;
      case 1: // Q
        return std::get<1>(m_Node3Data);
        break;
      case 2: // H
        return std::get<2>(m_Node3Data);
        break;
      default: // return the first element as default
        return std::get<0>(m_Node3Data);
        break;
      }
    } else {
      return m_nodeIdX.second; // x-coordinate
    }
  }

  /** element accessor (write access)
   * @result nodal value from given index
   */
  const float64 &operator[](int i) const {
    //			BOOST_ASSERT(i>=0&&i<3);

    if (m_NumOfAttr == 3) {
      switch (i) {
      case 0: // A
        return std::get<0>(m_Node3Data);
        break;
      case 1: // Q
        return std::get<1>(m_Node3Data);
        break;
      case 2: // H
        return std::get<2>(m_Node3Data);
        break;
      default: // return the first element as default
        return std::get<0>(m_Node3Data);
        break;
      }
    } else {
      return m_nodeIdX.second;
    }
  }
  // i don't get it??
  void Values(const tuplevar aTuple) { m_Node3Data = aTuple; }

  /** Display node's information
   * (used with for_each and mem_fun_ref)
   */
  void printNodeInfo() {
    // formating the output operator
    std::cout.setf(std::ios::fixed);
    std::cout.precision(4);
    std::cout.width(4);

    if (m_NumOfAttr == 3) {
      std::cout << "Id " << m_nodeIdX.first << '\t'
                << "A: " << std::get<0>(m_Node3Data) << '\t'
                << "Q: " << std::get<1>(m_Node3Data) << '\t'
                << "H: " << std::get<2>(m_Node3Data) << '\t'
                << "X: " << m_nodeIdX.second << "\n";
    } else {
      std::cout << "Id " << m_nodeIdX.first << '\t' << "X: " << m_nodeIdX.second
                << "\n";
    }
  }
  // 		std::string toString() const
  // 		{ return std::string("");}

  /**
   *  Print node information
   */
  void print() {
    if (m_NumOfAttr == 3) {
      std::cout << "Id " << m_nodeIdX.first << '\t' << "X: " << m_nodeIdX.second
                << '\t' << "A: " << std::get<0>(m_Node3Data) << '\t'
                << "Q: " << std::get<1>(m_Node3Data) << '\t'
                << "H: " << std::get<2>(m_Node3Data) << "\n";
    } else {
      std::cout << "Id " << m_nodeIdX.first << '\t' << "X: " << m_nodeIdX.second
                << "\n";
    }
  }

  /** overload stream operator
   *
   */
  friend std::ostream &operator<<(std::ostream &, Nodal_Value &);

protected:
  //	int32 m_NodeNumber;        node number
  int32 m_NumOfAttr;    /**< number of attributes*/
  int32 m_TieNode_No;   /**< degree of liberty that is fix*/
  nodeIdX m_nodeIdX;    /**< identifier of the node (node number and coord.)*/
  tuplevar m_Node3Data; /**< hold state variable value (A,Q,H)*/
private:
  bool m_ghostNode;
};

/** Brief A nodal type that hold coordinate of the node.
 *  By inheriting from boost equality_comparable base class
 *  we make sure that our type can be used with STL container
 *  and satisfy equivalence criteria (see Scott Meyers "Effective STL")
 *
 *  NOTE this concept is not complete
 */
class GNode
    : public Nodal_Value,
      public boost::equality_comparable<GNode> // not sure about this one!!
{
public:
  /**
   *
   */
  GNode(int32 aId, float64 aXcoord, int32 aTiedNode = -1, unsigned aNumAttr = 1)
      : Nodal_Value(std::make_pair(aId, aXcoord), aNumAttr, aTiedNode) {
    // nothing else to do
  }
  /** for now we check the x coordinate equality
   */
  bool operator==(const GNode &aOther) {
    return getIdX().second == aOther.getIdX().second;
  }
  bool operator!=(const GNode &aOther) {
    return !(operator==(aOther));
  } // really need it since we inherit from boost equality_comparable?
};
} // namespace dbpp
