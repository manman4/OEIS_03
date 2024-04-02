seq(n) = my(A=1); for(i=1, n, A=1 + x * A * (1+A)^2 +x*O(x^n) ); Vec(A);
seq(25)    
