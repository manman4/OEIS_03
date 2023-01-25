M=22;

my(A=1, n=M); for(i=1, n, A=1/(1 - x * (1+x)^5 * A+x*O(x^n))); Vec(A)  