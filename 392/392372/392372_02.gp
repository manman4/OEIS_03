M=30;

\\ E.g.f. A(x) satisfies A(x) = exp( -x * d/dx log(1 - x*A(x)) ).
my(A=1, N=M); for(k=1, N, A=exp(-x*deriv(log(1-x*A)))+x*O(x^N)); Vec(serlaplace(A)) 

\\ E.g.f. A(x) satisfies A(x) = exp( x * (A(x) + x*d/dx A(x))/(1 - x*A(x)) ).
my(A=1, N=M); for(k=1, N, A=exp(x*(A+x*deriv(A))/(1-x*A))+x*O(x^N)); Vec(serlaplace(A))