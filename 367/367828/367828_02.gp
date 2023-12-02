seq(n) = my(A=1); for(i=1, n, A=(1 - log(1 - x +x*O(x^n)) * subst(A, x, 2*x) +x*O(x^n)) / (1-x) ); Vec(serlaplace(A)); 
seq(20)