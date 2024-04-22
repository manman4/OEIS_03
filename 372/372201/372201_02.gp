seq(n) = my(A=1); for(i=1, n, A=exp( 3*x / (1 - x*A^(1/3) +x*O(x^n))^3 ) ); Vec(serlaplace(A));      
seq(35)    

\\ A364938
seq(n) = my(A=1); for(i=1, n, A=exp( 3*x / (1 - x*A^(1/3) +x*O(x^n))^3 ) ); Vec(serlaplace(A^(1/3)));      
seq(35)    
