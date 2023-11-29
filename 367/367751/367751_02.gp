seq(n) = my(A=1); for(i=1, n, A=exp(x * (1+x+x^2+x^3+x^4) * subst(A, x, x^5) +x*O(x^n))); Vec(serlaplace(A)); 
seq(30)        