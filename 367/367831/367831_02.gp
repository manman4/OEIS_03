seq(n) = my(A=1); for(i=1, n, A=(1 + (exp(x +x*O(x^n)) - 1) * subst(A, x, 3*x) +x*O(x^n)) / (1-x) ); Vec(serlaplace(A)); 
seq(20)