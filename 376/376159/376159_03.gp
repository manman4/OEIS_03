\\ G.f. satisfies A(x) = 1 / ((1-x)^3 - x*A(x)).
seq(n) = my(A=1); for(i=1, n, A=1/((1-x)^3 - x*A + x*O(x^n))); Vec(A);
seq(20)