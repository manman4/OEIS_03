seq(n) = my(A=1); for(i=1, n, A=(1+x)^2 + x*A^2/(1+x) +x*O(x^n) ); Vec(A); 
seq(30)        