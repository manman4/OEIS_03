M=20;

\\ G.f.: 1/(1 - x*B(x)), where B(x) is the g.f. of A321799. 
my(A=1, n=M); for(i=1, n, A=1/( 1 - x/(1-x*A)^5 ) +x*O(x^n) ); Vec(A) 
my(A=1, n=M); for(i=1, n, A=1/( 1 - x/(1-x*A)^5 ) +x*O(x^n) ); Vec(1/(1 - x*A)) 