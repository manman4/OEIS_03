M=112;

a(n) = sum(k=1, n, (n%k^k==0)*k^k);
for(n=1, M, print1(a(n), ", "));