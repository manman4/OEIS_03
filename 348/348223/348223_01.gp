M=105;

a(n) = sumdiv(n, d, (-1)^(sigma(d)-1));
for(n=1, M, print1(a(n), ", "));