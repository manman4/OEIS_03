seq(n) = my(A=1); for(i=1, n, A=exp( 2*x * A^(5/2) * (1 + x*A +x*O(x^n)) ) ); Vec(serlaplace(A));      
seq(35)    

\\ A372182
seq(n) = my(A=1); for(i=1, n, A=exp( 2*x * A^(5/2) * (1 + x*A +x*O(x^n)) ) ); Vec(serlaplace(A^(1/2)));      
seq(35)    
