seq(n) = my(A=1); for(i=1, n, A=1/( 1 - 9*x/(1 - x*A) +x*O(x^n) )^(1/3) ); Vec(A);      
seq(35)    
