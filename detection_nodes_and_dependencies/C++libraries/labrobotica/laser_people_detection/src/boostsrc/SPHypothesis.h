////////////////////////////////////////////////////////////////////
// SPHypothesis.h: interface for the CSPHypothesis class.

#if !defined(AFX_SPHYPOTHESIS_H__ECE6F8CB_75B0_4E7D_A228_5733F90B4B1B__INCLUDED_)
#define AFX_SPHYPOTHESIS_H__ECE6F8CB_75B0_4E7D_A228_5733F90B4B1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IWeakHypothesis.h"
#include <vector>
#include <stdio.h>

/*!\class CSPHypothesis
  \brief Weak classifier class implementation, classifies samples
*/
class CSPHypothesis : public IWeakHypothesis
{
public:
  CSPHypothesis();          //!< constructor
  virtual ~CSPHypothesis(); //!< destructor

  /**
  * \brief Classify sample
  * \param in_Sample sample to classify
  * \return classifier value
  */
  double Predict(double * in_Sample);

  /**
  * \brief Classify vector of samples
  * \param in_vSamples vector of samples to classify
  * \param in_iTotalSamples number of samples to classify
  * \param out_vPredictions vector of classified values
  */
  void   PredictVector(double **in_vSamples, int in_iTotalSamples, double *out_vPredictions);

  /**
  * \brief Loads classifier from file
  * \param in_File classifier file
  * \return true
  */
  bool   LoadFromFile(FILE* in_File);

  /**
  * \brief Loads classifier from data
  * \param Data classifier data
  * \return true
  */
  bool   LoadFromString(const char* Data);

protected:
  std::vector <int> m_vDims;          //!< vector of dimensions (features)
  std::vector <double> m_vThresholds; //!< vector of thresholds (weak classifier thresholds)
  std::vector <double> m_vSignums;    //!< vector of signums (weak classifier direction)
};
#endif
////////////////////////////////////////////////////////////////////
