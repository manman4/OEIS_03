M=51;

a(n) = sum(k=1, n, eulerphi(k)*((n\k)^3-((n-1)\k)^3));
for(n=1, M, print1(a(n), ", "));