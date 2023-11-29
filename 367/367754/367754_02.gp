seq(n) = my(A=1); for(i=1, n, A=exp(x * (1+x) * subst(A, x, x^2/2) +x*O(x^n))); Vec(serlaplace(A)); 
seq(30)        