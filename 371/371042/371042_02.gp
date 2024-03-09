seq(n) = my(A=1); for(i=1, n, A=1 + x^2*exp(x*A +x*O(x^n)) ); Vec(serlaplace(A));         
seq(20)      
