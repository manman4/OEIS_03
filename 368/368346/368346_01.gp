a(n, m=4, k=2) = (k^(n+1)\(k^m-1)-(n+1)\m)/(k-1);
for(n=0, 35, print1(a(n), ", "));

\\ a(n) = floor(2^(n+1)/15) - floor((n+1)/4).
a(n) = 2^(n+1)\15-(n+1)\4;
for(n=0, 35, print1(a(n), ", "));

 