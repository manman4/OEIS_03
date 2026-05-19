N=550;
M=500;

B(v) = v / (1 - v)^3;
A = (B(B(B(x + x*O(x^N))))/x)^(1/3);

for(n=0, M, write("b393908.txt", n, " ", polcoef(A, n)));