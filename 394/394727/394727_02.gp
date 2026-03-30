\\ a(0) = 1; a(n) = a(n-1) + (n-1)! * Sum_{k=1..floor(n/2)} binomial(2*(k-1),k-1)/((2*k-1) * 4^(k-1)) * a(n-2*k)/(n-2*k)!.
a(n) = if(n==0, 1, a(n-1) + (n-1)! * sum(k=1, n\2, binomial(2*(k-1),k-1)/((2*k-1) * 4^(k-1)) * a(n-2*k)/(n-2*k)!));
for(n=0, 24, print1(a(n),", "));