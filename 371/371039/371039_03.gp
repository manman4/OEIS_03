seq(n) = my(A=1); for(i=1, n, A=exp(x^3*A +x*O(x^n)) / (1-x + x*O(x^n)) ); Vec(serlaplace(A));         
seq(20)      
