M=24;

\\ G.f. A(x) satisfies A(x) = 1/( (1+x) * (1 - x^2*A'(x)) ).
my(A=1, n=M); for(i=1, n, A= 1/( (1 + x + x*O(x^n) ) * (1 - x^2*deriv(A)) ) ); Vec(A)  