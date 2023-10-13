M=19;

a(n) = fromdigits(digits((10^n-1)/9, 8));
for(n=0, M, print1(a(n), ", "));