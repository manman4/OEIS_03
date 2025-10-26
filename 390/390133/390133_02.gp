\\ G.f.: 2/(1 + sqrt(1 - 4*x/(1+x^2)^3)).
my(A=1, n=30); for(i=1, n, A=1 + x/(1+x^2)^2*A^2 + x*O(x^n)); Vec(A)
