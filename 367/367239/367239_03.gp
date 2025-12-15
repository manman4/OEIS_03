M=20;

\\ G.f.: 1/(1 - x*B(x)^3), where B(x) is the g.f. of A391649. 
my(A=1, n=M); for(i=1, n, A=1/( 1 - x/(1-x*A^3)^2 ) +x*O(x^n) ); Vec(1/(1 - x*A^3)) 