N=550;
M=500;

B(v) = v / (1 - v)^4;
A = (B(B(B(x + x*O(x^N))))/x)^(1/4);

for(n=0, M, write("b396208.txt", n, " ", polcoef(A, n)));