seq(n) = my(A=1); for(i=1, n, A=1 + x + x*A^(5/2) +x*O(x^n) ); Vec(A); 
seq(30)

\\ G.f.: A(x) = 1/B(-x) where B(x) is the g.f. of A366404.
seq(n) = my(B=1); for(i=1, n, B=(1+x/B^(3/2))/(1 - x +x*O(x^n)) ); Vec(1/subst(B, x, -x));  
seq(30)

\\ A^(1/2) = A259757
seq(n) = my(A=1); for(i=1, n, A=1 + x + x*A^(5/2) +x*O(x^n) ); Vec(sqrt(A));
seq(30) 