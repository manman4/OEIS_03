M=41;

my(A=1, n=M); for(i=1, n, A=1/(1-x+x*O(x^n))^3-x*A^2); Vec(A)  