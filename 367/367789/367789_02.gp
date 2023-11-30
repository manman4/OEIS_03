seq(n) = my(A=1); for(i=1, n, A=exp( x/(1-x)^3 * A +x*O(x^n))); Vec(serlaplace(A)); 
seq(17)  