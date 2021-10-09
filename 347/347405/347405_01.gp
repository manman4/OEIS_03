M=76;

a(n) = sumdiv(n, d, 2^(numdiv(d)-1));
for(n=1, M, print1(a(n), ", "));