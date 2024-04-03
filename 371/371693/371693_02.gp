seq(n) = my(A=1); for(i=1, n, A=( 1 + x * A * (1+A) +x*O(x^n) )^2 ); Vec(A);
seq(25)    

\\ sqrt(A) = A219534
seq(n) = my(A=1); for(i=1, n, A=( 1 + x * A * (1+A) +x*O(x^n) )^2 ); Vec(A^(1/2));
seq(25)    

