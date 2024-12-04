\\ G.f. A(x) satisfies A(x) = 1 + x*A(x)^7/(1 - x*A(x)^6).
my(A=1, n=22); for(i=1, n, A= 1 + x*A^7/(1 - x*A^6) + x*O(x^n) ); Vec(A)

 