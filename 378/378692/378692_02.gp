\\ G.f. A(x) satisfies A(x) = 1 + x*A(x)^7/(1 - x*A(x)).
my(A=1, n=22); for(i=1, n, A= 1 + x*A^7/(1 - x*A) + x*O(x^n) ); Vec(A)


 