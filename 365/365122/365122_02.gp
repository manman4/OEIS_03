seq(n) = my(A=1); for(i=1, n, A=(1 + x/(1 - x*A +x*O(x^n))^3)^3 ); Vec(A);         
seq(22)

\\ G.f.: A(x) = B(x)^3 where B(x) is the g.f. of A371616. 
seq(n) = my(A=1); for(i=1, n, A=(1 + x/(1 - x*A +x*O(x^n))^3)^3 ); Vec(A^(1/3));  
seq(22)
