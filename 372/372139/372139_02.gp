seq(n) = my(A=1); for(i=1, n, A=1/( 1 - x*(1 + 4*x +x*O(x^n))^(1/2) * A^2 ) ); Vec(A);      
seq(35)    
