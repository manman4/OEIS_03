M=15;

a(n) = sumdiv(n, d, d^(sigma(d)-1));
for(n=1, M, print1(a(n), ", "));