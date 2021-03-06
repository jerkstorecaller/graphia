//=================================================================================================
/*!
//  \file blaze/math/traits/DVecScalarDivExprTrait.h
//  \brief Header file for the DVecScalarDivExprTrait class template
//
//  Copyright (C) 2013 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. You can redistribute it and/or modify it under
//  the terms of the New (Revised) BSD License. Redistribution and use in source and binary
//  forms, with or without modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. Neither the names of the Blaze development group nor the names of its contributors
//     may be used to endorse or promote products derived from this software without specific
//     prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
*/
//=================================================================================================

#ifndef _BLAZE_MATH_TRAITS_DVECSCALARDIVEXPRTRAIT_H_
#define _BLAZE_MATH_TRAITS_DVECSCALARDIVEXPRTRAIT_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blaze/math/expressions/Forward.h>
#include <blaze/math/traits/DivTrait.h>
#include <blaze/math/typetraits/IsColumnVector.h>
#include <blaze/math/typetraits/IsDenseVector.h>
#include <blaze/math/typetraits/IsInvertible.h>
#include <blaze/math/typetraits/UnderlyingBuiltin.h>
#include <blaze/math/typetraits/UnderlyingNumeric.h>
#include <blaze/util/InvalidType.h>
#include <blaze/util/mpl/And.h>
#include <blaze/util/mpl/If.h>
#include <blaze/util/mpl/Or.h>
#include <blaze/util/typetraits/Decay.h>
#include <blaze/util/typetraits/IsBuiltin.h>
#include <blaze/util/typetraits/IsComplex.h>
#include <blaze/util/typetraits/IsConst.h>
#include <blaze/util/typetraits/IsFloatingPoint.h>
#include <blaze/util/typetraits/IsNumeric.h>
#include <blaze/util/typetraits/IsReference.h>
#include <blaze/util/typetraits/IsVolatile.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Auxiliary helper struct for the DVecScalarDivExprTrait trait.
// \ingroup math_traits
*/
template< typename VT
        , typename ST
        , bool Condition >
struct DVecScalarDivExprTraitHelper
{
 private:
   //**********************************************************************************************
   using ScalarType = If_< Or< IsFloatingPoint< UnderlyingBuiltin_<VT> >
                             , IsFloatingPoint< UnderlyingBuiltin_<ST> > >
                         , If_< And< IsComplex< UnderlyingNumeric_<VT> >
                                   , IsBuiltin<ST> >
                              , DivTrait_< UnderlyingBuiltin_<VT>, ST >
                              , DivTrait_< UnderlyingNumeric_<VT>, ST > >
                         , ST >;
   //**********************************************************************************************

 public:
   //**********************************************************************************************
   using Type = If_< IsInvertible<ScalarType>
                   , DVecScalarMultExpr<VT,ScalarType,false>
                   , DVecScalarDivExpr<VT,ScalarType,false> >;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of the DVecScalarDivExprTraitHelper class template.
// \ingroup math_traits
*/
template< typename VT
        , typename ST >
struct DVecScalarDivExprTraitHelper<VT,ST,false>
{
 public:
   //**********************************************************************************************
   using Type = INVALID_TYPE;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Evaluation of the expression type of a dense vector/scalar division.
// \ingroup math_traits
//
// Via this type trait it is possible to evaluate the resulting expression type of a dense
// vector/scalar division. Given the non-transpose dense vector type \a VT and the scalar type
// \a ST, the nested type \a Type corresponds to the resulting expression type. In case either
// \a VT is not a non-transpose dense vector type or \a ST is not a scalar type, the resulting
// \a Type is set to \a INVALID_TYPE.
*/
template< typename VT    // Type of the left-hand side dense vector
        , typename ST >  // Type of the right-hand side scalar
struct DVecScalarDivExprTrait
{
 private:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   enum : bool { condition = And< IsDenseVector<VT>, IsColumnVector<VT>, IsNumeric<ST> >::value };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   using Type = typename If_< Or< IsConst<VT>, IsVolatile<VT>, IsReference<VT>
                                , IsConst<ST>, IsVolatile<ST>, IsReference<ST> >
                            , DVecScalarDivExprTrait< Decay_<VT>, Decay_<ST> >
                            , DVecScalarDivExprTraitHelper<VT,ST,condition> >::Type;
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Auxiliary alias declaration for the DVecScalarDivExprTrait class template.
// \ingroup math_traits
//
// The DVecScalarDivExprTrait_ alias declaration provides a convenient shortcut to access
// the nested \a Type of the DVecScalarDivExprTrait class template. For instance, given the
// non-transpose dense vector type \a VT and the scalar type \a ST the following two type
// definitions are identical:

   \code
   using Type1 = typename DVecScalarDivExprTrait<VT,ST>::Type;
   using Type2 = DVecScalarDivExprTrait_<VT,ST>;
   \endcode
*/
template< typename VT    // Type of the left-hand side dense vector
        , typename ST >  // Type of the right-hand side scalar
using DVecScalarDivExprTrait_ = typename DVecScalarDivExprTrait<VT,ST>::Type;
//*************************************************************************************************

} // namespace blaze

#endif
