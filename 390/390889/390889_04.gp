M=15;

\\ a(n) = exp(-2/5) * Sum_{k>=0} 2^k * 5^(n-k) * k^n/k!.
a(n) = exp(-2/5) * sum(k=0, 500, 2.^k * 5^(n-k) * k^n / k! );
for(n=0, 10, print1(a(n),", "));

a(n) = round( exp(-2/5) * sum(k=0, 500, 2.^k * 5^(n-k) * k^n / k! ) );
for(n=0, 10, print1(a(n),", "));