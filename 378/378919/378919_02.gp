\\ G.f. A(x) satisfies A(x) = 1 + x*A(x)^6/(1 + x*A(x)).
seq(n) = my(A=1); for(i=1, n, A=1 + x*A^6/(1 + x*A) +x*O(x^n) ); Vec(A);
seq(25)

