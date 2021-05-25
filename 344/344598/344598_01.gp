M=61;

a(n) = sum(k=1, n, eulerphi(k)*((n\k)^2-((n-1)\k)^2));
for(n=1, M, print1(a(n), ", "));