M=20;

my(A=1, n=M); for(i=1, n, A=1+x*A^2/((1-x)^5 +x*O(x^n)) ); Vec(A)
