M=20;

my(A=1, n=M); for(i=1, n, A=1/(1-x+x*O(x^n))^2-x^2*A^2); Vec(A)   