M=37;

a(n) = sum(k=1, n, k^3*eulerphi(k));
for(n=1, M, print1(a(n), ", "));