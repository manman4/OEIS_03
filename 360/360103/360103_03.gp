M=20;

my(A=1, n=M); for(i=1, n, A=1/(1-x +x*O(x^n)) +x*A^2/((1-x)^4 +x*O(x^n)) ); Vec(A)
