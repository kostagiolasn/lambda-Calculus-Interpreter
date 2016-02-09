#ifndef LAMBDAREDUCTIONENGINE_H
#define LAMBDAREDUCTIONENGINE_H

#include "LambdaSyntaxTree.h"

// 
//
//

void LambdaReductionEngine_Substitution(Term target, char *var, Term newTerm);
void LambdaReductionEngine_BetaReduction(Term target);
void LambdaReductionEngine_EtaReduction(Term target);
void LambdaReductionEngine_AlphaReduction(Term target);
void LambdaReductionEngine_ArithmeticReduction(Term target);
Term LambdaReductionEngine_GetNextRedexTerm(Term target);
int LambdaReductionEngine_GetRedexType(Term target);
int LambdaReductionEngine_Normalize(Term target);
int LambdaReductionEngine_LimitedNormalize(Term target, int redexLimit);
int LambdaReductionEngine_NormalizeStep(Term target);
void LambdaReductionEngine_ResetVarGenerator();
int LambdaReductionEngine_IsFreeVariable(Term target, char *var);


#endif

