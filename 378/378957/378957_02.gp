\\ G.f. A(x) satisfies A(x) = ( (1 + x * A(x)^9) / (1 - x) )^(1/2).
seq(n) = my(A=1); for(i=1, n, A=( (1 + x*A^9)/(1 - x) )^(1/2) +x*O(x^n) ); Vec(A);
seq(25)

