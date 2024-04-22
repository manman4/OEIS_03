seq(n) = my(A=1); for(i=1, n, A=exp( 2*x*A^(1/2) / (1 - x +x*O(x^n)) ) ); Vec(serlaplace(A));      
seq(35)    
