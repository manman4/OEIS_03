M=30;

my(A=1, n=M); for(i=1, n, A=1/(1 + x * (1+x)^3 * A+x*O(x^n))); Vec(A)  