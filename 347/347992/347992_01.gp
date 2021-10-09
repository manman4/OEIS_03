M=87;

a(n) = sumdiv(n, d, (-1)^(numdiv(d)-1));
for(n=1, M, print1(a(n), ", "));