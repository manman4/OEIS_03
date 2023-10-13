M=16;

a(n) = fromdigits(digits((10^n-1)/9, 4));
for(n=0, M, print1(a(n), ", "));