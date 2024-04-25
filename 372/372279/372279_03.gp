seq(n) = my(A=1); for(i=1, n, A=exp( x * (exp(x + x*O(x^n)) * A )^(5/2) ) ); Vec(serlaplace(A));
seq(35)    

