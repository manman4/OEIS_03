M=30;

a(n) = sumdiv(n, d, 2^(sigma(d)-1));
for(n=1, M, print1(a(n), ", "));