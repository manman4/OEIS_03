seq(n) = my(A=1); for(i=1, n, A=exp(x * subst(A, x, -x^3) +x*O(x^n))); Vec(serlaplace(A)); 
seq(30)        