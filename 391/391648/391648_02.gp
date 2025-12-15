M=20;

\\ G.f. A(x) satisfies A(x) = 1/(1 - x/(1 - x*A(x)^3)^4).
my(A=1, n=M); for(i=1, n, A=1/( 1 - x/(1-x*A^3)^4 ) +x*O(x^n) ); Vec(A) 

\\ G.f.: 1/(1 - x*B(x)^4), where B(x) is the g.f. of A391651.
my(B=1, n=M); for(i=1, n, B=1/( 1 - x/(1-x*B^4)^3 ) +x*O(x^n) ); Vec(B) 
my(A=1, B=1, n=M); for(i=1, n, A=1/( 1 - x/(1-x*A^3)^4 ) +x*O(x^n); B=1/( 1 - x/(1-x*B^4)^3 ) +x*O(x^n) ); Vec(A-1/(1 - x*B^4)) 