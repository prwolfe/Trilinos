/*@HEADER
// ***********************************************************************
//
//       Ifpack: Object-Oriented Algebraic Preconditioner Package
//                 Copyright (2002) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ***********************************************************************
//@HEADER
*/

#ifndef IFPACK_ICT_H
#define IFPACK_ICT_H

#include "Ifpack_ConfigDefs.h"
#include "Ifpack_CondestType.h"
#include "Ifpack_ScalingType.h"
#include "Ifpack_Preconditioner.h"
#include "Epetra_Vector.h"
#include "Epetra_CrsMatrix.h"
#include "Epetra_Time.h"
#include "Teuchos_RefCountPtr.hpp"

class Epetra_RowMatrix;
class Epetra_SerialComm;
class Epetra_Comm;
class Epetra_Map;
class Epetra_MultiVector;

namespace Teuchos {
  class ParameterList;
}

//! Ifpack_ICT: A class for constructing and using an incomplete Cholesky factorization of a given Epetra_RowMatrix.

/*! The Ifpack_ICT class computes a threshold based incomplete 
 LDL^T factorization of a given Epetra_RowMatrix.  The factorization 
 that is produced is a function of several parameters:
<ol>
  <li> Maximum number of entries per row/column in factor - The factorization will contain at most this number of nonzero
       terms in each row/column of the factorization.

  <li> Diagonal perturbation - Prior to computing the factorization, it is possible to modify the diagonal entries of the matrix
       for which the factorization will be computing.  If the absolute and relative perturbation values are zero and one,
       respectively, the
       factorization will be compute for the original user matrix A.  Otherwise, the factorization
       will computed for a matrix that differs from the original user matrix in the diagonal values only. Details can be found in \ref ifp_diag_pert.
</ol>

*/    

class Ifpack_ICT: public Ifpack_Preconditioner {
      
 public:
  //! Ifpack_ICT constuctor with variable number of indices per row.
  /*! Creates a Ifpack_ICT object and allocates storage.  
    
    \param In 
           A - User matrix to be factored.
    \param In
           Graph - Graph generated by Ifpack_IlukGraph.
  */
  Ifpack_ICT(const Epetra_RowMatrix* A);
  
  //! Ifpack_ICT Destructor
  virtual ~Ifpack_ICT();

  //! Set parameters using a Teuchos::ParameterList object.
  /* This method is only available if the Teuchos package is enabled.
     This method recognizes five parameter names: level_fill, drop_tolerance,
     absolute_threshold, relative_threshold and overlap_mode. These names are
     case insensitive. For level_fill the ParameterEntry must have type int, the 
     threshold entries must have type double and overlap_mode must have type
     Epetra_CombineMode.
  */
  int SetParameters(Teuchos::ParameterList& parameterlis);

  //! Returns a reference to the matrix to be preconditioned.
  const Epetra_RowMatrix& Matrix() const
  {
    return(A_);
  }

  //! Returns \c true is the preconditioner has been successfully initialized.
  bool IsInitialized() const
  {
    return(IsInitialized_);
  }

  //! Initialize L and U with values from user matrix A.
  /*! Copies values from the user's matrix into the nonzero pattern of L and U.
    \param In 
           A - User matrix to be factored.
    \warning The graph of A must be identical to the graph passed in to Ifpack_IlukGraph constructor.
             
   */
  int Initialize();

  //! Compute IC factor U using the specified graph, diagonal perturbation thresholds and relaxation parameters.
  /*! This function computes the RILU(k) factors L and U using the current:
    <ol>
    <li> Ifpack_IlukGraph specifying the structure of L and U.
    <li> Value for the RILU(k) relaxation parameter.
    <li> Value for the \e a \e priori diagonal threshold values.
    </ol>
    InitValues() must be called before the factorization can proceed.
   */
  int Compute();

  //! If factor is completed, this query returns true, otherwise it returns false.
  bool IsComputed() const {return(IsComputed_);};

  // Mathematical functions.
  
  //! Returns the result of a Ifpack_ICT forward/back solve on a Epetra_MultiVector X in Y.
  /*! 
    \param In
    Trans -If true, solve transpose problem.
    \param In
    X - A Epetra_MultiVector of dimension NumVectors to solve for.
    \param Out
    Y -A Epetra_MultiVector of dimension NumVectorscontaining result.
    
    \return Integer error code, set to 0 if successful.
  */
  int ApplyInverse(const Epetra_MultiVector& X, Epetra_MultiVector& Y) const;

  int Apply(const Epetra_MultiVector& X, Epetra_MultiVector& Y) const;

  //! Returns the maximum over all the condition number estimate for each local ILU set of factors.
  /*! This functions computes a local condition number estimate on each processor and return the
      maximum over all processor of the estimate.
   \param In
    Trans -If true, solve transpose problem.
    \param Out
    ConditionNumberEstimate - The maximum across all processors of 
    the infinity-norm estimate of the condition number of the inverse of LDU.
  */
  double Condest(const Ifpack_CondestType CT = Ifpack_Cheap, 
                 const int MaxIters = 1550,
                 const double Tol = 1e-9,
		 Epetra_RowMatrix* Matrix_in = 0);

  double Condest() const
  {
    return(Condest_);
  }

  // Attribute access functions
    
  //! Returns the number of nonzero entries in the global graph.
  int NumGlobalNonzeros() const {return(H().NumGlobalNonzeros());};
 
  //! Returns the number of nonzero entries in the local graph.
  int NumMyNonzeros() const {return(H().NumMyNonzeros());};

  //! Returns the address of the D factor associated with this factored matrix.
  const Epetra_CrsMatrix& H() const {return(*H_);};
    
  //@{ \name Additional methods required to support the Epetra_Operator interface.

    //! If set true, transpose of this operator will be applied.
    /*! This flag allows the transpose of the given operator to be used implicitly.  Setting this flag
        affects only the Apply() and ApplyInverse() methods.  If the implementation of this interface 
	does not support transpose use, this method should return a value of -1.
      
    \param In
	   UseTranspose_in -If true, multiply by the transpose of operator, otherwise just use operator.

    \return Always returns 0.
  */
  int SetUseTranspose(bool UseTranspose_in) {UseTranspose_ = UseTranspose_in; return(0);};

  //! Returns 0.0 because this class cannot compute Inf-norm.
  double NormInf() const {return(0.0);};

  //! Returns false because this class cannot compute an Inf-norm.
  bool HasNormInf() const {return(false);};

  //! Returns the current UseTranspose setting.
  bool UseTranspose() const {return(UseTranspose_);};

  //! Returns the Epetra_Map object associated with the domain of this operator.
  const Epetra_Map& OperatorDomainMap() const {return(A_.OperatorDomainMap());};

  //! Returns the Epetra_Map object associated with the range of this operator.
  const Epetra_Map& OperatorRangeMap() const{return(A_.OperatorRangeMap());};

  //! Returns the Epetra_BlockMap object associated with the range of this matrix operator.
  const Epetra_Comm& Comm() const{return(Comm_);};
  //@}

  const char* Label() const
  {
    return(Label_.c_str());
  }

  int SetLabel(const char* Label_in)
  {
    Label_ = Label_in;
    return(0);
  }
 
  //! Prints basic information on iostream. This function is used by operator<<.
  virtual ostream& Print(std::ostream& os) const;

  //! Returns the number of calls to Initialize().
  virtual int NumInitialize() const
  {
    return(NumInitialize_);
  }

  //! Returns the number of calls to Compute().
  virtual int NumCompute() const
  {
    return(NumCompute_);
  }

  //! Returns the number of calls to ApplyInverse().
  virtual int NumApplyInverse() const
  {
    return(NumApplyInverse_);
  }

  //! Returns the time spent in Initialize().
  virtual double InitializeTime() const
  {
    return(InitializeTime_);
  }

  //! Returns the time spent in Compute().
  virtual double ComputeTime() const
  {
    return(ComputeTime_);
  }

  //! Returns the time spent in ApplyInverse().
  virtual double ApplyInverseTime() const
  {
    return(ApplyInverseTime_);
  }

  //! Returns the number of flops in the initialization phase.
  virtual double InitializeFlops() const
  {
    return(0.0);
  }

  //! Returns the number of flops in all applications of Compute().
  virtual double ComputeFlops() const
  {
    return(ComputeFlops_);
  }

  //! Returns the number of flops in all applications of ApplyInverse().
  virtual double ApplyInverseFlops() const
  {
    return(ApplyInverseFlops_);
  }

  //! Returns the level-of-fill 
  /*! \note: if 1.0, then the factored matrix
      contains approximatively the same number of elements of A.
   */
  inline double LevelOfFill() const
  {
    return(LevelOfFill_);
  }

  //! Returns the absolute threshold.
  inline double AbsoluteThreshold() const
  {
    return(Athresh_);
  }

  //! Returns the relative threshold.
  inline double RelativeThreshold() const
  {
    return(Rthresh_);
  }

  //! Returns the relaxation value.
  inline double RelaxValue() const
  {
    return(Relax_);
  }

  //! Returns the drop threshold.
  inline double DropTolerance() const
  {
    return(DropTolerance_);
  }

private:
  
  //! Should not be used.
  Ifpack_ICT(const Ifpack_ICT& rhs) :
    A_(rhs.Matrix()),
    Comm_(Comm()),
    Time_(Comm())
  {}

  //! Should not be used.
  Ifpack_ICT& operator=(const Ifpack_ICT& rhs)
  {
    return(*this);
  }

  //! Destroys all data associated to the preconditioner.
  void Destroy();

  //! Reference to the matrix to be preconditioned, supposed symmetric.
  const Epetra_RowMatrix& A_;
  //! Reference to the communicator.
  const Epetra_Comm& Comm_;
  //! Contains the Cholesky factorization.
  Teuchos::RefCountPtr<Epetra_CrsMatrix> H_;
  //! Contains the estimate of the condition number, if -1.0 if not computed.
  double Condest_;
  //! Absolute threshold.
  double Athresh_;
  //! Relative threshold.
  double Rthresh_;
  //! Level of fill.
  double LevelOfFill_;
  //! During factorization, drop all values below this.
  double DropTolerance_;
  //! Relaxation value.
  double Relax_;
  //! Label of \c this object.
  string Label_;
  //! If \c true, the preconditioner has been successfully initialized.
  bool IsInitialized_;
  //! If \c true, the preconditioner has been successfully computed.
  bool IsComputed_;
  //! If \c true, use the transpose of the matrix.
  bool UseTranspose_;
  //! Number of local rows in the matrix.
  int NumMyRows_;
  //! Contains the number of successful calls to Initialize().
  int NumInitialize_;
  //! Contains the number of successful call to Compute().
  int NumCompute_;
  //! Contains the number of successful call to ApplyInverse().
  mutable int NumApplyInverse_;
  //! Contains the time for all successful calls to Initialize().
  double InitializeTime_;
  //! Contains the time for all successful calls to Compute().
  double ComputeTime_;
  //! Contains the time for all successful calls to ApplyInverse().
  mutable double ApplyInverseTime_;
  //! Contains the number of flops for Compute().
  double ComputeFlops_;
  //! Contain sthe number of flops for ApplyInverse().
  mutable double ApplyInverseFlops_;
  //! Used for timing purposes.
  mutable Epetra_Time Time_;
  //! Global number of nonzeros in L and U factors
  int GlobalNonzeros_;
  Teuchos::RefCountPtr<Epetra_SerialComm> SerialComm_;
  Teuchos::RefCountPtr<Epetra_Map> SerialMap_;
};

#endif /* IFPACK_ICT_H */
