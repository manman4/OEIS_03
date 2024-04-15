seq(n) = my(A=1); for(i=1, n, A=( 1 + 25*x*A*(1 + x*A) +x*O(x^n) )^(1/5) ); Vec(A);      
seq(35)    