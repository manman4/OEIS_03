\\ a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/2)} (2*k+1) * binomial(n,2*k+1) * a(n-1-2*k) * A092812(k).
a092812(n) = 3*0^n/8 + 16^n/8 + 4^n/2;
for(n=0, 20, print1(a092812(n),", "));
a(n) = if(n==0, 1, sum(k=0, (n-1)\2, (2*k+1) * binomial(n, 2*k+1) * a(n-1-2*k) * a092812(k)));
for(n=0, 15, print1(a(n),", "));