seq(n) = my(A=1); for(i=1, n, A=1 + x/((1 + x +x*O(x^n))^(7/2)*A^(5/2)) ); Vec(A); 
seq(30)

\\ G.f.: A(x) = 1/B(-x) where B(x) is the g.f. of A366432.
seq(n) = my(B=1); for(i=1, n, B=1+x*(B/(1 - x +x*O(x^n)))^(7/2) ); Vec(1/subst(B, x, -x));  
seq(40)                