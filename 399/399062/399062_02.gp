\\ a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/2)} (2*k+1) * binomial(n-1,2*k) * a(n-1-2*k) * A054879(k).
a054879(n) = (3^(2*n)+3)/4;
for(n=0, 20, print1(a054879(n),", "));
a(n) = if(n==0, 1, sum(k=0, (n-1)\2, (2*k+1) * binomial(n-1, 2*k) * a(n-1-2*k) * a054879(k)));
for(n=0, 15, print1(a(n),", "));