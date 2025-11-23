M=15;

\\ a(n) = exp(-3/2) * Sum_{k>=0} 3^k * 2^(n-k) * k^n/k!.
a(n) = exp(-3/2) * sum(k=0, 500, 3.^k * 2^(n-k) * k^n / k! );
for(n=0, 10, print1(a(n),", "));

a(n) = round( exp(-3/2) * sum(k=0, 500, 3.^k * 2^(n-k) * k^n / k! ) );
for(n=0, 10, print1(a(n),", "));