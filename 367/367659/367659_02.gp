seq(n) = my(A=1); for(i=1, n, A=1 / (1 - x - x * subst(A, x, x^3) +x*O(x^n)) ); Vec(A); 
seq(30)        