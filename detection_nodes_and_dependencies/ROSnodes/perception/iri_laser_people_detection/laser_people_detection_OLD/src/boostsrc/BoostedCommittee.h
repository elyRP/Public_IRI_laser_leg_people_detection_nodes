////////////////////////////////////////////////////////////////////
// BoostedCommittee.h: interface for the CBoostedCommittee class.

#if !defined(AFX_BOOSTEDCOMMITTEE_H__BAEE4FC8_5AEF_4B42_854C_845B8CF397B8__INCLUDED_)
#define AFX_BOOSTEDCOMMITTEE_H__BAEE4FC8_5AEF_4B42_854C_845B8CF397B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <stdio.h>
#include "SPHypothesis.h"

/*!\class CBoostedCommittee
  \brief Boosting classifier class, loads model from file and classifies samples.
*/
class CBoostedCommittee
{
public:
  CBoostedCommittee(); //!< constructor
  virtual ~CBoostedCommittee(); //!< destructor

  /**
  * \brief Classifies samples
  * \return classifier value
  */
  double Predict(double * in_Sample);

  /**
  * \brief Loads classifier from file
  * \param in_File classifier file
  * \return true
  */
  bool LoadFromFile(FILE* in_File);

  /**
  * \brief Loads classifier from data
  * \param Data classifier data
  * \return true
  */
  bool LoadFromString(const char* Data);

  /**
  * \brief Gives max accumulated weight
  * \return max weight possible
  */
  double getMaxWeight();

  /**
  * \brief Gives min accumulated weight
  * \return min weight possible
  */
  double getMinWeight();

protected:
  std::vector <CSPHypothesis> m_vHypotheses; //!< vector of weak classifiers
  std::vector <double> m_vWeights;           //!< vector of weights
  double maxWeight;                          //!< max weight possible
  double minWeight;                          //!< min weight possible
};
#endif
////////////////////////////////////////////////////////////////////