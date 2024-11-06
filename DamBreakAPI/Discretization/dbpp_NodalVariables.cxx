
// stl include
#include <algorithm>
// library include
#include "dbpp_NodalVariables.h"

namespace dbpp {
//
// Design Note (July 2015)
//   These methods need a serious clean-up, it's a big mess.
//   Specially the attribute seems to be mixed up, i am not
//   but a closer look will help!!

// default ctor (these are default values and have no meaning)
Nodal_Value::Nodal_Value()
    : m_NumOfAttr(3),
      //	  m_NodeNumber(   0 ),
      m_TieNode_No(0), m_isTiedNode{false}, m_ghostNode(false) {
  m_nodeIdX.first = 0;           // node id
  m_nodeIdX.second = 0.;         // node x-coord.
  std::get<0>(m_Node3Data) = 0.; // A
  std::get<1>(m_Node3Data) = 0.; // Q
  std::get<2>(m_Node3Data) = 0.; // H
}
Nodal_Value::Nodal_Value(nodeIdX aNodeId, int32 aNumOfAttr, int32 aTiedNode,
                         bool aIsTieNode /*=false*/)
    : //	  m_NodeNumber(   aNodeId.first),
      m_NumOfAttr(aNumOfAttr), m_TieNode_No(aTiedNode),
      m_nodeIdX(aNodeId), m_isTiedNode{aIsTieNode}, m_ghostNode(false) {
  std::get<0>(m_Node3Data) = 0.; // A
  std::get<1>(m_Node3Data) = 0.; // Q
  std::get<2>(m_Node3Data) = 0.; // H
}

Nodal_Value::Nodal_Value(nodeIdX aNodeXId, int aNumAttr, tuplevar aTuple,
                         int32 aTiedNode /*= 0*/, bool aIsTiedNode /*= false*/)
    : m_NumOfAttr(aNumAttr), m_TieNode_No(aTiedNode), m_nodeIdX(aNodeXId),
      m_Node3Data(aTuple), m_isTiedNode{aIsTiedNode}, m_ghostNode(false) {
  // not much to do for now
}

#if 0
// copy ctor
Nodal_Value::Nodal_Value(const Nodal_Value &aOther) {
  m_nodeIdX = aOther.m_nodeIdX;
  //		m_NodeNumber = aOther.m_NodeNumber;
  m_NumOfAttr = aOther.m_NumOfAttr;
  m_TieNode_No = aOther.m_TieNode_No;
  m_Node3Data = aOther.m_Node3Data; // tuple version
  m_ghostNode = aOther.m_ghostNode;
}

// assignment ctor
Nodal_Value &Nodal_Value::operator=(const Nodal_Value &aOther) {
  // check for self assignment
  if (&aOther != this) {
    m_nodeIdX = aOther.m_nodeIdX;
    //			m_NodeNumber = aOther.m_NodeNumber;
    m_NumOfAttr = aOther.m_NumOfAttr;
    m_TieNode_No = aOther.m_TieNode_No;
    //		m_Node2Data=aOther.m_Node2Data;  // deprecated
    m_Node3Data = aOther.m_Node3Data;
    m_ghostNode = aOther.m_ghostNode;
  }

  return *this;
}
#endif

// ___________________________________________________
//
//  friend function
std::ostream &operator<<(std::ostream &s, Nodal_Value &nv) {
  s << "{";
  // output tie node number to indicate been tied
  if (nv.tie_node_no() == -1)
    s << nv.node_no() << " | (";
  else
    s << nv.node_no() << "-" << (nv.tie_node_no()) << " | {";
  // printing nodal attributes
  //  int checkNumofdim = nv.no_of_dim();
  for (int i = 0; i < nv.no_of_dim(); i++) {
    if (i != nv.no_of_dim() - 1)
      s << (nv[i]) << ", ";
    else
      s << (nv[i]) << ") } ";
  }
  // 		for( int32 i = 0; i < nv.no_of_dim(); i++)
  // 		{
  // 			if(i != nv.no_of_dim()-1) s << ( *nv.Values()) << ", ";
  // 			else s << ( *nv.Values()) << ") } ";
  // 			++nv.Values();
  // 		}
  //
  return s;
}
} // namespace dbpp
