seq(n) = my(A=1); for(i=1, n, A=exp( x * (1 + A^(3/2) +x*O(x^n)) ) ); Vec(serlaplace(A));
seq(35)    

