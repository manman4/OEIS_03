\\ a(n) = Sum_{k=0..floor(n/2)} 3^(n-2*k) * a(k) for n >= 1.
a(n) = if(n==0, 1, sum(k=0, n\2, 3^(n-2*k) * a(k)));
for(n=0, 15, print1(a(n),", "));

