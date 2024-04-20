seq(n) = my(A=1); for(i=1, n, A=1/( 1 - x*(1 + 9*x +x*O(x^n))^(1/3) * A ) ); Vec(A);      
seq(35)    
