seq(n) = my(A=1); for(i=1, n, A=1 + x*exp(x^3*A^3 +x*O(x^n)) ); Vec(serlaplace(A));         
seq(20)      
