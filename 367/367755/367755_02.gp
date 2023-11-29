seq(n) = my(A=1); for(i=1, n, A=exp(x * (1+x+x^2) * subst(A, x, x^3/6) +x*O(x^n))); Vec(serlaplace(A)); 
seq(30)        