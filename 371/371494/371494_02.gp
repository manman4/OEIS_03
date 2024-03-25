seq(n) = my(A=1); for(i=1, n, A=1/(1 - x*A/(1+x) +x*O(x^n) )^2 ); Vec(A);
seq(25)
