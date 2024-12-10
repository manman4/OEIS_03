\\ G.f. A(x) satisfies A(x) = 1 + x * A(x)^(2/3) * (1 + A(x)^(4/3) + A(x)^(5/3)).
seq(n) = my(A=1); for(i=1, n, A=1 + x*A^(2/3)*(1 + A^(4/3) + A^(5/3)) + x*O(x^n) ); Vec(A);
seq(18)

