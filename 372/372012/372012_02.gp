seq(n) = my(A=1); for(i=1, n, A=1/( 1 + x*(1 - 4*x*A)^(1/2) +x*O(x^n) ) ); Vec(A);      
seq(35)    
