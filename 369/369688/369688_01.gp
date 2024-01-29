seq(n) = my(A=1); for(i=1, n, A=1 + x*A + x^2*(1-x)^3*A^5 + x*O(x^n)); Vec(A);
seq(30)

