seq(n) = my(A=1); for(i=1, n, A=exp( x*(1 + 2*x +x*O(x^n))^(1/2) * A ) ); Vec(serlaplace(A));      
seq(35)    
