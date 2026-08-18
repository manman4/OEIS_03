\\ a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/2)} (2*k+1) * binomial(n-1,2*k) * a(n-1-2*k) * A081294(k).
a081294(n) = (4^n+0^n)/2;
for(n=0, 20, print1(a081294(n),", "));
a(n) = if(n==0, 1, sum(k=0, floor((n-1)/2), (2*k+1) * binomial(n-1, 2*k) * a(n-1-2*k) * a081294(k)));
for(n=0, 15, print1(a(n),", "));