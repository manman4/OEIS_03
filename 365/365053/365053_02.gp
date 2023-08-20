seq(n) = my(A=1); for(i=1, n, A=exp(x*(1 + x/2 +x*O(x^n))*A ) ); Vec(serlaplace(A));         
seq(18)
