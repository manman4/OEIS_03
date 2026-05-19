N=550;
M=500;

B(v) = v / (1 - v)^3;
A = B(B(B(x + x*O(x^N))));

for(n=0, M, write("b396209.txt", n, " ", polcoef(A, n)));